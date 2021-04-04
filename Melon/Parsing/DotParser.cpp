#include "DotParser.h"

#include "TemplateParser.h"

#include "Melon/Nodes/DotNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

NodePtr DotParser::Parse(ParsingInfo& info) {
	if (info.Current().type != TokenType::Dot) return nullptr;

	if (info.Next().type != TokenType::Name) {
		ErrorLog::Error(LogMessage("error.syntax.expected.after", "name", "'.'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
	}

	Pointer<DotNode> dn = new DotNode(info.scope, info.GetFileInfo(info.Current(-1).line));
	dn->name = Name(info.Current().value);

	info.index++;

	if (Optional<List<NameList>> templateArgs = TemplateParser::Parse(info)) {
		dn->name.types = templateArgs;
	}

	return dn;
}