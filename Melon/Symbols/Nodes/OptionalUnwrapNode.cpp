#include "OptionalUnwrapNode.h"

#include "Melon/Symbols/NameList.h"
#include "Melon/Symbols/FunctionSymbol.h"

#include "Kiwi/Kiwi.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

CompiledNode OptionalUnwrapNode::Compile(const _Expression_& operand, CompileInfo& info) const {
	CompiledNode c = operand->Compile(info);

	FunctionSymbol* const func = operand->Type()->FindUnaryOperator(Name::Unwrap, operand->File());
	if (!func) return c;

	c.size = func->ReturnType(0)->Size();
	c.argument.mem.offset += 1;

	return c;
}
