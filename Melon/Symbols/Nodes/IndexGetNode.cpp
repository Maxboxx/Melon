#include "IndexGetNode.h"

#include "Melon/Symbols/TemplateTypeSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"
#include "Melon/Symbols/PtrSymbol.h"
#include "Melon/Symbols/ClassSymbol.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Optimizing;
using namespace Melon::Symbols::Nodes;

Ptr<Kiwi::Value> IndexGetNode::Compile(List<Weak<Expression>> operands, CompileInfo& info, bool includeType) const {
	Ptr<Kiwi::Variable> arr = operands[0]->Compile(info).AsPtr<Kiwi::Variable>();

	TypeSymbol* const typeSym = operands[0]->Type();
	VariableSymbol* const itemsSym = typeSym->Find<VariableSymbol>(Name::Items, operands[0]->File());

	Ptr<Kiwi::Variable> var;

	if (typeSym->Is<ClassSymbol>()) {
		arr = new Kiwi::DerefVariable(arr->name);
	}
	
	if (PtrSymbol* ps = itemsSym->Type()->Cast<PtrSymbol>()) {
		Kiwi::Type type = ps->KiwiType();

		Ptr<Kiwi::Variable> items = new Kiwi::Variable(info.NewRegister());

		info.AddInstruction(new Kiwi::AssignInstruction(type, items->Copy(), new Kiwi::SubVariable(arr, itemsSym->KiwiName())));

		var = new Kiwi::Variable(info.NewRegister());

		type.pointers--;

		info.AddInstruction(new Kiwi::AssignInstruction(
			type, var->Copy(), 
			new Kiwi::OffsetExpression(
				new Kiwi::DerefVariable(items->name),
				type, type, operands[1]->Compile(info)
			)
		));
	}
	else if (TemplateTypeSymbol* tts = typeSym->Cast<TemplateTypeSymbol>()) {
		Kiwi::Type type = tts->TemplateArgument(0)->KiwiType();

		var = new Kiwi::Variable(info.NewRegister());

		info.AddInstruction(new Kiwi::AssignInstruction(
			type, var->Copy(), 
			new Kiwi::OffsetExpression(
				new Kiwi::SubVariable(arr, itemsSym->KiwiName()),
				type, type, operands[1]->Compile(info)
			)
		));
	}
	
	return var;
}
