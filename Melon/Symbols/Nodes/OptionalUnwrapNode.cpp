#include "OptionalUnwrapNode.h"

#include "Melon/Symbols/NameList.h"
#include "Melon/Symbols/FunctionSymbol.h"

#include "Kiwi/Old/Kiwi.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

CompiledNode OptionalUnwrapNode::Compile(Weak<Expression> operand, OldCompileInfo& info) const {
	CompiledNode c = operand->Compile(info);

	FunctionSymbol* const func = operand->Type()->FindUnaryOperator(Name::Unwrap, operand->File());
	if (!func) return c;

	c.size = func->ReturnType(0)->Size();
	c.argument.mem.offset += 1;

	return c;
}
