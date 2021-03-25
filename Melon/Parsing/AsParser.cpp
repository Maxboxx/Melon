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

	// Parse as token
	if (info.EndOfFile() || info.Current().type != TokenType::As) {
		info.index = startIndex;
		return nullptr;
	}

	// Get token info
	const UInt asLine = info.Current().line;
	const String asValue = info.Current().value;

	info.index++;

	// Parse type
	if (Optional<ScopeList> type = TypeParser::Parse(info)) {
		Pointer<ConvertNode> cn = new ConvertNode(info.scope, info.GetFileInfo(asLine));
		cn->isExplicit = true;
		cn->type = *type;
		return cn;
	}
	else {
		ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("type", "'" + asValue + "'"), info.GetFileInfo(asLine)));
	}

	info.index = startIndex;
	return nullptr;
}