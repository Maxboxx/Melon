#include "BooleanNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

BooleanNode::BooleanNode(const FileInfo& file) : ExpressionNode(nullptr, file) {

}

BooleanNode::~BooleanNode() {

}

TypeSymbol* BooleanNode::Type() const {
	return (TypeSymbol*)SymbolTable::Bool;
}

bool BooleanNode::IsImmediate() const {
	return true;
}

Long BooleanNode::GetImmediate() const {
	return boolean ? 1 : 0;
}

CompiledNode BooleanNode::Compile(CompileInfo& info) {
	CompiledNode node;
	node.argument = Argument(GetImmediate());
	node.size = 1;
	return node;
}

StringBuilder BooleanNode::ToMelon(const UInt indent) const {
	return String(boolean ? "true" : "false");
}