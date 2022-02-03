#include "EmptySymbolNode.h"

using namespace Melon::Nodes;
using namespace Melon::Symbols::Nodes;

CompiledNode EmptySymbolNode::Compile(CompileInfo& info) const {
	return CompiledNode();
}