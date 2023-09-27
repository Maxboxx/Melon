#include "EnumCompareNode.h"

#include "Melon/Symbols/IntegerSymbol.h"

#include "Melon/Optimizing/OptimizerInstruction.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Optimizing;
using namespace Melon::Symbols::Nodes;

Ptr<Kiwi::Value> EnumCompareNode::Compile(Weak<Expression> operand1, Weak<Expression> operand2, CompileInfo& info, bool includeType) const {
	Ptr<Kiwi::Value> value1 = operand1->Compile(info);
	Ptr<Kiwi::Value> value2 = operand2->Compile(info);
	Ptr<Kiwi::Variable> var = new Kiwi::Variable(info.NewRegister());

	Ptr<Kiwi::Expression> expr;

	if (op == Name::Equal) {
		expr = new Kiwi::EqualExpression(value1, value2);
	}
	else if (op == Name::NotEqual) {
		expr = new Kiwi::NotEqualExpression(value1, value2);
	}

	info.AddInstruction(new Kiwi::AssignInstruction(SymbolTable::Bool->KiwiType(), var->Copy(), expr));

	return var;
}
