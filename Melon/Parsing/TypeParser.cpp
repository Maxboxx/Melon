#include "TypeParser.h"

#include "TemplateParser.h"

#include "Melon/Nodes/NameNode.h"
#include "Melon/Nodes/DotNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

Optional<ScopeList> TypeParser::Parse(ParsingInfo& info) {
	Optional<Scope> first = nullptr;

	if (info.Current().type == TokenType::Global) {
		first = Scope::Global;
		info.index++;
	}
	else {
		first = ParseScope(info);
	}

	if (first) {
		while (!info.EndOfFile() && (info.Current().type == TokenType::Question || info.Current().type == TokenType::DoubleQuestion)) {
			UInt n = info.Current().type == TokenType::Question ? 1 : 2;
			info.index++;

			for (UInt i = 0; i < n; i++) {
				Scope optionalScope = Scope::Optional;
				optionalScope.types = List<ScopeList>();
				optionalScope.types.Get().Add(ScopeList().Add((Scope)first));
				first = optionalScope;

				SymbolTable::SpecializeTemplate(ScopeList().Add((Scope)first), info.scope->AbsoluteName(), info.GetFileInfo(info.Current(-1).line));
			}
		}

		ScopeList type = ScopeList().Add((Scope)first);

		while (!info.EndOfFile()) {
			if (info.Current().type != TokenType::Dot) break;
			info.index++;

			if (Optional<Scope> scope = ParseScope(info)) {
				type = type.Add((Scope)scope);

				if (scope.Get().types) {
					SymbolTable::SpecializeTemplate(type, info.scope->AbsoluteName(), info.GetFileInfo(info.Current(-1).line));
				}

				while (!info.EndOfFile() && (info.Current().type == TokenType::Question || info.Current().type == TokenType::DoubleQuestion)) {
					UInt n = info.Current().type == TokenType::Question ? 1 : 2;
					info.index++;

					for (UInt i = 0; i < n; i++) {
						Scope optionalScope = Scope::Optional;
						optionalScope.types = List<ScopeList>();
						optionalScope.types.Get().Add(ScopeList().Add(type));
						type = ScopeList().Add(optionalScope);

						SymbolTable::SpecializeTemplate(ScopeList().Add(type), info.scope->AbsoluteName(), info.GetFileInfo(info.Current(-1).line));
					}
				}
			}
			else {
				info.index--;
				break;
			}
		}

		if (type.Size() == 1 && type[0] == Scope::Global) {
			if (info.Current().type == TokenType::Dot) {
				ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("'.'", "'global'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
			}
			else {
				ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("name", "'.'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
			}

			return nullptr;
		}

		return type;
	}

	return nullptr;
}

Optional<Scope> TypeParser::ParseScope(ParsingInfo& info) {
	if (
		info.Current().type != TokenType::Name &&
		info.Current().type != TokenType::Type &&
		info.Current().type != TokenType::Nil
	) {
		return nullptr;
	}

	Scope scope = Scope(info.Current().value);
	info.index++;

	if (info.EndOfFile()) return scope;

	if (Optional<List<ScopeList>> templateArgs = TemplateParser::Parse(info)) {
		scope.types = templateArgs;

		SymbolTable::SpecializeTemplate(ScopeList().Add(scope), info.scope->AbsoluteName(), info.GetFileInfo(info.Current(-1).line));
	}

	return scope;
}