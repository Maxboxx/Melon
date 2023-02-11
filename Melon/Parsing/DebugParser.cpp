#include "DebugParser.h"

#include "ExpressionParser.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Parsing;

Ptr<Debug> DebugParser::Parse(ParsingInfo& info) {
	if (info.Current().type != TokenType::Name || info.Current().value != "_debug") return nullptr;

	info.index++;

	if (info.Current().type == TokenType::Name) {
		String type = info.Current().value;
		info.index++;

		Ptr<Expression> expr = ExpressionParser::Parse(info);

		Ptr<Debug> node = new Debug(info.scope, info.GetFileInfoPrev());
		node->type = type;
		node->argument = expr;
		return node;
	}

	info.index--;
	ErrorLog::Error(LogMessage("error.debug"), info.GetFileInfo());
	return nullptr;
}
