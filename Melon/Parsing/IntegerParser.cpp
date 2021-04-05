#include "IntegerParser.h"

#include "Melon/Nodes/IntegerNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Parsing;

NodePtr IntegerParser::Parse(ParsingInfo& info) {
	if (info.Current().type == TokenType::Integer) {
		Pointer<IntegerNode> node = new IntegerNode(info.GetFileInfo());
		node->scope  = info.scope;
		node->number = info.Current().value.ToULong();
		node->isUnsigned = true;
		info.index++;
		return node;
	}
	else if (info.Current().type == TokenType::Minus && info.Peek().type == TokenType::Integer) {
		Pointer<IntegerNode> node = new IntegerNode(info.GetFileInfo());
		node->scope  = info.scope;
		node->number = -(Long)info.Peek().value.ToULong();
		node->isUnsigned = false;
		info.index += 2;
		return node;
	}

	return nullptr;
}