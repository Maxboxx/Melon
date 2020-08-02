#include "VariableAttributeParser.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

Set<SymbolAttribute> VariableAttributeParser::Parse(ParsingInfo& info) {
	Set<SymbolAttribute> attributes;

	while (true) {
		if (info.Current().type == TokenType::Const) {
			for (const SymbolAttribute a : attributes) {
				if (a == SymbolAttribute::Const) {
					ErrorLog::Error(SyntaxError(SyntaxError::MultipleAttribute("const"), FileInfo(info.filename, info.Current().line)));
				}
			}

			attributes.Add(SymbolAttribute::Const);
			info.index++;
		} 
		else if (info.Current().type == TokenType::Ref) {
			for (const SymbolAttribute a : attributes) {
				if (a == SymbolAttribute::Copy) {
					ErrorLog::Error(SyntaxError(SyntaxError::AttributeIncompatible("ref", "copy"), FileInfo(info.filename, info.Current().line)));
				}
				else if (a == SymbolAttribute::Ref) {
					ErrorLog::Error(SyntaxError(SyntaxError::MultipleAttribute("ref"), FileInfo(info.filename, info.Current().line)));
				}
			}

			attributes.Add(SymbolAttribute::Ref);
			info.index++;
		}
		else if (info.Current().type == TokenType::Copy) {
			for (const SymbolAttribute a : attributes) {
				if (a == SymbolAttribute::Ref) {
					ErrorLog::Error(SyntaxError(SyntaxError::AttributeIncompatible("copy", "ref"), FileInfo(info.filename, info.Current().line)));
				}
				else if (a == SymbolAttribute::Copy) {
					ErrorLog::Error(SyntaxError(SyntaxError::MultipleAttribute("copy"), FileInfo(info.filename, info.Current().line)));
				}
			}

			attributes.Add(SymbolAttribute::Copy);
			info.index++;
		}
		else if (info.Current().type == TokenType::Nil) {
			for (const SymbolAttribute a : attributes) {
				if (a == SymbolAttribute::Nil) {
					ErrorLog::Error(SyntaxError(SyntaxError::MultipleAttribute("nil"), FileInfo(info.filename, info.Current().line)));
				}
			}

			attributes.Add(SymbolAttribute::Nil);
			info.index++;
		}
		else {
			break;
		}
	}

	return attributes;
}