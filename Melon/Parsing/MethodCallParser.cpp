#include "MethodCallParser.h"

#include "CallParser.h"

#include "Melon/Nodes/CallNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

NodePtr MethodCallParser::Parse(ParsingInfo& info) {
	if (info.EndOfFile()) return nullptr;

	const UInt startIndex = info.index;

	if (info.Current().type == TokenType::Colon && info.Current(1).type == TokenType::Name) {
		Name name = Name(info.Current(1).value);
		info.index += 2;

		if (NodePtr node = CallParser::Parse(info)) {
			Pointer<CallNode> cn = node.Cast<CallNode>();
			cn->isMethod = true;
			cn->methodName = name;

			return cn;
		}
	}

	info.index = startIndex;
	return nullptr;
}