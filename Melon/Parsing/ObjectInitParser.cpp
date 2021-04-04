#include "ObjectInitParser.h"

#include "ExpressionParser.h"

#include "Melon/Nodes/ObjectInitNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

NodePtr ObjectInitParser::Parse(ParsingInfo& info) {
	if (info.Current().type != TokenType::CurlyOpen) return nullptr;
	info.index++;

	Pointer<ObjectInitNode> cn = new ObjectInitNode(info.scope, info.GetFileInfo(info.Current(-1).line));

	while (info.Current().type != TokenType::CurlyClose) {
		if (!cn->vars.IsEmpty()) {
			if (info.Current().type != TokenType::Comma) {
				ErrorLog::Error(LogMessage("error.syntax.expected.close", "'}'", "object initializer"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
			}

			info.index++;
		}

		if (info.Current().type == TokenType::Name) {
			cn->vars.Add(Name(info.Current().value));

			if (info.Next().type != TokenType::Assign) {
				ErrorLog::Error(LogMessage("error.syntax.expected.after", "'='", LogMessage::Quote(info.Current(-1).value)), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
			}

			info.index++;

			if (NodePtr node = ExpressionParser::Parse(info)) {
				cn->expressions.Add(node);
			}
			else {
				ErrorLog::Error(LogMessage("error.syntax.expected.after", "expression", "'='"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
			}
		}
		else {
			ErrorLog::Error(LogMessage("error.syntax.expected.name.object_init"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
		}
	}

	info.index++;
	return cn;
}
