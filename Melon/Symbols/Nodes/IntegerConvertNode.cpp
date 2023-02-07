#include "IntegerConvertNode.h"

#include "Kiwi/Old/Kiwi.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Symbols::Nodes;

IntegerConvertNode::IntegerConvertNode(const Kiwi::Type& type) {
	this->type = type;
}

Ptr<Kiwi::Value> IntegerConvertNode::Compile(Weak<Expression> operand, CompileInfo& info, bool includeType) const {
	Ptr<Kiwi::Value> value = operand->Compile(info);
	Ptr<Kiwi::Variable> var = new Kiwi::Variable(info.NewRegister());
	info.AddInstruction(new Kiwi::AssignInstruction(type, var->Copy(), value));
	return var;
}

