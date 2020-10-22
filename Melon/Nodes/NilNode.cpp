#include "NilNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;

NilNode::NilNode(const FileInfo& file) : Node(ScopeList(), file) {

}

NilNode::~NilNode() {

}

ScopeList NilNode::Type() const {
	return ScopeList::Nil;
}

bool NilNode::IsImmediate() const {
	return true;
}

Long NilNode::GetImmediate() const {
	return 0;
}

CompiledNode NilNode::Compile(CompileInfo& info) {
	CompiledNode node;
	node.argument = Argument(0);
	node.size = 0;
	return node;
}

Mango NilNode::ToMango() const {
	return Mango();
}

StringBuilder NilNode::ToMelon(const UInt indent) const {
	return "nil";
}