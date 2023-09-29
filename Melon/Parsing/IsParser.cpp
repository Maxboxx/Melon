#include "IsParser.h"

#include "ExpressionParser.h"
#include "TypeParser.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

Ptr<IsExpression> IsParser::Parse(ParsingInfo& info) {
	const UInt startIndex = info.index;

	// Parse is token
	if (info.EndOfFile() || info.Current().type != TokenType::Is) {
		info.index = startIndex;
		return nullptr;
	}

	// Get token info
	const UInt isLine = info.Current().line;
	const String isValue = info.Current().value;

	info.index++;
	
	Optional<NameList> type = TypeParser::Parse(info);

	if (!type) {
		ErrorLog::Error(LogMessage("error.syntax.expected.after", "type", LogMessage::Quote(isValue)), info.GetFileInfo(isLine));
		return nullptr;
	}

	Ptr<IsExpression> cn = new IsExpression(info.scope, info.GetFileInfo(isLine));
	cn->type = *type;
	return cn;
}
