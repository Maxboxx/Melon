#include "AsParser.h"

#include "ExpressionParser.h"
#include "TypeParser.h"

#include "Melon/Nodes/ConvertNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

NodePtr AsParser::Parse(ParsingInfo& info) {
	const UInt startIndex = info.index;

	if (info.EndOfFile() || info.Current().type != TokenType::As) {
		info.index = startIndex;
		return nullptr;
	}

	const UInt asLine = info.Current().line;
	const String asValue = info.Current().value;

	info.index++;

	if (Optional<ScopeList> type = TypeParser::Parse(info)) {
		Pointer<ConvertNode> cn = new ConvertNode(info.scopes, FileInfo(info.filename, asLine, info.statementNumber));
		cn->isExplicit = true;
		cn->type = type.Get();
		return cn;
	}
	else {
		ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("type", "'" + asValue + "'"), FileInfo(info.filename, asLine, info.statementNumber)));
	}

	info.index = startIndex;
	return nullptr;
}