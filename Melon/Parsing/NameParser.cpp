#include "NameParser.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

Ptr<NameExpression> NameParser::Parse(ParsingInfo& info) {
	if (info.Current().type == TokenType::Name) {
		Ptr<NameExpression> node = new NameExpression(info.scope, info.GetFileInfo());
		node->name = Name(info.Current().value);
		info.index++;
		return node;
	}

	return nullptr;
}