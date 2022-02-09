#include "ObjectInitParser.h"

#include "ExpressionParser.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

Ptr<ObjectInitExpression> ObjectInitParser::Parse(ParsingInfo& info) {
	if (info.Current().type != TokenType::CurlyOpen) return nullptr;
	info.index++;

	Ptr<ObjectInitExpression> cn = new ObjectInitExpression(info.scope, info.GetFileInfoPrev());

	while (info.Current().type != TokenType::CurlyClose) {
		if (!cn->vars.IsEmpty()) {
			if (info.Current().type != TokenType::Comma) {
				ErrorLog::Error(LogMessage("error.syntax.expected.close", LogMessage::Quote("}"), "object initializer"), info.GetFileInfoPrev());
			}

			info.index++;
		}

		if (info.Current().type == TokenType::Name) {
			cn->vars.Add(Name(info.Current().value));

			if (info.Next().type != TokenType::Assign) {
				ErrorLog::Error(LogMessage("error.syntax.expected.after", LogMessage::Quote("="), LogMessage::Quote(info.Prev().value)), info.GetFileInfoPrev());
			}

			info.index++;

			if (Ptr<Expression> node = ExpressionParser::Parse(info)) {
				cn->expressions.Add(node);
			}
			else {
				ErrorLog::Error(LogMessage("error.syntax.expected.after", "expression", LogMessage::Quote("=")), info.GetFileInfoPrev());
			}
		}
		else {
			ErrorLog::Error(LogMessage("error.syntax.expected.name.object_init"), info.GetFileInfoPrev());
		}
	}

	info.index++;
	return cn;
}
