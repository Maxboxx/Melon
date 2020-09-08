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
		ScopeList type = ScopeList().Add((Scope)first);

		while (true) {
			if (info.Current().type != TokenType::Dot) break;
			info.index++;

			if (Optional<Scope> scope = ParseScope(info)) {
				if (Optional<List<ScopeList>> templateArgs = TemplateParser::Parse(info)) {
					scope.Get().types = templateArgs;
				}

				type = type.Add((Scope)scope);

				Symbol::TemplateSymbol ts;
				ts.type = type;
				ts.scope = info.scopes;
				ts.file = FileInfo(info.filename, info.Current(-1).line, info.statementNumber, info.currentNamespace, info.includedNamespaces);
				Symbol::templateSymbols.Add(ts);
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
	if (info.Current().type != TokenType::Name) return nullptr;

	Scope scope = Scope(info.Current().value);
	info.index++;

	if (Optional<List<ScopeList>> templateArgs = TemplateParser::Parse(info)) {
		scope.types = templateArgs;

		Symbol::TemplateSymbol ts;
		ts.type = ScopeList().Add(scope);
		ts.scope = info.scopes;
		ts.file = FileInfo(info.filename, info.Current(-1).line, info.statementNumber, info.currentNamespace, info.includedNamespaces);
		Symbol::templateSymbols.Add(ts);
	}

	return scope;
}