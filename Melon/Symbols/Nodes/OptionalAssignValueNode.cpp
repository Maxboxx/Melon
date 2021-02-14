#include "OptionalAssignValueNode.h"

#include "Melon/Symbols/ScopeList.h"
#include "Melon/Symbols/TypeSymbol.h"
#include "Melon/Symbols/StructSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"

#include "Melon/Nodes/MemoryNode.h"
#include "Melon/Nodes/BooleanNode.h"

#include "Kiwi/Kiwi.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

CompiledNode OptionalAssignValueNode::Compile(const Boxx::List<NodePtr>& nodes, CompileInfo& info) const {
	bool important = info.important;
	info.important = false;

	CompiledNode c1 = nodes[0]->Compile(info);

	StructSymbol* const type = nodes[0]->Type()->Cast<StructSymbol>();
	const bool isNil = nodes[1]->Type() == SymbolTable::Nil;

	UInt offset = 0;

	for (UInt i = 0; i < type->members.Size(); i++) {
		if (i > 0 && isNil) break;

		VariableSymbol* const member = type->Find<VariableSymbol>(type->members[i], nodes[0]->file);
		
		Pointer<MemoryNode> mn1 = new MemoryNode(c1.argument.mem);
		mn1->mem.offset += offset;
		mn1->type = member->Type()->AbsoluteName();
		offset++;

		NodePtr value;

		if (i == 1) {
			value = nodes[1];
		}
		else {
			Pointer<BooleanNode> bn = new BooleanNode(nodes[0]->file);
			bn->boolean = !isNil;
			value = bn;
		}

		info.important = important;
		c1.AddInstructions(Node::CompileAssignment(mn1, value, info, nodes[1]->file).instructions);
		info.important = false;
	}

	info.important = important;

	return c1;
}
