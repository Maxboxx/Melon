#include "BooleanNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;

BooleanNode::BooleanNode(const FileInfo& file) : Node(ScopeList(), file) {

}

BooleanNode::~BooleanNode() {

}

ScopeList BooleanNode::Type() const {
	return ScopeList().Add(Scope::Bool);
}

bool BooleanNode::IsImmediate() const {
	return true;
}

Long BooleanNode::GetImmediate() const {
	return boolean ? 1 : 0;
}

CompiledNode BooleanNode::Compile(CompileInfo& info) {
	CompiledNode node;
	node.argument = Argument(boolean ? 1 : 0);
	node.size = 1;
	return node;
}

Mango BooleanNode::ToMango() const {
	return Mango("bool", boolean);
}

StringBuilder BooleanNode::ToMelon(const UInt indent) const {
	return String(boolean ? "true" : "false");
}