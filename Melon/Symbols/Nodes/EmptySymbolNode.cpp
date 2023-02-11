#include "EmptySymbolNode.h"

using namespace Melon::Nodes;
using namespace Melon::Symbols::Nodes;

Ptr<Kiwi::Value> EmptySymbolNode::Compile(CompileInfo& info) const {
	return nullptr;
}
