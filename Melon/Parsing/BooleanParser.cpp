#include "BooleanParser.h"

#include "Melon/Nodes/BooleanNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Parsing;

NodePtr BooleanParser::Parse(ParsingInfo& info) {
	if (info.Current().type == TokenType::Boolean) {
		Pointer<BooleanNode> node = new BooleanNode(FileInfo(info.filename, info.Current().line));
		node->boolean = info.Current().value == "true" ? true : false;
		info.index++;
		return node;
	}

	return nullptr;
}