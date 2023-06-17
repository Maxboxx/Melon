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

TypeSymbol* Melon::Nodes::Array::Type() const {
	return SymbolTable::Find<TypeSymbol>(type, scope->AbsoluteName(), file, SymbolTable::SearchOptions::ReplaceTemplates);
}

Ptr<Kiwi::Value> Melon::Nodes::Array::Compile(CompileInfo& info) {
	const String arr = info.NewRegister();
	info.AddInstruction(new Kiwi::AssignInstruction(Type()->KiwiType(), arr, new Kiwi::AllocExpression(Type()->KiwiType().name)));
	info.AddInstruction(new Kiwi::AssignInstruction(new Kiwi::SubVariable(new Kiwi::DerefVariable(arr), Name::Length.name), new Kiwi::Integer(0)));
	return new Kiwi::Variable(arr);
}

StringBuilder Melon::Nodes::Array::ToMelon(const UInt indent) const {
	StringBuilder builder;
	builder += type.ToSimpleString();
	builder += " {}";
	return builder;
}