#include "EnumStructAssignNode.h"

#include "Melon/Symbols/EnumSymbol.h"

#include "Kiwi/Old/Kiwi.h"

#include "Melon/Nodes/KiwiVariable.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Optimizing;
using namespace Melon::Symbols::Nodes;

Ptr<Kiwi::Value> EnumStructAssignNode::Compile(Weak<Expression> operand, CompileInfo& info, bool includeType) const {
	EnumSymbol* const type = operand->Type()->Parent<EnumSymbol>();
	Kiwi::Type valueType = type->IdentifierType()->KiwiType();

	Ptr<Kiwi::Variable> var = new Kiwi::Variable(info.NewRegister());

	info.AddInstruction(new Kiwi::AssignInstruction(type->KiwiType(), var->Copy(), nullptr));
	info.AddInstruction(new Kiwi::AssignInstruction(new Kiwi::SubVariable(var->Copy(), Name::Value.name), new Kiwi::Integer(valueType, value)));

	Ptr<KiwiVariable> kiwiVar = new KiwiVariable(new Kiwi::SubVariable(var->Copy(), Name::Items.name), operand->Type()->AbsoluteName());

	Node::CompileAssignmentSimple(kiwiVar, operand, info, operand->File(), false);

	return var;
}

Ptr<Kiwi::Value> EnumStructAssignNode::Compile(Weak<Expression> operand1, Weak<Expression> operand2, CompileInfo& info, bool includeType) const {
	EnumSymbol* const type = operand1->Type()->Cast<EnumSymbol>();
	Kiwi::Type valueType = type->IdentifierType()->KiwiType();

	Ptr<Kiwi::Variable> var = operand1->Compile(info).AsPtr<Kiwi::Variable>();

	if (Node::IncludeType(var, includeType)) {
		info.AddInstruction(new Kiwi::AssignInstruction(type->KiwiType(), var->Copy(), nullptr));
	}

	info.AddInstruction(new Kiwi::AssignInstruction(new Kiwi::SubVariable(var->Copy(), Name::Value.name), new Kiwi::Integer(valueType, value)));

	Ptr<KiwiVariable> kiwiVar = new KiwiVariable(new Kiwi::SubVariable(var->Copy(), Name::Items.name), operand2->Type(info.PeekExpectedType())->AbsoluteName());

	return Node::CompileAssignmentSimple(kiwiVar, operand2, info, operand1->File(), false);
}