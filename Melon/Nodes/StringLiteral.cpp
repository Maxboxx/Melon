#include "StringLiteral.h"

#include "Boxx/Math.h"

#include "Melon/Symbols/IntegerSymbol.h"
#include "Melon/Symbols/StructSymbol.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Symbols;

StringLiteral::StringLiteral(const FileInfo& file) : Expression(nullptr, file) {

}

StringLiteral::~StringLiteral() {

}

TypeSymbol* StringLiteral::Type(TypeSymbol* expected) const {
	return (TypeSymbol*)SymbolTable::String;
}

Ptr<Kiwi::Value> StringLiteral::Compile(CompileInfo& info) {
	Ptr<Kiwi::Variable> var = new Kiwi::Variable(info.NewRegister());

	info.AddInstruction(new Kiwi::AssignInstruction(SymbolTable::String->KiwiType(), var->Copy(), nullptr));
	info.AddInstruction(new Kiwi::AssignInstruction(new Kiwi::SubVariable(var->Copy(), Name::Length.name), new Kiwi::Integer(SymbolTable::UInt->KiwiType(), value.Length())));
	info.AddInstruction(new Kiwi::AssignInstruction(new Kiwi::SubVariable(var->Copy(), Name::Items.name), new Kiwi::StringValue(value)));
	
	return var;
}

StringBuilder StringLiteral::ToMelon(const UInt indent) const {
	StringBuilder builder;
	builder += "\"";
	builder += value;
	builder += "\"";
	return builder;
}