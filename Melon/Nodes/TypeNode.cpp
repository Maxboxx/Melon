#include "TypeNode.h"

#include "Melon/Parsing/Parser.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;

TypeNode::TypeNode(const NameList& type) : Expression(nullptr, FileInfo()) {
	this->type = type;
}

TypeNode::~TypeNode() {

}

CompiledNode TypeNode::Compile(CompileInfo& info) {
	return CompiledNode();
}

TypeSymbol* TypeNode::Type() const {
	return SymbolTable::FindAbsolute<TypeSymbol>(type, file);
}

StringBuilder TypeNode::ToMelon(const UInt indent) const {
	return StringBuilder();
}
