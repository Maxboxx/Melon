#include "SwitchExpressionParser.h"

#include "ExpressionParser.h"

#include "Melon/Nodes/SwitchNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

NodePtr SwitchExpressionParser::Parse(ParsingInfo& info, const bool returnOnError) {
	const UInt startIndex = info.index;

	if (info.Current().type != TokenType::Switch) {
		return nullptr;
	}

	const UInt switchLine = info.Current().line;
	info.index++;

	NodePtr value = ExpressionParser::Parse(info);

	bool error = false;

	if (!value) {
		ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfterIn("match expression", "'switch'", "switch expression"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
		error = true;
	}

	Pointer<SwitchNode> switchexpr = new SwitchNode(info.scope->AbsoluteName(), FileInfo(info.filename, switchLine, info.statementNumber));
	switchexpr->expr = true;
	switchexpr->match = value;

	while (info.Current().type == TokenType::Case || info.Current().type == TokenType::Default) {
		bool isDefault = info.Current().type == TokenType::Default;
		const UInt caseLine = info.Current().line;

		if (isDefault && switchexpr->def) {
			ErrorLog::Error(SyntaxError(SyntaxError::MultipleDefaultExpr, FileInfo(info.filename, info.Current().line, info.statementNumber)));
			error = true;
		}

		info.index++;

		List<NodePtr> caseValues;

		if (!isDefault) {
			while (NodePtr node = ExpressionParser::Parse(info)) {
				caseValues.Add(node);

				if (info.Current().type != TokenType::Comma) break;
				info.index++;
			}

			if (caseValues.IsEmpty()) {
				ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfterIn("case expression", "'case'", "switch expression"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
				error = true;
			}
		}

		bool hasThen = false;

		if (info.Current().type == TokenType::Then) {
			info.index++;
			hasThen = true;
		}
		else if (info.Current().type == TokenType::Arrow) {
			info.index++;
		}
		else {
			ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("'then'", "case expression"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
			error = true;
		}

		if (NodePtr node = ExpressionParser::Parse(info)) {
			if (isDefault) {
				switchexpr->def = node;
			}
			else {
				switchexpr->nodes.Add(node);
				switchexpr->cases.Add(caseValues);
			}
		}
		else {
			ErrorLog::Error(SyntaxError(SyntaxError::ExprSwitchCase, FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
			error = true;
		}

		if (hasThen) {
			if (info.Current().type == TokenType::End) {
				info.index++;
			}
			else {
				ErrorLog::Error(SyntaxError(SyntaxError::EndExpected("switch case", caseLine), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
				error = true;
			}
		}
	}

	if (!switchexpr->def) {
		ErrorLog::Error(SyntaxError(SyntaxError::DefaultRequired, FileInfo(info.filename, switchLine, info.statementNumber)));
	}

	if (info.Current().type == TokenType::End) {
		info.index++;
	}
	else {
		ErrorLog::Error(SyntaxError(SyntaxError::EndExpected("switch expression", switchLine), FileInfo(info.filename, switchLine, info.statementNumber)));
		error = true;
	}

	if (error && returnOnError) return nullptr;
	return switchexpr;
}