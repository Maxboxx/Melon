#include "LengthNode.h"

#include "Melon/Symbols/TemplateTypeSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Optimizing;
using namespace Melon::Symbols::Nodes;

Ptr<Kiwi::Value> LengthNode::Compile(Weak<Expression> operand, CompileInfo& info, bool includeType) const {
	Ptr<Kiwi::Variable> var = operand->Compile(info).AsPtr<Kiwi::Variable>();

	Kiwi::Type lenType = operand->Type()->Find<VariableSymbol>(Name::Length, operand->File())->KiwiType();

	Ptr<Kiwi::Variable> len = new Kiwi::Variable(info.NewRegister());
	info.AddInstruction(new Kiwi::AssignInstruction(lenType, len->Copy(), new Kiwi::SubVariable(new Kiwi::DerefVariable(var->name), Name::Length.name)));
	return len;
}
