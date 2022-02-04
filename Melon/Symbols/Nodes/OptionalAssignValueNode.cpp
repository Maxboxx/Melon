#include "OptionalAssignValueNode.h"

#include "Melon/Symbols/NameList.h"
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

CompiledNode OptionalAssignValueNode::Compile(const _Expression_& operand1, const _Expression_& operand2, CompileInfo& info) const {
	bool important = info.important;
	info.important = false;

	CompiledNode c1 = operand1->Compile(info);

	StructSymbol* const type = operand1->Type()->Cast<StructSymbol>();
	const bool isNil = operand2->Type() == SymbolTable::Nil;

	UInt offset = 0;

	for (UInt i = 0; i < type->members.Size(); i++) {
		if (i > 0 && isNil) break;

		VariableSymbol* const member = type->Find<VariableSymbol>(type->members[i], operand1->File());
		
		Pointer<MemoryNode> mn1 = new MemoryNode(c1.argument.mem);
		mn1->mem.offset += offset;
		mn1->type = member->Type()->AbsoluteName();
		offset++;

		_Expression_ value;

		if (i == 1) {
			value = operand2;
		}
		else {
			Pointer<BooleanNode> bn = new BooleanNode(operand1->File());
			bn->boolean = !isNil;
			value = bn;
		}

		info.important = important;
		c1.AddInstructions(Node::CompileAssignment(mn1, value, info, operand2->File()).instructions);
		info.important = false;
	}

	info.important = important;

	return c1;
}
