#include "IndexSetNode.h"

#include "Melon/Symbols/TemplateTypeSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"
#include "Melon/Symbols/PtrSymbol.h"
#include "Melon/Symbols/ClassSymbol.h"

#include "Melon/Nodes/KiwiValue.h"
#include "Melon/Nodes/KiwiVariable.h"
#include "Melon/Nodes/TypeConversion.h"

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

	Ptr<TypeConversion> convert = new TypeConversion(operands[2]->scope, operands[2]->File());
	convert->isExplicit = false;
	
	if (PtrSymbol* ps = itemsSym->Type()->Cast<PtrSymbol>()) {
		convert->type = ps->PtrType()->AbsoluteName();

		Kiwi::Type type = ps->KiwiType();

		Ptr<Kiwi::Variable> items = new Kiwi::Variable(info.NewRegister());

		info.AddInstruction(new Kiwi::AssignInstruction(type, items->Copy(), new Kiwi::SubVariable(arr, Name::Items.name)));

		type.pointers--;

		Ptr<Kiwi::Value> index = operands[1]->Compile(info);

		convert->expression = new KiwiValue(operands[2]->Compile(info), operands[2]->Type()->AbsoluteName());

		info.AddInstruction(new Kiwi::OffsetAssignInstruction(
			new Kiwi::DerefVariable(items->name),
			convert->Compile(info), type, index
		));
	}
	else if (TemplateTypeSymbol* tts = typeSym->Cast<TemplateTypeSymbol>()) {
		convert->type = tts->TemplateArgument(0)->AbsoluteName();

		Kiwi::Type type = tts->TemplateArgument(0)->KiwiType();

		Ptr<Kiwi::Value> index = operands[1]->Compile(info);

		convert->expression = new KiwiValue(operands[2]->Compile(info), operands[2]->Type()->AbsoluteName());

		info.AddInstruction(new Kiwi::OffsetAssignInstruction(
			new Kiwi::SubVariable(arr, Name::Items.name),
			convert->Compile(info), type, index
		));
	}
	
	return nullptr;
}
