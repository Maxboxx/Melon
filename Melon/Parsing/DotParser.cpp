#include "DotParser.h"

#include "Melon/Nodes/DotNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

NodePtr DotParser::Parse(ParsingInfo& info) {
	if (info.Current().type != TokenType::Dot) return nullptr;

	if (info.Next().type != TokenType::Name)
		ErrorLog::Error(SyntaxError(SyntaxError::DotName, FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));

	Pointer<DotNode> dn = new DotNode(info.scopes, FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
	dn->name = Scope(info.Current().value);
	info.index++;

	return dn;
}