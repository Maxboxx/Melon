#include "IntegerParser.h"

#include "Melon/Nodes/IntegerNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Parsing;

NodePtr IntegerParser::Parse(ParsingInfo& info) {
	if (info.Current().type == TokenType::Integer) {
		Pointer<IntegerNode> node = new IntegerNode(FileInfo(info.filename, info.Current().line, info.statementNumber));
		node->number = info.Current().value.ToLong();
		info.index++;
		return node;
	}
	else if (info.Current().type == TokenType::Minus && info.Current(1).type == TokenType::Integer) {
		Pointer<IntegerNode> node = new IntegerNode(FileInfo(info.filename, info.Current().line, info.statementNumber));
		node->number = -info.Current(1).value.ToLong();
		info.index += 2;
		return node;
	}

	return nullptr;
}