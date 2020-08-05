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

ScopeList TypeNode::Type() const {
	return type;
}

Mango TypeNode::ToMango() const {
	return Mango();
}
