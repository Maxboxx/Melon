#include "VariableAttributeParser.h"

#include "Melon/Symbols/VariableSymbol.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

VariableModifiers VariableAttributeParser::Parse(ParsingInfo& info, const bool includeRef) {
	VariableModifiers modifiers = VariableModifiers::None;

	while (true) {
		if (info.Current().type == TokenType::Const) {
			if ((modifiers & VariableModifiers::Const) != VariableModifiers::None) {
				ErrorLog::Error(LogMessage("error.syntax.attrubute.multiple", "const"), info.GetFileInfo());
			}

			modifiers |= VariableModifiers::Const;
			info.index++;
		} 
		else if (info.Current().type == TokenType::Ref) {
			if (!includeRef) {
				// TODO: Remove
				ErrorLog::Error(LogMessage("error.scan.use.ref"), info.GetFileInfo());
			}
			else if ((modifiers & VariableModifiers::Ref) != VariableModifiers::None) {
				ErrorLog::Error(LogMessage("error.syntax.attrubute.multiple", "ref"), info.GetFileInfo());
			}

			modifiers |= VariableModifiers::Ref;
			info.index++;
		}
		else {
			break;
		}
	}

	return modifiers;
}
