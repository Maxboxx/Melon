#include "StringParser.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Parsing;

Ptr<StringLiteral> StringParser::Parse(ParsingInfo& info) {
	if (info.Current().type == TokenType::StringLiteral) {
		Ptr<StringLiteral> node = new StringLiteral(info.GetFileInfoPrev());
		node->value = info.Current().value;
		info.index++;
		return node;
	}

	return nullptr;
}