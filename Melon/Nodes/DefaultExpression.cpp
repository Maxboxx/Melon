#include "DefaultExpression.h"

#include "KiwiMemoryExpression.h"
#include "TypeExpression.h"

#include "Melon/Symbols/VariableSymbol.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

DefaultExpression::DefaultExpression(Symbols::Symbol* const scope, const FileInfo& file) : BinaryOperatorExpression(scope, Name::Default, file) {

}

DefaultExpression::~DefaultExpression() {

}

TypeSymbol* DefaultExpression::Type() const {
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

CompiledNode DefaultExpression::Compile(CompileInfo& info) {
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
	Ptr<KiwiMemoryExpression> sn1 = new KiwiMemoryExpression(cn.argument.mem, Type()->AbsoluteName());
	Ptr<KiwiMemoryExpression> sn2 = new KiwiMemoryExpression(c1.argument.mem.offset + 1, operand1->Type()->Find<VariableSymbol>(Name::Value, file)->Type()->AbsoluteName());

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

void DefaultExpression::IncludeScan(ParsingInfo& info) {
	operand1->IncludeScan(info);
	operand2->IncludeScan(info);
}

ScanResult DefaultExpression::Scan(ScanInfoStack& info) {
	ScopeInfo scopeInfo = info->scopeInfo.CopyBranch();

	ScanResult result1 = operand1->Scan(info);
	result1.SelfUseCheck(info, operand1->File());

	ScanResult result2 = operand2->Scan(info);
	result2.SelfUseCheck(info, operand2->File());

	TypeSymbol* const type = Type();
	
	if (type) {
		Ptr<TypeExpression> tn = new TypeExpression(type->AbsoluteName());
		ScanAssignment(tn, operand2, info, file);
	}

	info->scopeInfo = scopeInfo;

	return result1 | result2;
}

NameList DefaultExpression::FindSideEffectScope(const bool assign) {
	return CombineSideEffects(operand1->GetSideEffectScope(assign), operand2->GetSideEffectScope(assign));
}

Ptr<Expression> DefaultExpression::Optimize(OptimizeInfo& info) {
	Node::Optimize(operand1, info);
	Node::Optimize(operand2, info);
	return nullptr;
}

StringBuilder DefaultExpression::ToMelon(const UInt indent) const {
	StringBuilder sb = operand1->ToMelon(indent);
	sb += " ?? ";
	sb += operand2->ToMelon(indent);
	return sb;
}