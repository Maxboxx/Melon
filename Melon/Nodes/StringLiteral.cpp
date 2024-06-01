#include "StringLiteral.h"

#include "Boxx/Math.h"

#include "Melon/Symbols/IntegerSymbol.h"
#include "Melon/Symbols/StructSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"

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

	VariableSymbol* lenSym = SymbolTable::String->Find<VariableSymbol>(Name::Length, File());
	VariableSymbol* strSym = SymbolTable::String->Find<VariableSymbol>(Name::Items, File());

	String str = value;
	str = str.Replace("\\\\", "\\");
	str = str.Replace("\\n", "\n");
	str = str.Replace("\\t", "\t");

	info.AddInstruction(new Kiwi::AssignInstruction(SymbolTable::String->KiwiType(), var->Copy(), nullptr));
	info.AddInstruction(new Kiwi::AssignInstruction(new Kiwi::SubVariable(var->Copy(), lenSym->KiwiName()), new Kiwi::Integer(SymbolTable::UInt->KiwiType(), str.Length())));
	info.AddInstruction(new Kiwi::AssignInstruction(new Kiwi::SubVariable(var->Copy(), strSym->KiwiName()), new Kiwi::StringValue(str)));
	
	return var;
}

StringBuilder StringLiteral::ToMelon(const UInt indent) const {
	StringBuilder builder;
	builder += "\"";
	builder += value;
	builder += "\"";
	return builder;
}