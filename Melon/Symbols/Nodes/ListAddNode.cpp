#include "ListAddNode.h"

#include "Melon/Symbols/TemplateTypeSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"
#include "Melon/Symbols/PtrSymbol.h"
#include "Melon/Symbols/ClassSymbol.h"
#include "Melon/Symbols/IntegerSymbol.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Optimizing;
using namespace Melon::Symbols::Nodes;

Ptr<Kiwi::Value> ListAddNode::Compile(List<Weak<Expression>> operands, CompileInfo& info, bool includeType) const {
	Ptr<Kiwi::Variable> list = operands[0]->Compile(info).AsPtr<Kiwi::Variable>();

	const String addLbl = info.NewLabel();

	TemplateTypeSymbol* type = operands[0]->Type()->Cast<TemplateTypeSymbol>();
	TypeSymbol* itemType  = type->TemplateArgument(0);
	VariableSymbol* itemsSym = type->Find<VariableSymbol>(Name::Items, operands[0]->File());
	TypeSymbol* arrayType = itemsSym->Type();
	VariableSymbol* lenSym = type->Find<VariableSymbol>(Name::Length, operands[0]->File());
	VariableSymbol* capSym = type->Find<VariableSymbol>(Name::Capacity, operands[0]->File());

	info.AddInstruction(new Kiwi::IfInstruction(
		new Kiwi::LessExpression(
			new Kiwi::SubVariable(new Kiwi::DerefVariable(list->name), lenSym->KiwiName()),
			new Kiwi::SubVariable(new Kiwi::DerefVariable(list->name), capSym->KiwiName())
		),
		addLbl
	));

	Ptr<Kiwi::Variable> newCap = new Kiwi::Variable(info.NewRegister());

	info.AddInstruction(new Kiwi::AssignInstruction(
		capSym->KiwiType(), newCap->Copy(),
		new Kiwi::LeftShiftExpression(new Kiwi::SubVariable(new Kiwi::DerefVariable(list->name), capSym->KiwiName()), new Kiwi::Integer(capSym->KiwiType(), 1))
	));
	
	info.AddInstruction(new Kiwi::AssignInstruction(
		new Kiwi::SubVariable(new Kiwi::DerefVariable(list->name), capSym->KiwiName()),
		newCap->Copy()
	));

	Ptr<Kiwi::Variable> size = new Kiwi::Variable(info.NewRegister());

	info.AddInstruction(new Kiwi::AssignInstruction(
		capSym->KiwiType(), size->Copy(),
		new Kiwi::MulExpression(
			newCap->Copy(),
			new Kiwi::Integer(capSym->KiwiType(), itemType->Size())
		)
	));

	Ptr<Kiwi::Variable> newList = new Kiwi::Variable(info.NewRegister());
	
	info.AddInstruction(new Kiwi::AssignInstruction(
		arrayType->KiwiType(), newList->Copy(),
		new Kiwi::AllocExpression(size->Copy())
	));

	info.AddInstruction(new Kiwi::AssignInstruction(
		lenSym->KiwiType(), size->Copy(),
		new Kiwi::MulExpression(
			new Kiwi::SubVariable(new Kiwi::DerefVariable(list->name), lenSym->KiwiName()),
			new Kiwi::Integer(lenSym->KiwiType(), itemType->Size())
		)
	));

	info.AddInstruction(new Kiwi::CopyInstruction(
		newList->Copy(), 
		new Kiwi::SubVariable(new Kiwi::DerefVariable(list->name), itemsSym->KiwiName()),
		size->Copy()
	));

	info.AddInstruction(new Kiwi::FreeInstruction(new Kiwi::SubVariable(new Kiwi::DerefVariable(list->name), itemsSym->KiwiName())));

	info.AddInstruction(new Kiwi::AssignInstruction(
		new Kiwi::SubVariable(new Kiwi::DerefVariable(list->name), itemsSym->KiwiName()),
		newList
	));

	info.NewInstructionBlock(addLbl);

	Ptr<Kiwi::Variable> items = new Kiwi::Variable(info.NewRegister());

	info.AddInstruction(new Kiwi::AssignInstruction(
		arrayType->KiwiType(), items->Copy(),
		new Kiwi::SubVariable(new Kiwi::DerefVariable(list->name), itemsSym->KiwiName())
	));

	info.AddInstruction(new Kiwi::OffsetAssignInstruction(
		new Kiwi::DerefVariable(items->name),
		operands[1]->Compile(info),
		itemType->KiwiType(),
		new Kiwi::SubVariable(new Kiwi::DerefVariable(list->name), lenSym->KiwiName())
	));

	info.AddInstruction(new Kiwi::AssignInstruction(
		new Kiwi::SubVariable(new Kiwi::DerefVariable(list->name), lenSym->KiwiName()),
		new Kiwi::AddExpression(
			new Kiwi::SubVariable(new Kiwi::DerefVariable(list->name), lenSym->KiwiName()),
			new Kiwi::Integer(lenSym->KiwiType(), 1)
		)
	));
	
	return nullptr;
}
