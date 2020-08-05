#include "CustomInitParser.h"

#include "ExpressionParser.h"

#include "Melon/Nodes/CustomInitNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

NodePtr CustomInitParser::Parse(ParsingInfo& info) {
	if (info.Current().type != TokenType::CurlyOpen) return nullptr;
	info.index++;

	Pointer<CustomInitNode> cn = new CustomInitNode(info.scopes, FileInfo(info.filename, info.Current(-1).line, info.statementNumber));

	while (info.Current().type != TokenType::CurlyClose) {
		if (!cn->vars.IsEmpty()) {
			if (info.Current().type != TokenType::Comma)
				ErrorLog::Error(SyntaxError(SyntaxError::Close("'}'", "custom init"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));

			info.index++;
		}

		if (info.Current().type == TokenType::Name) {
			cn->vars.Add(Scope(info.Current().value));

			if (info.Next().type != TokenType::Assign)
				ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("'='", "'" + info.Current(-1).value + "'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));

			info.index++;

			if (NodePtr node = ExpressionParser::Parse(info)) {
				cn->expressions.Add(node);
			}
			else {
				ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("expression", "'='"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
			}
		}
		else {
			ErrorLog::Error(SyntaxError(SyntaxError::CustomInitName, FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
		}
	}

	info.index++;
	return cn;
}
