#include "CharParser.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Parsing;

Ptr<Character> CharParser::Parse(ParsingInfo& info) {
	if (info.Current().type == TokenType::Character) {
		Ptr<Character> node = new Character(info.GetFileInfoPrev());
		node->value = info.Current().value[0];
		info.index++;
		return node;
	}

	return nullptr;
}