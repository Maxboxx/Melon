#include "EmptySymbolNode.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Symbols::Nodes;

CompiledNode EmptySymbolNode::Compile(const List<NodePtr>& nodes, CompileInfo& info) const {
	return CompiledNode();
}