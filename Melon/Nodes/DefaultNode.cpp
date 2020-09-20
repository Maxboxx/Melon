#include "DefaultNode.h"

#include "StackNode.h"
#include "TypeNode.h"

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
	eq.arguments.Add(c1.argument);
	eq.arguments.Add(Argument(0));
	cn.instructions.Add(eq);

	Pointer<StackNode> sn1 = new StackNode(cn.argument.mem.offset);
	sn1->type = Type();

	Pointer<StackNode> sn2 = new StackNode(c1.argument.mem.offset + 1);

	if (c1.argument.mem.reg.type == RegisterType::Register) {
		sn2->regIndex = c1.argument.mem.reg.index;
	}

	sn2->type = Symbol::Find(node1->Type(), file).Get(Scope::Value, file).varType;

	cn.AddInstructions(CompileAssignment(sn1, sn2, info, file).instructions);

	UInt jmp = cn.instructions.Size();
	cn.instructions.Add(Instruction(InstructionType::Jmp));

	cn.instructions.Add(Instruction::Label(info.label));
	cn.instructions[eqIndex].instruction.arguments.Add(Argument(ArgumentType::Label, info.label++));

	cn.AddInstructions(CompileAssignment(sn1, node2, info, file).instructions);

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
	ScanAssignment(new TypeNode(Type()), new TypeNode(Symbol::Find(node1->Type(), node1->file).Get(Scope::Value, node1->file).varType), info, file);
	ScanAssignment(new TypeNode(Type()), node2, info, file);

	info.Get().scopeInfo = scopeInfo;
	return scanSet;
}

Mango DefaultNode::ToMango() const {
	Mango mango = Mango("??", MangoType::List);
	mango.Add(node1->ToMango());
	mango.Add(node2->ToMango());
	return mango;
}