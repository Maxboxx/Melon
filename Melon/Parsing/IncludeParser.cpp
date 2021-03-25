#include "IncludeParser.h"

#include "Boxx/System.h"
#include "Boxx/Array.h"

#include "Melon/Symbols/SymbolTable.h"
#include "Melon/Symbols/NamespaceSymbol.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

bool IncludeParser::Parse(ParsingInfo& info) {
	if (info.Current().type == TokenType::Include) {
		info.index++;

		NameList include = NameList();

		if (info.Current().type == TokenType::Name) {
			include = include.Add(Name(info.Current().value));
			info.index++;

			while (true) {
				if (info.Current().type != TokenType::Dot) break;

				if (info.Current(1).type == TokenType::Name) {
					include = include.Add(Name(info.Current(1).value));
					info.index += 2;
				}
				else {
					break;
				}
			}

			const String fileDir = Regex::Match("^(./)~[%/\\]+$", info.filename)->groups[0];
			String includeDir = include[0].ToString();

			for (UInt i = 1; i < include.Size(); i++) {
				includeDir += "/" + include[i].ToString();
			}

			bool found = false;

			if (System::FileExists(fileDir + includeDir + ".melon")) {
				found = true;
				include = info.currentNamespace.Add(include);
				ParseFile(fileDir + includeDir + ".melon", include.Pop(), include.Last(), info);

				if (include.Size() == 1) include = include.Pop();
			}
			else if (System::DirectoryExists(fileDir + includeDir)) {
				found = true;
				include = info.currentNamespace.Add(include);
				ParseDirectory(fileDir + includeDir, include, info);
			}

			if (!found) {
				for (String dir : info.options.includeDirectories) {
					dir += includeDir;

					if (System::FileExists(dir + ".melon")) {
						found = true;
						ParseFile(dir + ".melon", include.Pop(), include.Last(), info);
						if (include.Size() == 1) include = include.Pop();
						break;
					}
					else if (System::DirectoryExists(dir)) {
						found = true;
						ParseDirectory(dir, include, info);
						break;
					}
				}
			}

			if (include.Size() > 0) {
				info.includedNamespaces.Add(include);
			}

			return true;
		}
		else {
			ErrorLog::Error(SyntaxError(SyntaxError::InvalidInclude, FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
			return false;
		}
	}

	return false;
}

void IncludeParser::ParseInclude(const NameList& include, ParsingInfo& info) {
	if (NamespaceSymbol* const ns = SymbolTable::FindAbsolute<NamespaceSymbol>(include.Pop(), FileInfo())) {
		String dir = ns->IncludedPath() + "/" + include.Last().ToString();

		if (System::DirectoryExists(dir)) {
			ParseDirectory(dir, include, info);
		}
	}
}

void IncludeParser::ParseFile(const String& filename, const NameList& include, const Name& includeFile, ParsingInfo& info) {
	String file = info.filename;
	info.filename = filename;

	NameList currentNamespace = info.currentNamespace;
	info.currentNamespace = include;

	Name currentFile = info.currentFile;
	info.currentFile = includeFile;

	Set<NameList> namespaces = info.includedNamespaces;
	info.includedNamespaces = Set<NameList>();

	List<Token> tokens = info.tokens;
	info.tokens = List<Token>();

	UInt index = info.index;
	info.index = 0;

	UInt loops = info.loops;
	info.loops = 0;

	UInt scopeCount = info.scopeCount;
	info.scopeCount = 0;

	UInt statementNumber = info.statementNumber;
	info.statementNumber = 1;

	MapSymbol* const scope = info.scope;
	info.scope = SymbolTable::FindAbsolute<NamespaceSymbol>(include, FileInfo());

	CreateIncludeSymbols(filename, include);

	Parser::ParseFile(filename, info);

	info.filename = file;
	info.currentFile = currentFile;
	info.currentNamespace = currentNamespace;
	info.includedNamespaces = namespaces;
	info.tokens = tokens;
	info.index = index;
	info.loops = loops;
	info.scopeCount = scopeCount;
	info.statementNumber = statementNumber;
	info.scope = scope;
}

void IncludeParser::CreateIncludeSymbols(const String& filename, const NameList& include) {
	for (UInt i = 0; i < include.Size(); i++) {
		NameList includeScopes;

		for (UInt u = 0; u <= i; u++) {
			includeScopes = includeScopes.Add(include[u]);
		}

		if (!SymbolTable::ContainsAbsolute(includeScopes)) {
			Array<String> dirs = filename.Replace("\\", "/").Split("/");
			String path = dirs[0];

			for (UInt u = 1; u < includeScopes.Size() + 1; u++) {
				path += "/" + dirs[u];
			}

			NamespaceSymbol* const ns = SymbolTable::FindAbsolute<NamespaceSymbol>(includeScopes.Pop(), FileInfo());
			ns->AddSymbol(includeScopes.Last(), new NamespaceSymbol(path, FileInfo(filename, 1, 0)));
		}
	}
}

void IncludeParser::ParseDirectory(const String& directory, const NameList& include, ParsingInfo& info) {
	CreateIncludeSymbols(directory, include);

	for (const String& file : System::GetFilesInDirectory(directory)) {
		Array<String> parts = file.Split(".");

		if (parts.Size() == 2 && parts[1] == "melon") {
			ParseFile(directory + "/" + file, include, Name(parts[0]), info);
		}
	}
}