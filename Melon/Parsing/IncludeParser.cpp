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
	if (info.Current().type != TokenType::Include) return false;
	info.index++;

	NameList include = NameList();

	if (info.Current().type != TokenType::Name) {
		ErrorLog::Error(LogMessage("error.syntax.expected.after", "name", LogMessage::Quote(info.Prev().value)), info.GetFileInfoPrev());
		return false;
	}

	include = include.Add(Name(info.Current().value));
	info.index++;

	while (true) {
		if (info.Current().type != TokenType::Dot) break;
		if (info.Peek().type != TokenType::Name) break;

		include = include.Add(Name(info.Peek().value));
		info.index += 2;
	}

	Include(include, info);

	return true;
}

void IncludeParser::Include(const NameList& include, ParsingInfo& info) {
	String dir = Path::GetDirectory(info.GetFileInfo().filename);
	String ns = Path::Combine(dir, include[0].name);

	if (System::DirectoryExists(ns)) {
		IncludeNamespace(ns, include, 0, info);
	}
}

void IncludeParser::IncludeNamespace(const String& dir, const NameList& include, UInt index, ParsingInfo& info) {
	MapSymbol* ns = SymbolTable::FindAbsolute<MapSymbol>(info.currentNamespace, info.GetFileInfo());
	if (!ns) return;

	NameList nsList = ns->AddSymbol(include[index], new NamespaceSymbol(dir, info.GetFileInfo()))->AbsoluteName();

	if (index + 1 < include.Size()) {
		String newDir = Path::Combine(dir, include[index + 1].name);

		if (System::DirectoryExists(newDir)) {
			IncludeNamespace(newDir, include, index + 1, info);
		}
	}

	for (const String& file : System::GetFilesInDirectory(dir)) {
		if (Path::GetExtension(file) != "melon") continue;
		ParseFile(Path::Combine(dir, file), nsList, Name(Path::GetFileName(file)), info);
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