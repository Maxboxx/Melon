#include "IntegerNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;

IntegerNode::IntegerNode(const FileInfo& file) : Node(ScopeList(), file) {

}

IntegerNode::~IntegerNode() {

}

ScopeList IntegerNode::Type() const {
	return ScopeList().Add(Scope::Long);
}

bool IntegerNode::IsImmediate() const {
	return true;
}

CompiledNode IntegerNode::Compile(CompileInfo& info) {
	CompiledNode node;
	node.argument = Argument(number);
	node.size = info.stack.ptrSize;
	return node;
}

Mango IntegerNode::ToMango() const {
	return Mango("long", number);
}

StringBuilder IntegerNode::ToMelon(const UInt indent) const {
	return String::ToString(number);
}