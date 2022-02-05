#include "Condition.h"

#include "ConvertNode.h"
#include "Assignment.h"
#include "Boolean.h"
#include "KiwiExpression.h"
#include "TypeNode.h"
#include "DiscardNode.h"

#include "Melon/Symbols/VariableSymbol.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

Condition::Condition(Symbols::Symbol* const scope, const FileInfo& file) : Node(scope, file) {

}

Condition::~Condition() {
	
}

bool Condition::IsImmediate() const {
	return !assign && cond->IsImmediate();
}

Boxx::Long Condition::GetImmediate() const {
	return !assign ? cond->GetImmediate() : 0;
}

TypeSymbol* Condition::Type() const {
	return assign ? (TypeSymbol*)SymbolTable::Bool : cond->Type();
}

UInt Condition::GetSize() const {
	return assign ? assign->GetSize() : 0;
}

CompiledNode Condition::CompileAssignCondition(CompileInfo& info) {
	TypeSymbol* const type = assign->values[0]->Type();

	// Compile value
	CompiledNode c = assign->values[0]->Compile(info);
	Argument argCopy = c.argument;
	argCopy.mem.offset++;

	// Temporary replaces the value node with argCopy
	Ptr<KiwiExpression> value = new KiwiExpression(argCopy);
	value->type = type->Find<VariableSymbol>(Name::Value, file)->Type()->AbsoluteName();

	Ptr<Expression> tempValue = assign->values[0];
	assign->values[0] = value;

	// Stores the condition result in a register
	OptimizerInstruction mov = Instruction(InstructionType::Mov, 1);
	mov.instruction.arguments.Add(Argument(Register(info.index++)));
	mov.instruction.arguments.Add(c.argument);
	//mov.important = true;
	c.argument = mov.instruction.arguments[0];
	c.instructions.Add(mov);

	// Checks if the condition is false
	Instruction eq = Instruction(InstructionType::Eq, 1);
	eq.arguments.Add(c.argument);
	eq.arguments.Add(Argument(0));

	UInt eqIndex = c.instructions.Size();
	c.instructions.Add(eq);

	// Compile assignment and restore the value node
	c.AddInstructions(assign->Compile(info).instructions);
	assign->values[0] = tempValue;

	// Add a jump to label for skipping assignment
	c.instructions[eqIndex].instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
	c.instructions.Add(Instruction::Label(info.label++));

	return c;
}

CompiledNode Condition::Compile(CompileInfo& info) {
	// Compile assign condition
	if (assign) {
		return CompileAssignCondition(info);
	}
	// Compile regular condition
	else {
		Pointer<TypeConversion> convert = new TypeConversion(scope, file);
		convert->isExplicit = true;
		convert->expression = cond;
		convert->type = NameList::Bool;
		return convert->Compile(info);
	}
}

void Condition::IncludeScan(ParsingInfo& info) {
	cond->IncludeScan(info);
}

ScanResult Condition::Scan(ScanInfoStack& info) {
	// Scan assignment condition
	if (assign) {
		Ptr<Expression> tempValue = assign->values[0];
		
		TypeSymbol* const type = assign->values[0]->Type();

		// Check if the types match
		if (type && type->AbsoluteName()[0].name == Name::Optional.name) {
			assign->values[0] = new TypeNode(type->Find(Name::Value, file)->Type()->AbsoluteName());
		}
		else {
			ErrorLog::Error(LogMessage("error.type.conditional_assign", tempValue->Type()->ToString()), tempValue->File());
		}

		ScanResult result = cond->Scan(info) | tempValue->Scan(info);
		assign->values[0] = tempValue;
		return result;
	}
	// Scan regular condition
	else {
		Pointer<TypeConversion> convert = new TypeConversion(scope, file);
		convert->isExplicit = true;
		convert->expression = cond;
		convert->type = NameList::Bool;
		return convert->Scan(info);
	}
}

NameList Condition::FindSideEffectScope(const bool assign) {
	return cond->GetSideEffectScope(assign);
}

Ptr<Condition> Condition::Optimize(OptimizeInfo& info) {
	// Optimize assignment
	if (assign) {
		assign->OptimizeAsCondition(info);

		if (assign->assignableValues.IsEmpty() || assign->assignableValues[0].Is<DiscardExpression>()) {
			cond   = assign->values[0];
			assign = nullptr;
		}
	}
	// Optimize regular condition
	else {
		Node::Optimize(cond, info);
	}

	// Replace condition with immediate value
	if (cond->IsImmediate()) {
		Ptr<BooleanNode> bn = new BooleanNode(cond->File());
		bn->boolean = cond->GetImmediate() != 0;
		info.optimized = true;
		return bn;
	}

	return nullptr;
}

StringBuilder Condition::ToMelon(const UInt indent) const {
	return cond->ToMelon(indent);
}
