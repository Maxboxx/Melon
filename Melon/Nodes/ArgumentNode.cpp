#include "ArgumentNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

ArgumentNode::ArgumentNode(const Argument& arg) : Node(nullptr, FileInfo()) {
	argument = arg;
}

ArgumentNode::~ArgumentNode() {

}

TypeSymbol* ArgumentNode::Type() const {
	return SymbolTable::FindAbsolute<TypeSymbol>(type, file);
}

CompiledNode ArgumentNode::Compile(CompileInfo& info) {
	CompiledNode c;
	c.argument = argument;
	c.size = Type()->Size();
	return c;
}

StringBuilder ArgumentNode::ToMelon(const UInt indent) const {
	return StringBuilder();
}