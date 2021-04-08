#include "GuardParser.h"

#include "ConditionParser.h"
#include "ScopeParser.h"

#include "Melon/Nodes/GuardNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;

NodePtr GuardParser::Parse(ParsingInfo& info) {
	if (info.Current().type != TokenType::Guard) {
		return nullptr;
	}

	const UInt startIndex = info.index;
	const UInt guardLine = info.Current().line;

	info.index++;

	if (NodePtr node = ConditionParser::Parse(info)) {
		info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo(guardLine));

		Pointer<GuardNode> gn = new GuardNode(info.scope, info.GetFileInfo(guardLine));
		gn->cond = node;
		gn->else_ = ScopeParser::Parse(info, TokenType::Else, "guard statement", guardLine);

		info.scope = info.scope->Parent<ScopeSymbol>();
		info.statementNumber++;
		return gn;
	}

	info.index = startIndex;
	return nullptr;
}