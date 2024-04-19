#include "ClassCastNode.h"

#include "Melon/Symbols/NameList.h"
#include "Melon/Symbols/TypeSymbol.h"
#include "Melon/Symbols/ClassSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"

#include "Kiwi/Old/Kiwi.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

Ptr<Kiwi::Value> ClassCastNode::Compile(Weak<Expression> operand, CompileInfo& info, bool includeType) const {
	Ptr<Kiwi::Value> val = operand->Compile(info);
	Ptr<Kiwi::Variable> var = new Kiwi::Variable(info.NewRegister());
	info.AddInstruction(new Kiwi::AssignInstruction(type->KiwiType(), var->Copy(), val));
	return var;
}
