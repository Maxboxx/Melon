#include "BooleanBinaryOperatorNode.h"

#include "Melon/Symbols/IntegerSymbol.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Symbols::Nodes;

BooleanBinaryOperatorNode::BooleanBinaryOperatorNode(const Name& op) {
	this->op = op;
}

Ptr<Kiwi::Value> BooleanBinaryOperatorNode::Compile(Weak<Expression> operand1, Weak<Expression> operand2, CompileInfo& info, bool includeType) const {
	Ptr<Kiwi::Expression> expression;
	Ptr<Kiwi::Value> value1 = operand1->Compile(info);
	Ptr<Kiwi::Value> value2 = operand2->Compile(info);

	if (op.name == Name::BitOr.name) {
		expression = new Kiwi::BitOrExpression(value1, value2);
	}
	else if (op.name == Name::BitAnd.name) {
		expression = new Kiwi::BitAndExpression(value1, value2);
	}
	else if (op.name == Name::BitXor.name) {
		expression = new Kiwi::BitXorExpression(value1, value2);
	}
	else if (op.name == Name::Equal.name) {
		expression = new Kiwi::EqualExpression(value1, value2);
	}
	else if (op.name == Name::NotEqual.name) {
		expression = new Kiwi::NotEqualExpression(value1, value2);
	}

	Ptr<Kiwi::Variable> var = new Kiwi::Variable(info.NewRegister());
	info.AddInstruction(new Kiwi::AssignInstruction(SymbolTable::Bool->KiwiType(), var->Copy(), expression));
	return var;
}

