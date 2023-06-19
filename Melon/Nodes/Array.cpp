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
	else {
		return expected;
	}
}

Ptr<Kiwi::Value> Melon::Nodes::Array::Compile(CompileInfo& info) {
	TemplateTypeSymbol* type = Type(info.PeekExpectedType())->Cast<TemplateTypeSymbol>();

	if (type == nullptr) return nullptr;

	Ptr<Kiwi::Variable> arr = new Kiwi::Variable(info.NewRegister());

	VariableSymbol* lenSym = type->Find<VariableSymbol>(Name::Length, file);
	TypeSymbol* itemType = type->TemplateArgument(0);
	UInt size = lenSym->Size();
	size += itemType->Size() * items.Count();

	info.AddInstruction(new Kiwi::AssignInstruction(type->KiwiType(), arr->Copy(), new Kiwi::AllocExpression(size)));
	info.AddInstruction(new Kiwi::AssignInstruction(new Kiwi::SubVariable(new Kiwi::DerefVariable(arr->name), Name::Length.name), new Kiwi::Integer(lenSym->KiwiType(), items.Count())));

	Kiwi::Type kiwiType = type->Find<VariableSymbol>(Name::Items, file)->KiwiType();

	info.PushExpectedType(itemType);

	for (UInt i = 0; i < items.Count(); i++) {
		Ptr<Kiwi::Value> value = items[i]->Compile(info);
		info.AddInstruction(new Kiwi::OffsetAssignInstruction(new Kiwi::SubVariable(new Kiwi::DerefVariable(arr->name), Name::Items.name), value, kiwiType, i));
	}

	info.PopExpectedType();

	return arr;
}

StringBuilder Melon::Nodes::Array::ToMelon(const UInt indent) const {
	StringBuilder builder;

	if (type) {
		builder += (*type).ToSimpleString();
		builder += ' ';
	}

	builder += "{}";
	return builder;
}