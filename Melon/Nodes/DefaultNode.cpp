#include "DefaultNode.h"

#include "MemoryNode.h"
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

Symbol* DefaultNode::Type() const {
	Symbol* type = node1->Type();
	/* TODO: node
	type = Symbols::Find(type, file).Get(Scope::Value, file).varType;

	if (!Symbols::HasImplicitConversion(node2->Type(), type)) ErrorLog::Error(TypeError(TypeError::DefaultType, file));
	*/

	return type;
}

CompiledNode DefaultNode::Compile(CompileInfo& info) {
	CompiledNode cn;
	cn.size = info.stack.ptrSize;

	List<UInt> jumps;

	/* TODO: node
	info.stack.PushExpr(Symbols::Find(Type(), file).size, cn);
	cn.argument = Argument(MemoryLocation(info.stack.Offset()));

	CompiledNode c1 = node1->Compile(info);
	cn.AddInstructions(c1.instructions);
	cn.size = c1.size - 1;

	UInt eqIndex = cn.instructions.Size();

	Instruction eq = Instruction(InstructionType::Eq, 1);
	eq.arguments.Add(c1.argument);
	eq.arguments.Add(Argument(0));
	cn.instructions.Add(eq);

	Pointer<MemoryNode> sn1 = new MemoryNode(cn.argument.mem);
	sn1->type = Type();

	Pointer<MemoryNode> sn2 = new MemoryNode(c1.argument.mem);
	sn2->mem.offset++;

	sn2->type = Symbols::Find(node1->Type(), file).Get(Scope::Value, file).varType;

	cn.AddInstructions(CompileAssignment(sn1, sn2, info, file).instructions);

	UInt jmp = cn.instructions.Size();
	cn.instructions.Add(Instruction(InstructionType::Jmp));

	cn.instructions.Add(Instruction::Label(info.label));
	cn.instructions[eqIndex].instruction.arguments.Add(Argument(ArgumentType::Label, info.label++));

	cn.AddInstructions(CompileAssignment(sn1, node2, info, file).instructions);

	cn.instructions.Add(Instruction::Label(info.label));
	cn.instructions[jmp].instruction.arguments.Add(Argument(ArgumentType::Label, info.label++));

	info.stack.Pop(Symbols::Find(Type(), file).size);
	*/

	return cn;
}

void DefaultNode::IncludeScan(ParsingInfo& info) {
	node1->IncludeScan(info);
	node2->IncludeScan(info);
}

Set<ScanType> DefaultNode::Scan(ScanInfoStack& info) {
	Set<ScanType> scanSet = Set<ScanType>();

	ScopeInfo scopeInfo = info.Get().scopeInfo.CopyBranch();

	/* TODO: node
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

	Symbols::Find(Type(), file);
	ScanAssignment(new TypeNode(Type()), new TypeNode(Symbols::Find(node1->Type(), node1->file).Get(Scope::Value, node1->file).varType), info, file);
	ScanAssignment(new TypeNode(Type()), node2, info, file);
	*/

	info.Get().scopeInfo = scopeInfo;
	return scanSet;
}

ScopeList DefaultNode::FindSideEffectScope(const bool assign) {
	return CombineSideEffects(node1->GetSideEffectScope(assign), node2->GetSideEffectScope(assign));
}

NodePtr DefaultNode::Optimize(OptimizeInfo& info) {
	if (NodePtr node = node1->Optimize(info)) node1 = node;
	if (NodePtr node = node2->Optimize(info)) node2 = node;

	return nullptr;
}

StringBuilder DefaultNode::ToMelon(const UInt indent) const {
	StringBuilder sb = node1->ToMelon(indent);
	sb += " ?? ";
	sb += node2->ToMelon(indent);
	return sb;
}