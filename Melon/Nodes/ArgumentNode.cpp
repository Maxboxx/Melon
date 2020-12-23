#include "ArgumentNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

ArgumentNode::ArgumentNode(const Argument& arg) : Node(ScopeList(), FileInfo()) {
	argument = arg;
}

ArgumentNode::~ArgumentNode() {

}

ScopeList ArgumentNode::Type() const {
	return type;
}

CompiledNode ArgumentNode::Compile(CompileInfo& info) {
	CompiledNode c;
	c.argument = argument;
	/* TODO: node
	c.size = Symbols::Find(type, file).size;
	*/
	return c;
}

StringBuilder ArgumentNode::ToMelon(const UInt indent) const {
	return StringBuilder();
}