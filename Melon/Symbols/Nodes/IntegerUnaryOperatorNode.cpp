#include "IntegerUnaryOperatorNode.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Symbols::Nodes;

IntegerUnaryOperatorNode::IntegerUnaryOperatorNode(const Name& op) {
	this->op = op;
}

Ptr<Kiwi::Value> IntegerUnaryOperatorNode::Compile(Weak<Expression> operand, CompileInfo& info, bool includeType) const {
	Ptr<Kiwi::Value> value = operand->Compile(info);
	Ptr<Kiwi::Expression> expression;

	if (op.name == Name::Neg.name) {
		expression = new Kiwi::NegExpression(value);
	}
	else if (op.name == Name::BitNot.name) {
		expression = new Kiwi::BitNotExpression(value);
	}

	Ptr<Kiwi::Variable> var = new Kiwi::Variable(info.NewRegister());
	info.AddInstruction(new Kiwi::AssignInstruction(operand->Type()->KiwiType(), var->Copy(), expression));
	return var;
}

