#include "LengthNode.h"

#include "Melon/Symbols/TemplateTypeSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"
#include "Melon/Symbols/ClassSymbol.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Optimizing;
using namespace Melon::Symbols::Nodes;

Ptr<Kiwi::Value> LengthNode::Compile(Weak<Expression> operand, CompileInfo& info, bool includeType) const {
	Ptr<Kiwi::Variable> var = operand->Compile(info).AsPtr<Kiwi::Variable>();

	TypeSymbol* const type = operand->Type();

	Kiwi::Type lenType = type->Find<VariableSymbol>(Name::Length, operand->File())->KiwiType();

	if (type->Is<ClassSymbol>()) {
		var = new Kiwi::DerefVariable(var->name);
	}

	Ptr<Kiwi::Variable> len = new Kiwi::Variable(info.NewRegister());
	info.AddInstruction(new Kiwi::AssignInstruction(lenType, len->Copy(), new Kiwi::SubVariable(var, Name::Length.name)));
	return len;
}
