#include "OptionalToBooleanNode.h"

#include "Melon/Symbols/ScopeList.h"
#include "Melon/Symbols/Symbols.h"

#include "Melon/Nodes/StackNode.h"
#include "Melon/Nodes/BooleanNode.h"

#include "Kiwi/Kiwi.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

CompiledNode OptionalToBooleanNode::Compile(const Boxx::List<NodePtr>& nodes, CompileInfo& info) const {
	CompiledNode c = nodes[0]->Compile(info);

	const ScopeList type = nodes[0]->Type();

	Symbol s = Symbol::Find(type, nodes[0]->file);
	UInt offset = s.size;

	Symbol argSym = s.Get(s.args[0], nodes[0]->file);
	Symbol argType = argSym.GetType(nodes[0]->file);
	offset -= argType.size;

	c.argument.mem.offset = c.argument.mem.offset + offset;
	return c;
}
