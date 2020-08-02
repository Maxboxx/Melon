#include "BooleanToBooleanNode.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Symbols::Nodes;

CompiledNode BooleanToBooleanNode::Compile(const List<NodePtr>& nodes, CompileInfo& info) const {
	return nodes[0]->Compile(info);
}