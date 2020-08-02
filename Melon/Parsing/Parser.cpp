#include "Parser.h"

#include "Boxx/Regex.h"

#include "StatementParser.h"

#include "Melon/Nodes/RootNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

Token ParsingInfo::Next() {
	if (index + 1 >= tokens.Size())
		ErrorLog::Error(SyntaxError(SyntaxError::UnexpectedEOF, FileInfo(filename, tokens.Last().line)));

	return tokens[++index];
}

Token ParsingInfo::Previous() {
	if (index - 1 >= tokens.Size())
		ErrorLog::Error(SyntaxError(SyntaxError::UnexpectedEOF, FileInfo(filename, tokens.Last().line)));

	return tokens[--index];
}

bool ParsingInfo::EndOfFile() const {
	return index >= tokens.Size();
}

Token ParsingInfo::Current() {
	if (index >= tokens.Size())
		ErrorLog::Error(SyntaxError(SyntaxError::UnexpectedEOF, FileInfo(filename, tokens.Last().line)));

	return tokens[index];
}

Token ParsingInfo::Current(const Int offset) {
	if (index + offset >= tokens.Size())
		ErrorLog::Error(SyntaxError(SyntaxError::UnexpectedEOF, FileInfo(filename, tokens.Last().line)));

	return tokens[index + offset];
}

RootNode Parser::Parse(const List<Token>& tokens, const String& filename) {
	ParsingInfo info;
	info.filename = filename;
	info.tokens = tokens;
	info.index = 0;
	info.loops = 0;

	while (!info.EndOfFile()) {
		if (!ParseNext(info)) {
			break;
		}
	}

	return info.root;
}

NodePtr Parser::UnexpectedToken(ParsingInfo& info) {
	ErrorLog::Error(SyntaxError(SyntaxError::UnexpectedTokenStart + info.Current().value + SyntaxError::UnexpectedTokenEnd, FileInfo(info.filename, info.Current().line)));
	return nullptr;
}

NodePtr Parser::ParseNext(ParsingInfo& info) {
	if (NodePtr node = StatementParser::Parse(info)) {
		info.root.nodes.Add(node);
		return node;
	}

	return UnexpectedToken(info);
}