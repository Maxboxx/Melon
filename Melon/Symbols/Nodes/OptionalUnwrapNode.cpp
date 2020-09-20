#include "OptionalUnwrapNode.h"

#include "Melon/Symbols/ScopeList.h"
#include "Melon/Symbols/Symbols.h"

#include "Kiwi/Kiwi.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

CompiledNode OptionalUnwrapNode::Compile(const Boxx::List<NodePtr>& nodes, CompileInfo& info) const {
	CompiledNode c = nodes[0]->Compile(info);

	List<ScopeList> args;
	args.Add(nodes[0]->Type());

	Symbol s = Symbol::FindFunction(args[0].Add(Scope::Unwrap), args, nodes[0]->file);
	if (s.type == SymbolType::None) return c;

	Symbol r = Symbol::Find(s.returnValues[0], nodes[0]->file);
	if (r.type == SymbolType::None) return c;
	c.size = r.size;
	c.argument.mem.offset += 1;

	return c;
}
