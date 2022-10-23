#include "EmptySymbolNode.h"

using namespace Melon::Nodes;
using namespace Melon::Symbols::Nodes;

CompiledNode EmptySymbolNode::Compile(OldCompileInfo& info) const {
	return CompiledNode();
}