#include "ReturnParser.h"

#include "ExpressionParser.h"

#include "Melon/Nodes/ReturnNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

NodePtr ReturnParser::Parse(ParsingInfo& info) {
	const UInt startIndex = info.index;

	if (info.Current().type == TokenType::Return) {
		info.index++;

		Pointer<ReturnNode> ret = new ReturnNode(info.scope->AbsoluteName(), FileInfo(info.filename, info.Current(-1).line, info.statementNumber, info.currentNamespace, info.includedNamespaces));

		while (NodePtr node = ExpressionParser::Parse(info)) {
			ret->nodes.Add(node);

			if (info.Current().type == TokenType::Comma) {
				info.index++;
			}
			else {
				break;
			}
		}

		info.statementNumber++;
		return ret;
	}

	info.index = startIndex;
	return nullptr;
}