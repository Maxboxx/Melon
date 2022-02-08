#include "BooleanParser.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Parsing;

Ptr<Boolean> BooleanParser::Parse(ParsingInfo& info) {
	if (info.Current().type == TokenType::Boolean) {
		Ptr<Boolean> node = new Boolean(info.GetFileInfoPrev());
		node->value = info.Current().value == "true" ? true : false;
		info.index++;
		return node;
	}

	return nullptr;
}