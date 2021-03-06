#include "StructAssignNode.h"

#include "Melon/Symbols/ScopeList.h"
#include "Melon/Symbols/TypeSymbol.h"
#include "Melon/Symbols/StructSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"

#include "Melon/Nodes/MemoryNode.h"

#include "Kiwi/Kiwi.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

CompiledNode StructAssignNode::Compile(const Boxx::List<NodePtr>& nodes, CompileInfo& info) const {
	bool important = info.important;
	info.important = false;

	CompiledNode c1 = nodes[0]->Compile(info);
	const UInt frame = info.stack.frame;
	CompiledNode c2 = nodes[1]->Compile(info);
	c1.AddInstructions(c2.instructions);
	c1.argument = OffsetArgument(c1.argument, frame, info);

	StructSymbol* const type = nodes[0]->Type()->Cast<StructSymbol>();

	for (UInt i = 0; i < type->members.Size(); i++) {
		VariableSymbol* const member = type->Find<VariableSymbol>(type->members[i], nodes[0]->file);

		Pointer<MemoryNode> mn1 = new MemoryNode(c1.argument.mem);
		mn1->mem.offset += member->stackIndex;
		mn1->type = member->Type()->AbsoluteName();

		Pointer<MemoryNode> mn2 = new MemoryNode(c2.argument.mem);
		mn2->mem.offset += member->stackIndex;
		mn2->type = member->Type()->AbsoluteName();

		info.important = important;
		c1.AddInstructions(Node::CompileAssignment(mn1, mn2, info, nodes[1]->file).instructions);
		info.important = false;
	}

	info.important = important;
	return c1;
}
