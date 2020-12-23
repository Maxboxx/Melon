#include "DiscardNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

DiscardNode::DiscardNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

DiscardNode::~DiscardNode() {

}

ScopeList DiscardNode::Type() const {
	return ScopeList::undefined;
}

CompiledNode DiscardNode::Compile(CompileInfo& info) {
	return CompiledNode();
}

StringBuilder DiscardNode::ToMelon(const Boxx::UInt indent) const {
	return "_";
}
