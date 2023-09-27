#include "EnumStatement.h"

#include "Melon/Symbols/EnumSymbol.h"
#include "Melon/Symbols/ValueSymbol.h"
#include "Melon/Symbols/IntegerSymbol.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

EnumStatement::EnumStatement(Symbol* const scope, const FileInfo& file) : Statement(scope, file) {

}

EnumStatement::~EnumStatement() {

}

Ptr<Kiwi::Value> EnumStatement::Compile(CompileInfo& info) {
	Ptr<Kiwi::Struct> struct_ = new Kiwi::Struct(symbol->KiwiName());

	struct_->AddVariable(SymbolTable::Byte->KiwiType(), Name::Value.name);
	struct_->AddVariable(SymbolTable::Int->KiwiType(), Name::Items.name);

	info.program->AddStruct(struct_);

	return nullptr;
}

StringBuilder EnumStatement::ToMelon(const UInt indent) const {
	StringBuilder sb = "enum ";
	sb += name.ToString();
	sb += "\n";

	String tabs = String('\t').Repeat(indent + 1);

	for (UInt i = 0; i < values.Count(); i++) {
		ValueSymbol* valueSym = symbol->Find<ValueSymbol>(values[i], file);

		sb += tabs;
		sb += values[i].ToString();

		if (valueSym->type) {
			sb += '(';
			sb += valueSym->type->ToSimpleString();
			sb += ')';
		}

		sb += " = ";
		sb += String::ToString(valueSym->value);
		sb += i != values.Count() - 1 ? ",\n" : "\n";
	}

	sb += String('\t').Repeat(indent);
	sb += "end";
	return sb;
}
