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

DefaultNode::DefaultNode(Symbol* const scope, const FileInfo& file) : BinaryOperatorNode(scope, Scope::Default, file) {

}

DefaultNode::~DefaultNode() {

}

TypeSymbol* DefaultNode::Type() const {
	TypeSymbol* const type1 = node1->Type();

	// TODO: error?
	if (type1 == nullptr) return nullptr;

	Symbol* const value = type1->Contains(Scope::Value);

	// TODO: error?
	if (value == nullptr) return nullptr;

	TypeSymbol* const type = value->Type();

	// TODO: error?
	if (type == nullptr) return nullptr;

	if (node2->Type()->ImplicitConversionTo(type)) {
		ErrorLog::Error(TypeError(TypeError::DefaultType, file));
	}

	return type;
}

CompiledNode DefaultNode::Compile(CompileInfo& info) {
	CompiledNode cn;
	cn.size = info.stack.ptrSize;

	List<UInt> jumps;

	info.stack.PushExpr(Type()->Size(), cn);
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
	sn1->type = Type()->AbsoluteName();

	Pointer<MemoryNode> sn2 = new MemoryNode(c1.argument.mem);
	sn2->mem.offset++;

	sn2->type = node1->Type()->Find<VariableSymbol>(Scope::Value, file)->Type()->AbsoluteName();

	cn.AddInstructions(CompileAssignment(sn1, sn2, info, file).instructions);

	UInt jmp = cn.instructions.Size();
	cn.instructions.Add(Instruction(InstructionType::Jmp));

	cn.instructions.Add(Instruction::Label(info.label));
	cn.instructions[eqIndex].instruction.arguments.Add(Argument(ArgumentType::Label, info.label++));

	cn.AddInstructions(CompileAssignment(sn1, node2, info, file).instructions);

	cn.instructions.Add(Instruction::Label(info.label));
	cn.instructions[jmp].instruction.arguments.Add(Argument(ArgumentType::Label, info.label++));

	info.stack.Pop(Type()->Size());

	return cn;
}

void DefaultNode::IncludeScan(ParsingInfo& info) {
	node1->IncludeScan(info);
	node2->IncludeScan(info);
}

ScanResult DefaultNode::Scan(ScanInfoStack& info) {
	ScopeInfo scopeInfo = info.ScopeInfo().CopyBranch();

	ScanResult result1 = node1->Scan(info);
	result1.SelfUseCheck(info, node1->file);

	ScanResult result2 = node2->Scan(info);
	result2.SelfUseCheck(info, node2->file);

	TypeSymbol* const type = Type();
	
	if (type) {
		ScanAssignment(new TypeNode(type->AbsoluteName()), node2, info, file);
	}

	info.ScopeInfo(scopeInfo);

	return result1 | result2;
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