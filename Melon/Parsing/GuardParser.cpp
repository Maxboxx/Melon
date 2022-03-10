#include "GuardParser.h"

#include "ConditionParser.h"
#include "ScopeParser.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;

Ptr<GuardStatement> GuardParser::Parse(ParsingInfo& info) {
	if (info.Current().type != TokenType::Guard) {
		return nullptr;
	}

	const UInt startIndex = info.index;
	const UInt guardLine = info.Current().line;

	info.index++;

	if (Ptr<Condition> node = ConditionParser::Parse(info)) {
		info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo(guardLine));

		Ptr<GuardStatement> gn = new GuardStatement(info.scope, info.GetFileInfo(guardLine));
		gn->cond = node;
		gn->else_ = ScopeParser::Parse(info, TokenType::Else, ScopeParser::Info("else", "guard condition", "guard statement", guardLine), false);

		info.scope = info.scope->Parent<ScopeSymbol>();
		info.statementNumber++;
		return gn;
	}

	info.index = startIndex;
	return nullptr;
}