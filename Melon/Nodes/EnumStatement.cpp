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

CompiledNode EnumStatement::Compile(CompileInfo& info) {
	return CompiledNode();
}

StringBuilder EnumStatement::ToMelon(const UInt indent) const {
	StringBuilder sb = "enum ";
	sb += name.ToString();
	sb += "\n";

	String tabs = String('\t').Repeat(indent + 1);

	for (UInt i = 0; i < values.Size(); i++) {
		sb += tabs;
		sb += values[i].ToString();
		sb += " = ";
		sb += String::ToString(symbol->Find<ValueSymbol>(values[i], file)->value);
		sb += i != values.Size() - 1 ? ",\n" : "\n";
	}

	sb += String('\t').Repeat(indent);
	sb += "end";
	return sb;
}
