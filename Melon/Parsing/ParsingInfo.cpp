#include "ParsingInfo.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;

Melon::Token ParsingInfo::Next() {
	if (index + 1 >= tokens.Count()) {
		ErrorLog::Error(LogMessage("error.syntax.unexpected.eof"), FileInfo(filename, tokens.Last().line));
		return Token();
	}

	return tokens[++index];
}

Melon::Token ParsingInfo::Next(const UInt steps) {
	if (index + steps >= tokens.Count()) {
		ErrorLog::Error(LogMessage("error.syntax.unexpected.eof"), FileInfo(filename, tokens.Last().line));
		return Token();
	}

	index += steps;
	return tokens[index];
}

Melon::Token ParsingInfo::Peek() const {
	if (index + 1 >= tokens.Count()) {
		return Token();
	}

	return tokens[index + 1];
}

Melon::Token ParsingInfo::Peek(const UInt steps) const {
	if (index + steps >= tokens.Count()) {
		return Token();
	}

	return tokens[index + steps];
}

Melon::Token ParsingInfo::Prev() const {
	if (index - 1 >= tokens.Count()) {
		return Token();
	}

	return tokens[index - 1];
}

Melon::Token ParsingInfo::Prev(const UInt steps) const {
	if (index - steps >= tokens.Count()) {
		return Token();
	}

	return tokens[index - steps];
}

bool ParsingInfo::EndOfFile() const {
	return index >= tokens.Count();
}

Melon::Token ParsingInfo::Current() const {
	if (index >= tokens.Count()) {
		ErrorLog::Error(LogMessage("error.syntax.unexpected.eof"), FileInfo(filename, tokens.Last().line));
		return Token();
	}

	return tokens[index];
}

FileInfo ParsingInfo::GetFileInfo() const {
	FileInfo file;
	file.line = Current().line;
	file.filename  = filename;
	file.includeName = currentFile;
	file.statement = statementNumber;
	file.currentNamespace = currentNamespace;
	file.includedNamespaces = includedNamespaces;
	return file;
}

FileInfo ParsingInfo::GetFileInfo(const UInt line) const {
	FileInfo file;
	file.filename  = filename;
	file.line      = line;
	file.includeName = currentFile;
	file.statement = statementNumber;
	file.currentNamespace = currentNamespace;
	file.includedNamespaces = includedNamespaces;
	return file;
}

FileInfo ParsingInfo::GetFileInfoNext() const {
	FileInfo file;
	file.line = Peek().line;
	file.filename  = filename;
	file.includeName = currentFile;
	file.statement = statementNumber;
	file.currentNamespace = currentNamespace;
	file.includedNamespaces = includedNamespaces;
	return file;
}

FileInfo ParsingInfo::GetFileInfoNext(const UInt steps) const {
	FileInfo file;
	file.filename  = filename;
	file.line      = Peek(steps).line;
	file.includeName = currentFile;
	file.statement = statementNumber;
	file.currentNamespace = currentNamespace;
	file.includedNamespaces = includedNamespaces;
	return file;
}

FileInfo ParsingInfo::GetFileInfoPrev() const {
	FileInfo file;
	file.line = Prev().line;
	file.filename  = filename;
	file.includeName = currentFile;
	file.statement = statementNumber;
	file.currentNamespace = currentNamespace;
	file.includedNamespaces = includedNamespaces;
	return file;
}

FileInfo ParsingInfo::GetFileInfoPrev(const UInt steps) const {
	FileInfo file;
	file.filename  = filename;
	file.line      = Prev(steps).line;
	file.includeName = currentFile;
	file.statement = statementNumber;
	file.currentNamespace = currentNamespace;
	file.includedNamespaces = includedNamespaces;
	return file;
}
