#include "DiscardNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

DiscardNode::DiscardNode(Symbols::Symbol* const scope, const FileInfo& file) : Expression(scope, file) {

}

DiscardNode::~DiscardNode() {

}

TypeSymbol* DiscardNode::Type() const {
	return nullptr;
}

CompiledNode DiscardNode::Compile(CompileInfo& info) {
	return CompiledNode();
}

StringBuilder DiscardNode::ToMelon(const Boxx::UInt indent) const {
	return "_";
}
