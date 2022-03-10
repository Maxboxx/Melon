#include "TypeParser.h"

#include "TemplateParser.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

Optional<NameList> TypeParser::Parse(ParsingInfo& info) {
	Optional<Name> first = nullptr;

	if (info.Current().type == TokenType::Global) {
		first = Name::Global;
		info.index++;
	}
	else {
		first = ParseName(info);
	}

	if (first) {
		while (!info.EndOfFile() && (info.Current().type == TokenType::Question || info.Current().type == TokenType::DoubleQuestion)) {
			UInt n = info.Current().type == TokenType::Question ? 1 : 2;
			info.index++;

			for (UInt i = 0; i < n; i++) {
				Name optionalScope = Name::Optional;
				optionalScope.types = List<NameList>();
				optionalScope.types->Add(NameList().Add(*first));
				first = optionalScope;

				SymbolTable::SpecializeTemplate(NameList().Add(*first), info.scope, info.GetFileInfoPrev());
			}
		}

		NameList type = NameList().Add(*first);

		while (!info.EndOfFile()) {
			if (info.Current().type != TokenType::Dot) break;
			info.index++;

			if (Optional<Name> scope = ParseName(info)) {
				type = type.Add(*scope);

				if (scope->types) {
					SymbolTable::SpecializeTemplate(type, info.scope, info.GetFileInfoPrev());
				}

				while (!info.EndOfFile() && (info.Current().type == TokenType::Question || info.Current().type == TokenType::DoubleQuestion)) {
					UInt n = info.Current().type == TokenType::Question ? 1 : 2;
					info.index++;

					for (UInt i = 0; i < n; i++) {
						Name optionalScope = Name::Optional;
						optionalScope.types = List<NameList>();
						optionalScope.types->Add(NameList().Add(type));
						type = NameList().Add(optionalScope);

						SymbolTable::SpecializeTemplate(NameList().Add(type), info.scope, info.GetFileInfoPrev());
					}
				}
			}
			else {
				info.index--;
				break;
			}
		}

		if (type.Size() == 1 && type[0] == Name::Global) {
			if (info.Current().type == TokenType::Dot) {
				ErrorLog::Error(LogMessage("error.syntax.expected.after", LogMessage::Quote("."), LogMessage::Quote("global")), info.GetFileInfoPrev());
			}
			else {
				ErrorLog::Error(LogMessage("error.syntax.expected.after", "name", LogMessage::Quote(".")), info.GetFileInfoPrev());
			}

			return nullptr;
		}

		return type;
	}

	return nullptr;
}

Optional<Name> TypeParser::ParseName(ParsingInfo& info) {
	if (
		info.Current().type != TokenType::Name &&
		info.Current().type != TokenType::Type &&
		info.Current().type != TokenType::Nil
	) {
		return nullptr;
	}

	Name scope = Name(info.Current().value);
	info.index++;

	if (info.EndOfFile()) return scope;

	if (Optional<List<NameList>> templateArgs = TemplateParser::Parse(info)) {
		scope.types = templateArgs;

		SymbolTable::SpecializeTemplate(NameList().Add(scope), info.scope, info.GetFileInfoPrev());
	}

	return scope;
}