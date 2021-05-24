#include "DoParser.h"

#include "ScopeParser.h"

#include "Melon/Nodes/DoNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

NodePtr DoParser::Parse(ParsingInfo& info) {
	if (info.Current().type != TokenType::Do) return nullptr;

	const UInt line = info.Current().line;
	info.index++;
	info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo(line));

	Pointer<DoNode> node = new DoNode(info.scope, info.GetFileInfo(line));
	info.scopeCount++;

	if (NodePtr nodes = ScopeParser::Parse(info, TokenType::None, ScopeParser::Info("do statement", line), true)) {
		node->nodes = nodes;
		info.scope = info.scope->Parent<ScopeSymbol>();
		info.statementNumber++;
		return node;
	}

	info.scope = info.scope->Parent<ScopeSymbol>();
	return nullptr;
}
