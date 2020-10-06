#include "EmptyNode.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Symbols;

EmptyNode::EmptyNode() : Node(ScopeList(), FileInfo()) {

}

EmptyNode::~EmptyNode() {

}

CompiledNode EmptyNode::Compile(CompileInfo& info) {
	return CompiledNode();
}

Mango EmptyNode::ToMango() const {
	if (node) return node->ToMango();
	return Mango();
}

StringBuilder EmptyNode::ToMelon(const UInt indent) const {
	if (node) return node->ToMelon(indent);
	return StringBuilder();
}