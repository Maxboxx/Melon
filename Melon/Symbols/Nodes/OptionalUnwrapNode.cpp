#include "OptionalUnwrapNode.h"

#include "Melon/Symbols/NameList.h"
#include "Melon/Symbols/FunctionSymbol.h"

#include "Kiwi/Old/Kiwi.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

Ptr<Kiwi::Value> OptionalUnwrapNode::Compile(Weak<Expression> operand, CompileInfo& info, bool includeType) const {
	Ptr<Kiwi::Variable> optional = operand->Compile(info).AsPtr<Kiwi::Variable>();
	if (!optional) return nullptr;

	FunctionSymbol* const func = operand->Type()->FindUnaryOperator(Name::Unwrap, operand->File());
	if (!func) return nullptr;

	return new Kiwi::SubVariable(optional, operand->Type()->Find(Name::Value, operand->File())->KiwiName());
}
