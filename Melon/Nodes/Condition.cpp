#include "Condition.h"

#include "TypeConversion.h"
#include "Assignment.h"
#include "Boolean.h"
#include "KiwiExpression.h"
#include "TypeExpression.h"
#include "DiscardExpression.h"

#include "Melon/Symbols/VariableSymbol.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;
using namespace KiwiOld;

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
	return !assign && expression->IsImmediate();
}

Boxx::Long Condition::GetImmediate() const {
	return !assign ? expression->GetImmediate() : 0;
}

TypeSymbol* Condition::Type() const {
	return assign ? (TypeSymbol*)SymbolTable::Bool : expression->Type();
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
		Ptr<TypeConversion> convert = new TypeConversion(scope, file);
		convert->isExplicit = true;
		convert->expression = new WeakExpression(expression);
		convert->type = NameList::Bool;
		return convert->Compile(info);
	}
}

void Condition::IncludeScan(ParsingInfo& info) {
	expression->IncludeScan(info);
}

ScanResult Condition::Scan(ScanInfoStack& info) {
	// Scan assignment condition
	if (assign) {
		Ptr<Expression> tempValue = assign->values[0];
		
		TypeSymbol* const type = assign->values[0]->Type();

		// Check if the types match
		if (type && type->AbsoluteName()[0].name == Name::Optional.name) {
			assign->values[0] = new TypeExpression(type->Find(Name::Value, file)->Type()->AbsoluteName());
		}
		else {
			ErrorLog::Error(LogMessage("error.type.conditional_assign", tempValue->Type()->ToString()), tempValue->File());
		}

		ScanResult result = expression->Scan(info) | tempValue->Scan(info);
		assign->values[0] = tempValue;
		return result;
	}
	// Scan regular condition
	else {
		Ptr<TypeConversion> convert = new TypeConversion(scope, file);
		convert->isExplicit = true;
		convert->expression = new WeakExpression(expression);
		convert->type = NameList::Bool;
		return convert->Scan(info);
	}
}

NameList Condition::FindSideEffectScope(const bool assign) {
	return expression->GetSideEffectScope(assign);
}

Ptr<Condition> Condition::Optimize(OptimizeInfo& info) {
	// Optimize assignment
	if (assign) {
		assign->OptimizeAsCondition(info);

		if (assign->assignableValues.IsEmpty() || assign->assignableValues[0].Is<DiscardExpression>()) {
			expression = assign->values[0];
			assign = nullptr;
		}
	}
	// Optimize regular condition
	else {
		Node::Optimize(expression, info);
	}

	// Replace condition with immediate value
	if (expression->IsImmediate()) {
		Ptr<Boolean> bn = new Boolean(expression->File());
		bn->value = expression->GetImmediate() != 0;
		info.optimized = true;
		return FromExpression(bn);
	}

	return nullptr;
}

StringBuilder Condition::ToMelon(const UInt indent) const {
	return expression->ToMelon(indent);
}

Ptr<Condition> Condition::FromExpression(Ptr<Expression> expression) {
	Ptr<Condition> cond = new Condition(expression->scope, expression->File());
	cond->expression = expression;
	return cond;
}
