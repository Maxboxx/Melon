#include "OptionalAssignValueNode.h"

#include "Melon/Symbols/NameList.h"
#include "Melon/Symbols/TypeSymbol.h"
#include "Melon/Symbols/StructSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"

#include "Melon/Nodes/KiwiMemoryExpression.h"
#include "Melon/Nodes/Boolean.h"

#include "Kiwi/Old/Kiwi.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

CompiledNode OptionalAssignValueNode::Compile(Weak<Expression> operand1, Weak<Expression> operand2, OldCompileInfo& info) const {
	bool important = info.important;
	info.important = false;

	CompiledNode c1 = operand1->Compile(info);

	StructSymbol* const type = operand1->Type()->Cast<StructSymbol>();
	const bool isNil = operand2->Type() == SymbolTable::Nil;

	UInt offset = 0;

	for (UInt i = 0; i < type->members.Count(); i++) {
		if (i > 0 && isNil) break;

		VariableSymbol* const member = type->Find<VariableSymbol>(type->members[i], operand1->File());
		
		Ptr<KiwiMemoryExpression> mn1 = new KiwiMemoryExpression(c1.argument.mem.offset + offset, member->Type()->AbsoluteName());
		offset++;

		info.important = important;

		if (i == 1) {
			c1.AddInstructions(Node::CompileAssignment(mn1, operand2, info, operand2->File()).instructions);
		}
		else {
			Ptr<Boolean> bn = new Boolean(operand1->File());
			bn->value = !isNil;
			c1.AddInstructions(Node::CompileAssignment(mn1, bn, info, operand2->File()).instructions);
		}

		info.important = false;
	}

	info.important = important;

	return c1;
}
