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

	info.AddInstruction(new Kiwi::IfInstruction(
		new Kiwi::LessExpression(
			new Kiwi::SubVariable(list->Copy(), Name::Length.name),
			new Kiwi::SubVariable(list->Copy(), Name::Capacity.name)
		),
		addLbl
	));

	TemplateTypeSymbol* type = operands[0]->Type()->Cast<TemplateTypeSymbol>();
	TypeSymbol* itemType  = type->TemplateArgument(0);
	TypeSymbol* arrayType = type->Find<VariableSymbol>(Name::Items, operands[0]->File())->Type();
	VariableSymbol* lenSym = type->Find<VariableSymbol>(Name::Length, operands[0]->File());
	VariableSymbol* capSym = type->Find<VariableSymbol>(Name::Capacity, operands[0]->File());

	Ptr<Kiwi::Variable> newCap = new Kiwi::Variable(info.NewRegister());

	info.AddInstruction(new Kiwi::AssignInstruction(
		capSym->KiwiType(), newCap->Copy(),
		new Kiwi::LeftShiftExpression(new Kiwi::SubVariable(list->Copy(), Name::Capacity.name), new Kiwi::Integer(SymbolTable::UInt->KiwiType(), 1))
	));
	
	Ptr<Kiwi::Variable> size = new Kiwi::Variable(info.NewRegister());

	info.AddInstruction(new Kiwi::AssignInstruction(
		SymbolTable::UInt->KiwiType(), size->Copy(),
		new Kiwi::MulExpression(
			newCap->Copy(),
			new Kiwi::Integer(SymbolTable::UInt->KiwiType(), arrayType->Size())
		)
	));

	Ptr<Kiwi::Variable> newList = new Kiwi::Variable(info.NewRegister());
	
	info.AddInstruction(new Kiwi::AssignInstruction(
		arrayType->KiwiType(), newList->Copy(),
		new Kiwi::AllocExpression(size)
	));

	info.AddInstruction(new Kiwi::AssignInstruction(
		SymbolTable::UInt->KiwiType(), size->Copy(),
		new Kiwi::MulExpression(
			new Kiwi::SubVariable(list->Copy(), Name::Length.name),
			new Kiwi::Integer(SymbolTable::UInt->KiwiType(), arrayType->Size())
		)
	));

	info.AddInstruction(new Kiwi::CopyInstruction(
		newList->Copy(), 
		new Kiwi::SubVariable(list->Copy(), Name::Items.name),
		size->Copy()
	));

	info.AddInstruction(new Kiwi::FreeInstruction(new Kiwi::SubVariable(list->Copy(), Name::Items.name)));

	info.AddInstruction(new Kiwi::AssignInstruction(
		new Kiwi::SubVariable(list->Copy(), Name::Items.name),
		newList
	));

	info.NewInstructionBlock(addLbl);

	info.AddInstruction(new Kiwi::OffsetAssignInstruction(
		new Kiwi::SubVariable(list->Copy(), Name::Items.name),
		operands[1]->Compile(info),
		itemType->KiwiType(),
		new Kiwi::SubVariable(list->Copy(), Name::Length.name)
	));
	
	return nullptr;
}
