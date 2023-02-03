#include "OptionalToBooleanNode.h"

#include "Melon/Symbols/NameList.h"

#include "Melon/Nodes/Boolean.h"

#include "Kiwi/Old/Kiwi.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

Ptr<Kiwi::Value> OptionalToBooleanNode::Compile(Weak<Expression> operand, CompileInfo& info, bool includeType) const {
	Ptr<Kiwi::Variable> var = operand->Compile(info).AsPtr<Kiwi::Variable>();
	if (!var) return nullptr;

	return new Kiwi::SubVariable(var, operand->Type()->Find(Name::HasValue, operand->File())->KiwiName());
}
