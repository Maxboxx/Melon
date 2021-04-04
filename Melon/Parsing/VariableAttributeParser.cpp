#include "VariableAttributeParser.h"

#include "Melon/Symbols/VariableSymbol.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

VariableAttributes VariableAttributeParser::Parse(ParsingInfo& info, const bool includeRef) {
	VariableAttributes attributes = VariableAttributes::None;

	while (true) {
		if (info.Current().type == TokenType::Const) {
			if ((attributes & VariableAttributes::Const) != VariableAttributes::None) {
				ErrorLog::Error(LogMessage("error.syntax.attrubute.multiple", "const"), FileInfo(info.filename, info.Current().line, info.statementNumber));
			}

			attributes |= VariableAttributes::Const;
			info.index++;
		} 
		else if (info.Current().type == TokenType::Ref) {
			if (!includeRef) {
				// TODO: Remove
				ErrorLog::Error(LogMessage("error.scan.use.ref"), FileInfo(info.filename, info.Current().line, info.statementNumber));
			}
			else if ((attributes & VariableAttributes::Ref) != VariableAttributes::None) {
				ErrorLog::Error(LogMessage("error.syntax.attrubute.multiple", "ref"), FileInfo(info.filename, info.Current().line, info.statementNumber));
			}

			attributes |= VariableAttributes::Ref;
			info.index++;
		}
		else {
			break;
		}
	}

	return attributes;
}
