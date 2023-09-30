#include "IndexSetNode.h"

#include "Melon/Symbols/TemplateTypeSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"
#include "Melon/Symbols/PtrSymbol.h"
#include "Melon/Symbols/ClassSymbol.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Optimizing;
using namespace Melon::Symbols::Nodes;

Ptr<Kiwi::Value> IndexSetNode::Compile(List<Weak<Expression>> operands, CompileInfo& info, bool includeType) const {
	Ptr<Kiwi::Variable> arr = operands[0]->Compile(info).AsPtr<Kiwi::Variable>();

	TypeSymbol* const typeSym = operands[0]->Type();
	VariableSymbol* const itemsSym = typeSym->Find<VariableSymbol>(Name::Items, operands[0]->File());

	if (typeSym->Is<ClassSymbol>()) {
		arr = new Kiwi::DerefVariable(arr->name);
	}
	
	if (PtrSymbol* ps = itemsSym->Type()->Cast<PtrSymbol>()) {
		Kiwi::Type type = ps->KiwiType();

		Ptr<Kiwi::Variable> items = new Kiwi::Variable(info.NewRegister());

		info.AddInstruction(new Kiwi::AssignInstruction(type, items->Copy(), new Kiwi::SubVariable(arr, Name::Items.name)));

		type.pointers--;

		Ptr<Kiwi::Value> index = operands[1]->Compile(info);
		Ptr<Kiwi::Value> value = operands[2]->Compile(info);

		info.AddInstruction(new Kiwi::OffsetAssignInstruction(
			new Kiwi::DerefVariable(items->name),
			value, type, index
		));
	}
	else if (TemplateTypeSymbol* tts = typeSym->Cast<TemplateTypeSymbol>()) {
		Kiwi::Type type = tts->TemplateArgument(0)->KiwiType();

		Ptr<Kiwi::Value> index = operands[1]->Compile(info);
		Ptr<Kiwi::Value> value = operands[2]->Compile(info);

		info.AddInstruction(new Kiwi::OffsetAssignInstruction(
			new Kiwi::SubVariable(arr, Name::Items.name),
			value, type, index
		));
	}
	
	return nullptr;
}
