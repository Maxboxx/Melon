#include "PtrUnwrapNode.h"

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

Ptr<Kiwi::Value> PtrUnwrapNode::Compile(Weak<Expression> operand, CompileInfo& info, bool includeType) const {
	Ptr<Kiwi::Variable> val = operand->Compile(info).AsPtr<Kiwi::Variable>();
	Ptr<Kiwi::Variable> ptr = new Kiwi::Variable(info.NewRegister());

	info.AddInstruction(new Kiwi::AssignInstruction(operand->Type()->KiwiType(), ptr->Copy(), val));

	Ptr<Kiwi::Variable> value = new Kiwi::Variable(info.NewRegister());

	TypeSymbol* sym = operand->Type()->Cast<TemplateTypeSymbol>()->TemplateArgument(0);

	info.AddInstruction(new Kiwi::AssignInstruction(sym->KiwiType(), value->Copy(), new Kiwi::DerefVariable(ptr->name)));

	return value;
}
