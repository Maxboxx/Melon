#include "DefaultNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

DefaultNode::DefaultNode(const ScopeList& scope, const FileInfo& file) : BinaryOperatorNode(scope, Scope::Default, file) {

}

DefaultNode::~DefaultNode() {

}

ScopeList DefaultNode::Type() const {
	ScopeList type = node1->Type();
	type = Symbol::Find(type, file).Get(Scope::Value, file).varType;

	if (type != node2->Type()) ErrorLog::Error(TypeError(TypeError::DefaultType, file));

	return type;
}

CompiledNode DefaultNode::Compile(CompileInfo& info) {
	CompiledNode cn;
	cn.size = info.stack.ptrSize;

	List<UInt> jumps;

	info.stack.Push(Symbol::Find(Type(), file).size);
	cn.argument = Argument(MemoryLocation(info.stack.Offset()));

	CompiledNode c1 = node1->Compile(info);
	cn.AddInstructions(c1.instructions);
	cn.size = c1.size - 1;

	UInt eqIndex = cn.instructions.Size();

	Instruction eq = Instruction(InstructionType::Eq, 1);
	eq.arguments.Add(MemoryLocation(c1.argument.mem.offset + Symbol::Find((node1->Type()), file).size - 1));
	eq.arguments.Add(Argument(0));
	cn.instructions.Add(eq);

	Instruction mov1 = Instruction(InstructionType::Mov, c1.size - 1);
	mov1.arguments.Add(cn.argument);
	mov1.arguments.Add(c1.argument);
	cn.instructions.Add(mov1);

	UInt jmp = cn.instructions.Size();
	cn.instructions.Add(Instruction(InstructionType::Jmp));

	cn.instructions.Add(Instruction::Label(info.label));
	cn.instructions[eqIndex].instruction.arguments.Add(Argument(ArgumentType::Label, info.label++));

	CompiledNode c2 = node2->Compile(info);
	cn.AddInstructions(c2.instructions);

	Instruction mov2 = Instruction(InstructionType::Mov, c2.size);
	mov2.arguments.Add(cn.argument);
	mov2.arguments.Add(c2.argument);
	cn.instructions.Add(mov2);

	cn.instructions.Add(Instruction::Label(info.label));
	cn.instructions[jmp].instruction.arguments.Add(Argument(ArgumentType::Label, info.label++));

	info.stack.Pop(Symbol::Find(Type(), file).size);

	return cn;
}

void DefaultNode::IncludeScan(ParsingInfo& info) {
	node1->IncludeScan(info);
	node2->IncludeScan(info);
}

Set<ScanType> DefaultNode::Scan(ScanInfoStack& info) {
	Set<ScanType> scanSet = Set<ScanType>();

	ScopeInfo scopeInfo = info.Get().scopeInfo.CopyBranch();

	for (const ScanType type : node1->Scan(info)) {
		scanSet.Add(type);

		if (info.Get().init && type == ScanType::Self && !info.Get().symbol.IsAssigned()) {
			ErrorLog::Error(CompileError(CompileError::SelfInit, node1->file));
		}
	}

	for (const ScanType type : node2->Scan(info)) {
		scanSet.Add(type);

		if (info.Get().init && type == ScanType::Self && !info.Get().symbol.IsAssigned()) {
			ErrorLog::Error(CompileError(CompileError::SelfInit, node2->file));
		}
	}

	Symbol::Find(Type(), file);

	info.Get().scopeInfo = scopeInfo;
	return scanSet;
}

Mango DefaultNode::ToMango() const {
	Mango mango = Mango("??", MangoType::List);
	mango.Add(node1->ToMango());
	mango.Add(node2->ToMango());
	return mango;
}