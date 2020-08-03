#include "IncludeParser.h"

#include "Boxx/System.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

bool IncludeParser::Parse(ParsingInfo& info) {
	if (info.Current().type == TokenType::Include) {
		info.index++;

		ScopeList include = ScopeList();

		if (info.Current().type == TokenType::Name) {
			include = include.Add(Scope(info.Current().value));
			info.index++;

			while (true) {
				if (info.Current().type != TokenType::Dot) break;

				if (info.Current(1).type == TokenType::Name) {
					include = include.Add(Scope(info.Current(1).value));
					info.index += 2;
				}
				else {
					break;
				}
			}

			const String fileDir = Regex("^(./)~[%/\\]+$").Match(info.filename)[0];
			String includeDir = include[0].ToString();

			for (UInt i = 1; i < include.Size(); i++) {
				includeDir += "/" + include[i].ToString();
			}

			bool found = false;

			if (System::FileExists(fileDir + includeDir + ".melon")) {
				found = true;
				include = info.currentNamespace.Add(include);
				ParseFile(fileDir + includeDir + ".melon", include.Pop(), info);
			}
			else if (System::DirectoryExists(fileDir + includeDir)) {
				found = true;
				include = info.currentNamespace.Add(include);
			}

			if (!found) {
				for (String dir : info.options.includeDirectories) {
					dir += includeDir;

					if (System::FileExists(dir + ".melon")) {
						found = true;
						ParseFile(dir + ".melon", include.Pop(), info);
						break;
					}
					else if (System::DirectoryExists(dir)) {
						found = true;
						break;
					}
				}
			}

			info.includedNamespaces.Add(include);
			return true;
		}
		else {
			ErrorLog::Error(SyntaxError(SyntaxError::InvalidInclude, FileInfo(info.filename, info.Current(-1).line)));
			return false;
		}
	}

	return false;
}

void IncludeParser::ParseFile(const String& filename, const ScopeList& include, ParsingInfo& info) {
	String file = info.filename;
	info.filename = filename;

	ScopeList currentNamespace = info.currentNamespace;
	info.currentNamespace = include;

	Set<ScopeList> namespaces = info.includedNamespaces;
	info.includedNamespaces = Set<ScopeList>();

	List<Token> tokens = info.tokens;
	info.tokens = List<Token>();

	UInt index = info.index;
	info.index = 0;

	UInt loops = info.loops;
	info.loops = 0;

	ScopeList scopes = info.scopes;
	info.scopes = include;

	for (UInt i = 0; i < include.Size(); i++) {
		ScopeList includeScopes;

		for (UInt u = 0; u <= i; u++) {
			includeScopes = includeScopes.Add(include[u]);
		}

		if (!Symbol::Contains(includeScopes)) {
			Symbol s = Symbol(SymbolType::Namespace);
			Symbol::Add(includeScopes, s, FileInfo(info.filename, 1));
		}
	}

	Parser::ParseFile(filename, info);

	info.filename = file;
	info.currentNamespace = currentNamespace;
	info.includedNamespaces = namespaces;
	info.tokens = tokens;
	info.index = index;
	info.loops = loops;
	info.scopes = scopes;
}