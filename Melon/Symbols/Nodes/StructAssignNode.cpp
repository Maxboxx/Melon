#include "StructAssignNode.h"

#include "Melon/Symbols/NameList.h"
#include "Melon/Symbols/TypeSymbol.h"
#include "Melon/Symbols/StructSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"

#include "Melon/Nodes/KiwiMemoryExpression.h"

#include "Kiwi/Kiwi.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

CompiledNode StructAssignNode::Compile(Weak<Expression> operand1, Weak<Expression> operand2, CompileInfo& info) const {
	bool important = info.important;
	info.important = false;

	CompiledNode c1 = operand1->Compile(info);
	const UInt frame = info.stack.frame;
	CompiledNode c2 = operand2->Compile(info);
	c1.AddInstructions(c2.instructions);
	c1.argument = OffsetArgument(c1.argument, frame, info);

	StructSymbol* const type = operand1->Type()->Cast<StructSymbol>();

	for (UInt i = 0; i < type->members.Size(); i++) {
		VariableSymbol* const member = type->Find<VariableSymbol>(type->members[i], operand1->File());

		Ptr<KiwiMemoryExpression> mn1 = new KiwiMemoryExpression(c1.argument.mem.offset + member->stackIndex, member->Type()->AbsoluteName());
		Ptr<KiwiMemoryExpression> mn2 = new KiwiMemoryExpression(c2.argument.mem.offset + member->stackIndex, member->Type()->AbsoluteName());

		info.important = important;
		c1.AddInstructions(Node::CompileAssignment(mn1, mn2, info, operand2->File()).instructions);
		info.important = false;
	}

	info.important = important;
	return c1;
}
