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