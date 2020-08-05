#include "NameParser.h"

#include "Melon/Nodes/NameNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

NodePtr NameParser::Parse(ParsingInfo& info) {
	if (info.Current().type == TokenType::Name) {
		Pointer<NameNode> node = new NameNode(info.scopes, FileInfo(info.filename, info.Current().line, info.statementNumber, info.currentNamespace, info.includedNamespaces));
		node->name = Scope(info.Current().value);
		info.index++;
		return node;
	}

	return nullptr;
}