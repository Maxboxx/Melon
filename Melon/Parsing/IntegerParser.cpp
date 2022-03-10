#include "IntegerParser.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Parsing;

Ptr<Integer> IntegerParser::Parse(ParsingInfo& info) {
	if (info.Current().type == TokenType::Integer) {
		Ptr<Integer> node = new Integer(info.GetFileInfo());
		node->number = info.Current().value.ToULong();
		node->isUnsigned = true;
		info.index++;
		return node;
	}
	else if (info.Current().type == TokenType::Minus && info.Peek().type == TokenType::Integer) {
		Ptr<Integer> node = new Integer(info.GetFileInfo());
		node->number = -(Long)info.Peek().value.ToULong();
		node->isUnsigned = false;
		info.index += 2;
		return node;
	}

	return nullptr;
}