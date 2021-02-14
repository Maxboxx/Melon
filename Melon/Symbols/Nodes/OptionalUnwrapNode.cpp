#include "OptionalUnwrapNode.h"

#include "Melon/Symbols/ScopeList.h"
#include "Melon/Symbols/FunctionSymbol.h"

#include "Kiwi/Kiwi.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

CompiledNode OptionalUnwrapNode::Compile(const Boxx::List<NodePtr>& nodes, CompileInfo& info) const {
	CompiledNode c = nodes[0]->Compile(info);

	FunctionSymbol* const func = nodes[0]->Type()->FindUnaryOperator(Scope::Unwrap, nodes[0]->file);
	if (!func) return c;

	c.size = func->ReturnType(0)->Size();
	c.argument.mem.offset += 1;

	return c;
}
