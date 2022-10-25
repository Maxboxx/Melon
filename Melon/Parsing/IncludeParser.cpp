#include "IncludeParser.h"

#include "Boxx/System.h"
#include "Boxx/Array.h"
#include "Boxx/Path.h"

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

				if (info.Peek().type == TokenType::Name) {
					include = include.Add(Name(info.Peek().value));
					info.index += 2;
				}
				else {
					break;
				}
			}

			const String fileDir = Path::GetDirectory(info.filename);
			String includeDir = include[0].ToString();

			for (UInt i = 1; i < include.Size(); i++) {
				includeDir = Path::Combine(includeDir, include[i].ToString());
			}

			const String fullDir = Path::Combine(fileDir, includeDir);

			bool found = false;

			if (System::FileExists(Path::SetExtension(fullDir, "melon"))) {
				found = true;
				include = info.currentNamespace.Add(include);
				ParseFile(Path::SetExtension(fullDir, "melon"), include.Pop(), include.Last(), info);

				if (include.Size() == 1) include = include.Pop();
			}
			else if (System::DirectoryExists(fullDir)) {
				found = true;
				include = info.currentNamespace.Add(include);
				ParseDirectory(fileDir + includeDir, include, info);
			}

			if (!found) {
				for (String dir : info.options.includeDirectories) {
					dir = Path::Combine(dir, includeDir);

					if (System::FileExists(Path::SetExtension(dir, "melon"))) {
						found = true;
						ParseFile(Path::SetExtension(dir, "melon"), include.Pop(), include.Last(), info);
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
			ErrorLog::Error(LogMessage("error.syntax.expected.after", "name", LogMessage::Quote(info.Prev().value)), info.GetFileInfoPrev());
			return false;
		}
	}

	return false;
}

void IncludeParser::ParseInclude(const NameList& include, ParsingInfo& info) {
	if (NamespaceSymbol* const ns = SymbolTable::FindAbsolute<NamespaceSymbol>(include.Pop(), FileInfo())) {
		String dir = Path::Combine(ns->IncludedPath(), include.Last().ToString());

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
				path = Path::Combine(path, dirs[u]);
			}

			NamespaceSymbol* const ns = SymbolTable::FindAbsolute<NamespaceSymbol>(includeScopes.Pop(), FileInfo());
			ns->AddSymbol(includeScopes.Last(), new NamespaceSymbol(path, FileInfo(filename, 1)));
		}
	}
}

void IncludeParser::ParseDirectory(const String& directory, const NameList& include, ParsingInfo& info) {
	CreateIncludeSymbols(directory, include);

	for (const String& file : System::GetFilesInDirectory(directory)) {
		String ext = Path::GetExtension(file);

		if (ext == "melon") {
			ParseFile(Path::Combine(directory, file), include, Name(Path::GetFileName(file)), info);
		}
	}
}