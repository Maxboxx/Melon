#include "Array.h"

#include "Boxx/Math.h"

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
	TypeSymbol* type = Type(info.PeekExpectedType());

	if (type == nullptr) return nullptr;

	const String arr = info.NewRegister();
	info.AddInstruction(new Kiwi::AssignInstruction(type->KiwiType(), arr, new Kiwi::AllocExpression(type->KiwiType().name)));
	info.AddInstruction(new Kiwi::AssignInstruction(new Kiwi::SubVariable(new Kiwi::DerefVariable(arr), Name::Length.name), new Kiwi::Integer(0)));
	return new Kiwi::Variable(arr);
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