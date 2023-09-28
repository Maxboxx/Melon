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

ScanResult EnumStatement::Scan(ScanInfoStack& info) {
	symbol->UpdateSize();
	return ScanResult();
}

Ptr<Kiwi::Value> EnumStatement::Compile(CompileInfo& info) {
	Ptr<Kiwi::Struct> struct_ = new Kiwi::Struct(symbol->KiwiName());

	struct_->AddVariable(symbol->IdentifierType()->KiwiType(), Name::Value.name);
	struct_->AddVariable(symbol->ValueKiwiType(), Name::Items.name);

	info.program->AddStruct(struct_);

	return nullptr;
}

StringBuilder EnumStatement::ToMelon(const UInt indent) const {
	StringBuilder sb = "enum ";
	sb += name.ToString();
	sb += "\n";

	String tabs = String('\t').Repeat(indent + 1);

	for (UInt i = 0; i < vars.Count(); i++) {
		ValueSymbol* valueSym = symbol->Find<ValueSymbol>(vars[i], file);

		sb += tabs;
		sb += vars[i].ToString();

		if (valueSym->type) {
			sb += '(';
			sb += valueSym->type->ToSimpleString();
			sb += ')';
		}

		sb += " = ";
		sb += String::ToString(valueSym->value);
		sb += i != vars.Count() - 1 ? ",\n" : "\n";
	}

	sb += String('\t').Repeat(indent);
	sb += "end";
	return sb;
}
