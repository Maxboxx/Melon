#include "OptionalAssignValueNode.h"

#include "Melon/Symbols/NameList.h"
#include "Melon/Symbols/TypeSymbol.h"
#include "Melon/Symbols/StructSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"

#include "Melon/Nodes/KiwiVariable.h"
#include "Melon/Nodes/KiwiValue.h"
#include "Melon/Nodes/Boolean.h"

#include "Kiwi/Old/Kiwi.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

Ptr<Kiwi::Value> OptionalAssignValueNode::Compile(Weak<Expression> operand1, Weak<Expression> operand2, CompileInfo& info, bool includeType) const {
	Ptr<Kiwi::Variable> optional = operand1->Compile(info).AsPtr<Kiwi::Variable>();

	StructSymbol* const type = operand1->Type()->Cast<StructSymbol>();

	if (Node::IncludeType(optional, includeType)) {
		info.AddInstruction(new Kiwi::AssignInstruction(type->KiwiType(), optional->Copy(), nullptr));
	}

	if (operand2->Type() == SymbolTable::Nil) {
		info.AddInstruction(new Kiwi::AssignInstruction(new Kiwi::SubVariable(optional->Copy(), type->Find(Name::HasValue, operand1->File())->KiwiName()), new Kiwi::Integer(0)));
	}
	else {
		Ptr<KiwiVariable> var = new KiwiVariable(new Kiwi::SubVariable(optional->Copy(), type->Find(Name::Value, operand1->File())->KiwiName()), type->Find<VariableSymbol>(NameList(Name::Value), operand1->File())->Type()->AbsoluteName());
		Ptr<KiwiValue> value = new KiwiValue(operand2->Compile(info), operand2->Type()->AbsoluteName());
		
		info.AddInstruction(new Kiwi::AssignInstruction(new Kiwi::SubVariable(optional->Copy(), type->Find(Name::HasValue, operand1->File())->KiwiName()), new Kiwi::Integer(1)));
		Node::CompileAssignment(var, value, info, operand2->File(), false);
	}

	return nullptr;
}
