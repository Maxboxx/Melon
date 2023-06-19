#include "BooleanConstantNode.h"

#include "Melon/Symbols/IntegerSymbol.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

BooleanConstantNode::BooleanConstantNode(const bool value) {
	this->value = value;
}

Ptr<Kiwi::Value> BooleanConstantNode::Compile(CompileInfo& info) const {
	return new Kiwi::Integer(SymbolTable::Bool->KiwiType(), value ? 1 : 0);
}