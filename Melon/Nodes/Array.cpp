#include "Array.h"

#include "Boxx/Math.h"

#include "Melon/Symbols/TemplateTypeSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

Melon::Nodes::Array::Array(Symbols::Symbol* scope, const FileInfo& file) : Expression(scope, file) {

}

Melon::Nodes::Array::~Array() {

}

TypeSymbol* Melon::Nodes::Array::Type(TypeSymbol* expected) const {
	if (type) {
		return SymbolTable::Find<TypeSymbol>(*type, scope->AbsoluteName(), file, SymbolTable::SearchOptions::ReplaceTemplates);
	}
	else if (!items.IsEmpty()) {
		TypeSymbol* itemType;

		if (expected && expected->Is<TemplateTypeSymbol>()) {
			itemType = expected->Cast<TemplateTypeSymbol>()->TemplateArgument(0);
		}
		else {
			itemType = items[0]->Type();
		}

		for (const Ptr<Expression>& item : items) {
			if (item->Type(itemType) != itemType) {
				return nullptr;
			}
		}

		// TODO: return array of itemType.
		return expected;
	}
	else {
		return expected;
	}
}

Ptr<Kiwi::Value> Melon::Nodes::Array::Compile(CompileInfo& info) {
	TemplateTypeSymbol* type = Type(info.PeekExpectedType())->Cast<TemplateTypeSymbol>();

	if (type == nullptr) return nullptr;

	if (type->Parent()->Name().name == Name::Array.name) {
		return CompileArray(type, info);
	}
	else if (type->Parent()->Name().name == Name::List.name) {
		return CompileList(type, info);
	}
	
	return nullptr;
}

Ptr<Kiwi::Value> Melon::Nodes::Array::CompileArray(TemplateTypeSymbol* type, CompileInfo& info) {
	Ptr<Kiwi::Variable> arr = new Kiwi::Variable(info.NewRegister());

	VariableSymbol* lenSym   = type->Find<VariableSymbol>(Name::Length, file);
	VariableSymbol* itemsSym = type->Find<VariableSymbol>(Name::Items, file);
	TypeSymbol* itemType = type->TemplateArgument(0);
	UInt size = lenSym->Size();
	size += itemType->Size() * items.Count();

	info.AddInstruction(new Kiwi::AssignInstruction(type->KiwiType(), arr->Copy(), new Kiwi::AllocExpression(size)));
	info.AddInstruction(new Kiwi::AssignInstruction(new Kiwi::SubVariable(new Kiwi::DerefVariable(arr->name), lenSym->KiwiName()), new Kiwi::Integer(lenSym->KiwiType(), items.Count())));

	Kiwi::Type kiwiType = type->Find<VariableSymbol>(Name::Items, file)->KiwiType();

	info.PushExpectedType(itemType);

	for (UInt i = 0; i < items.Count(); i++) {
		Ptr<Kiwi::Value> value = items[i]->Compile(info);
		info.AddInstruction(new Kiwi::OffsetAssignInstruction(new Kiwi::SubVariable(new Kiwi::DerefVariable(arr->name), itemsSym->KiwiName()), value, kiwiType, i));
	}

	info.PopExpectedType();

	return arr;
}

Ptr<Kiwi::Value> Melon::Nodes::Array::CompileList(TemplateTypeSymbol* type, CompileInfo& info) {
	VariableSymbol* lenSym   = type->Find<VariableSymbol>(Name::Length, File());
	VariableSymbol* capSym   = type->Find<VariableSymbol>(Name::Capacity, File());
	VariableSymbol* itemsSym = type->Find<VariableSymbol>(Name::Items, File());
	TypeSymbol*     itemType = type->TemplateArgument(0);

	Ptr<Kiwi::Variable> list = new Kiwi::Variable(info.NewRegister());

	info.AddInstruction(new Kiwi::AssignInstruction(
		type->KiwiType(), list->Copy(),
		new Kiwi::AllocExpression(type->MemorySize())
	));

	UInt size = Math::Max(4U, items.Count());

	info.AddInstruction(new Kiwi::AssignInstruction(
		new Kiwi::SubVariable(new Kiwi::DerefVariable(list->name), lenSym->KiwiName()),
		new Kiwi::Integer(lenSym->KiwiType(), items.Count())
	));

	info.AddInstruction(new Kiwi::AssignInstruction(
		new Kiwi::SubVariable(new Kiwi::DerefVariable(list->name), capSym->KiwiName()),
		new Kiwi::Integer(capSym->KiwiType(), size)
	));

	Ptr<Kiwi::Variable> memSize = new Kiwi::Variable(info.NewRegister());

	info.AddInstruction(new Kiwi::AssignInstruction(
		lenSym->KiwiType(), memSize->Copy(),
		new Kiwi::Integer(lenSym->KiwiType(), size * itemType->Size())
	));

	Ptr<Kiwi::Variable> itemsArr = new Kiwi::Variable(info.NewRegister());

	info.AddInstruction(new Kiwi::AssignInstruction(
		itemsSym->KiwiType(),
		itemsArr->Copy(),
		new Kiwi::AllocExpression(memSize)
	));

	Kiwi::Type kiwiType = itemType->KiwiType();

	info.PushExpectedType(itemType);

	for (UInt i = 0; i < items.Count(); i++) {
		Ptr<Kiwi::Value> value = items[i]->Compile(info);
		info.AddInstruction(new Kiwi::OffsetAssignInstruction(new Kiwi::DerefVariable(itemsArr->name), value, kiwiType, i));
	}

	info.PopExpectedType();

	info.AddInstruction(new Kiwi::AssignInstruction(
		new Kiwi::SubVariable(new Kiwi::DerefVariable(list->name), itemsSym->KiwiName()),
		itemsArr
	));

	return list;
}

StringBuilder Melon::Nodes::Array::ToMelon(const UInt indent) const {
	StringBuilder builder;

	if (type) {
		builder += (*type).ToSimpleString();
		builder += ' ';
	}

	if (items.IsEmpty()) {
		builder += "{}";
	}
	else {
		builder += '{';

		for (UInt i = 0; i < items.Count(); i++) {
			if (i > 0) builder += ", ";
			builder += items[i]->ToMelon(indent);
		}

		builder += '}';
	}

	return builder;
}