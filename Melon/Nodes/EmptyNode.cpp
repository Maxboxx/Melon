#include "EmptyNode.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Symbols;

EmptyNode::EmptyNode() : StatementNode(nullptr, FileInfo()) {

}

EmptyNode::~EmptyNode() {
	
}

CompiledNode EmptyNode::Compile(CompileInfo& info) {
	return CompiledNode();
}

StringBuilder EmptyNode::ToMelon(const UInt indent) const {
	if (statement) return statement->ToMelon(indent);
	return StringBuilder();
}