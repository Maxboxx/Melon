#include "DefaultNode.h"

#include "MemoryNode.h"
#include "TypeNode.h"

#include "Melon/Symbols/VariableSymbol.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

DefaultNode::DefaultNode(Symbols::Symbol* const scope, const FileInfo& file) : BinaryOperatorNode(scope, Name::Default, file) {

}

DefaultNode::~DefaultNode() {

}

TypeSymbol* DefaultNode::Type() const {
	TypeSymbol* const type1 = operand1->Type();

	// TODO: error?
	if (type1 == nullptr) return nullptr;

	Symbols::Symbol* const value = type1->Contains(Name::Value);

	// TODO: error?
	if (value == nullptr) return nullptr;

	TypeSymbol* const type = value->Type();

	// TODO: error?
	if (type == nullptr) return nullptr;

	if (SymbolTable::FindImplicitConversion(operand2->Type(), type, file)) {
		ErrorLog::Error(LogMessage("error.type.default"), file);
	}

	return type;
}

CompiledNode DefaultNode::Compile(CompileInfo& info) {
	CompiledNode cn;
	cn.size = info.stack.ptrSize;

	List<UInt> jumps;

	info.stack.PushExpr(Type()->Size(), cn);
	cn.argument = Argument(MemoryLocation(info.stack.Offset()));

	// Compile the optional value
	CompiledNode c1 = operand1->Compile(info);
	cn.AddInstructions(c1.instructions);
	cn.size = c1.size - 1;

	UInt eqIndex = cn.instructions.Size();

	// Check if the optional has a value
	Instruction eq = Instruction(InstructionType::Eq, 1);
	eq.arguments.Add(c1.argument);
	eq.arguments.Add(Argument(0));
	cn.instructions.Add(eq);

	// Get memory location for the result and the optional value
	Pointer<MemoryNode> sn1 = new MemoryNode(cn.argument.mem);
	sn1->type = Type()->AbsoluteName();

	Pointer<MemoryNode> sn2 = new MemoryNode(c1.argument.mem);
	sn2->mem.offset++;
	sn2->type = operand1->Type()->Find<VariableSymbol>(Name::Value, file)->Type()->AbsoluteName();

	// Assign the optional value to the result
	cn.AddInstructions(CompileAssignment(sn1, sn2, info, file).instructions);

	// And jump to end
	UInt jmp = cn.instructions.Size();
	cn.instructions.Add(Instruction(InstructionType::Jmp));

	// Add label for default value
	cn.instructions.Add(Instruction::Label(info.label));
	cn.instructions[eqIndex].instruction.arguments.Add(Argument(ArgumentType::Label, info.label++));

	// Assign default value to result
	cn.AddInstructions(CompileAssignment(sn1, operand2, info, file).instructions);

	// Add label to the end
	cn.instructions.Add(Instruction::Label(info.label));
	cn.instructions[jmp].instruction.arguments.Add(Argument(ArgumentType::Label, info.label++));

	info.stack.Pop(Type()->Size());

	return cn;
}

void DefaultNode::IncludeScan(ParsingInfo& info) {
	operand1->IncludeScan(info);
	operand2->IncludeScan(info);
}

ScanResult DefaultNode::Scan(ScanInfoStack& info) {
	ScopeInfo scopeInfo = info->scopeInfo.CopyBranch();

	ScanResult result1 = operand1->Scan(info);
	result1.SelfUseCheck(info, operand1->File());

	ScanResult result2 = operand2->Scan(info);
	result2.SelfUseCheck(info, operand2->File());

	TypeSymbol* const type = Type();
	
	if (type) {
		ScanAssignment(new TypeNode(type->AbsoluteName()), operand2, info, file);
	}

	info->scopeInfo = scopeInfo;

	return result1 | result2;
}

NameList DefaultNode::FindSideEffectScope(const bool assign) {
	return CombineSideEffects(operand1->GetSideEffectScope(assign), operand2->GetSideEffectScope(assign));
}

Expression DefaultNode::Optimize(OptimizeInfo& info) {
	Node::Optimize(operand1, info);
	Node::Optimize(operand2, info);
	return nullptr;
}

StringBuilder DefaultNode::ToMelon(const UInt indent) const {
	StringBuilder sb = operand1->ToMelon(indent);
	sb += " ?? ";
	sb += operand2->ToMelon(indent);
	return sb;
}