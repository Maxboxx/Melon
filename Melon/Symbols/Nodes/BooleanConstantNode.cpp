#include "BooleanConstantNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

BooleanConstantNode::BooleanConstantNode(const bool value) {
	this->value = value;
}

CompiledNode BooleanConstantNode::Compile(CompileInfo& info) const {
	CompiledNode c;
	c.argument = Argument(value ? 1 : 0);
	c.size = 1;
	return c;
}