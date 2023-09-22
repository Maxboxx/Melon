#include "AsParser.h"

#include "ExpressionParser.h"
#include "TypeParser.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

Ptr<TypeConversion> AsParser::Parse(ParsingInfo& info) {
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
	
	Optional<NameList> type = TypeParser::Parse(info);

	if (!type && info.Current().type == TokenType::Any) {
		info.index++;
		type = NameList::Any;
	}

	// Parse type
	if (type) {
		Ptr<TypeConversion> cn = new TypeConversion(info.scope, info.GetFileInfo(asLine));
		cn->isExplicit = true;
		cn->type = *type;
		return cn;
	}
	else {
		ErrorLog::Error(LogMessage("error.syntax.expected.after", "type", LogMessage::Quote(asValue)), info.GetFileInfo(asLine));
	}

	info.index = startIndex;
	return nullptr;
}
