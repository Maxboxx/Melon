#include "IntegerNotNode.h"

#include "Melon/Symbols/IntegerSymbol.h"

#include "Kiwi/Old/Kiwi.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Symbols::Nodes;

Ptr<Kiwi::Value> IntegerNotNode::Compile(Weak<Expression> operand, CompileInfo& info, bool includeType) const {
	Ptr<Kiwi::Value> value  = operand->Compile(info);
	Ptr<Kiwi::Variable> var = new Kiwi::Variable(info.NewRegister());
	info.AddInstruction(new Kiwi::AssignInstruction(SymbolTable::Bool->KiwiType(), var->Copy(), new Kiwi::EqualExpression(value, new Kiwi::Integer(SymbolTable::Bool->KiwiType(), 0))));
	return var;
}
