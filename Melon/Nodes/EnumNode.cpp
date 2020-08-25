#include "EnumNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

EnumNode::EnumNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

EnumNode::~EnumNode() {

}

CompiledNode EnumNode::Compile(CompileInfo& info) {
	return CompiledNode();
}

Mango EnumNode::ToMango() const {
	return Mango();
}