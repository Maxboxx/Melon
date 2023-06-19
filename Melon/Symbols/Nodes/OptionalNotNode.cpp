#include "OptionalNotNode.h"

#include "Melon/Symbols/IntegerSymbol.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

Ptr<Kiwi::Value> OptionalNotNode::Compile(Weak<Expression> operand, CompileInfo& info, bool includeType) const {
	Ptr<Kiwi::Variable> var = operand->Compile(info).AsPtr<Kiwi::Variable>();
	if (!var) return nullptr;

	Ptr<Kiwi::Variable> result = new Kiwi::Variable(info.NewRegister());

	info.AddInstruction(new Kiwi::AssignInstruction(SymbolTable::Bool->KiwiType(), result->Copy(), new Kiwi::EqualExpression(
		new Kiwi::SubVariable(var, operand->Type()->Find(Name::HasValue, operand->File())->KiwiName()), new Kiwi::Integer(SymbolTable::Bool->KiwiType(), 0)
	)));

	return result;
}
