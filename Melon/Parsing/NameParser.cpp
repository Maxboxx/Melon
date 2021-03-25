#include "NameParser.h"

#include "Melon/Nodes/NameNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

NodePtr NameParser::Parse(ParsingInfo& info) {
	if (info.Current().type == TokenType::Name) {
		Pointer<NameNode> node = new NameNode(info.scope, info.GetFileInfo(info.Current().line));
		node->name = Name(info.Current().value);
		info.index++;
		return node;
	}

	return nullptr;
}