#include "IntegerBinaryOperatorNode.h"

#include "Melon/Symbols/IntegerSymbol.h"

#include "Melon/Optimizing/OptimizerInstruction.h"

#include "Boxx/Math.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Optimizing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

IntegerBinaryOperatorNode::IntegerBinaryOperatorNode(const Name& op, const Kiwi::Type& type) {
	this->op   = op;
	this->type = type;
}

Ptr<Kiwi::Value> IntegerBinaryOperatorNode::Compile(Weak<Expression> operand1, Weak<Expression> operand2, CompileInfo& info, bool includeType) const {
	Ptr<Kiwi::Expression> expression;
	Ptr<Kiwi::Value> value1 = operand1->Compile(info);
	Ptr<Kiwi::Value> value2 = operand2->Compile(info);

	if (op.name == Name::Add.name) {
		expression = new Kiwi::AddExpression(value1, value2);
	}
	else if (op.name == Name::Sub.name) {
		expression = new Kiwi::SubExpression(value1, value2);
	}
	else if (op.name == Name::Mul.name) {
		expression = new Kiwi::MulExpression(value1, value2);
	}
	else if (op.name == Name::IDiv.name) {
		expression = new Kiwi::DivExpression(value1, value2);
	}
	else if (op.name == Name::Mod.name) {
		expression = new Kiwi::ModExpression(value1, value2);
	}
	else if (op.name == Name::BitOr.name) {
		expression = new Kiwi::BitOrExpression(value1, value2);
	}
	else if (op.name == Name::BitAnd.name) {
		expression = new Kiwi::BitAndExpression(value1, value2);
	}
	else if (op.name == Name::BitXor.name) {
		expression = new Kiwi::BitXorExpression(value1, value2);
	}
	else if (op.name == Name::ShiftLeft.name) {
		expression = new Kiwi::LeftShiftExpression(value1, value2);
	}
	else if (op.name == Name::ShiftRight.name) {
		expression = new Kiwi::RightShiftExpression(value1, value2);
	}
	else if (op.name == Name::Equal.name) {
		expression = new Kiwi::EqualExpression(value1, value2);
	}
	else if (op.name == Name::NotEqual.name) {
		expression = new Kiwi::NotEqualExpression(value1, value2);
	}
	else if (op.name == Name::Less.name) {
		expression = new Kiwi::LessExpression(value1, value2);
	}
	else if (op.name == Name::Greater.name) {
		expression = new Kiwi::GreaterExpression(value1, value2);
	}
	else if (op.name == Name::LessEqual.name) {
		expression = new Kiwi::LessEqualExpression(value1, value2);
	}
	else if (op.name == Name::GreaterEqual.name) {
		expression = new Kiwi::GreaterEqualExpression(value1, value2);
	}

	Ptr<Kiwi::Variable> var = new Kiwi::Variable(info.NewRegister());
	info.AddInstruction(new Kiwi::AssignInstruction(type, var->Copy(), expression));
	return var;
}
