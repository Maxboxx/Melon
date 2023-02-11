#include "EnumStatement.h"

#include "Melon/Symbols/EnumSymbol.h"
#include "Melon/Symbols/ValueSymbol.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

EnumStatement::EnumStatement(Symbol* const scope, const FileInfo& file) : Statement(scope, file) {

}

EnumStatement::~EnumStatement() {

}

Ptr<Kiwi::Value> EnumStatement::Compile(CompileInfo& info) {
	return nullptr;
}

StringBuilder EnumStatement::ToMelon(const UInt indent) const {
	StringBuilder sb = "enum ";
	sb += name.ToString();
	sb += "\n";

	String tabs = String('\t').Repeat(indent + 1);

	for (UInt i = 0; i < values.Count(); i++) {
		sb += tabs;
		sb += values[i].ToString();
		sb += " = ";
		sb += String::ToString(symbol->Find<ValueSymbol>(values[i], file)->value);
		sb += i != values.Count() - 1 ? ",\n" : "\n";
	}

	sb += String('\t').Repeat(indent);
	sb += "end";
	return sb;
}
