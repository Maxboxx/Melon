#include "TypeNode.h"

#include "Melon/Parsing/Parser.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;

TypeNode::TypeNode(const ScopeList& type) : Node(ScopeList(), FileInfo()) {
	this->type = type;
}

TypeNode::~TypeNode() {
}

CompiledNode TypeNode::Compile(CompileInfo& info) {
	return CompiledNode();
}

Symbol* TypeNode::Type() const {
	return SymbolTable::FindAbsolute(type, file);
}

StringBuilder TypeNode::ToMelon(const UInt indent) const {
	return StringBuilder();
}
