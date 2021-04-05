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
		ErrorLog::Error(LogMessage("error.syntax.expected.after", "match expression", LogMessage::Quote("switch")), info.GetFileInfoPrev());
		error = true;
	}

	Pointer<SwitchNode> switchexpr = new SwitchNode(info.scope, info.GetFileInfo(switchLine));
	switchexpr->expr = true;
	switchexpr->match = value;

	while (info.Current().type == TokenType::Case || info.Current().type == TokenType::Default) {
		bool isDefault = info.Current().type == TokenType::Default;
		const UInt caseLine = info.Current().line;

		if (isDefault && switchexpr->def) {
			ErrorLog::Error(LogMessage("error.syntax.switch.default.multiple.expr"), info.GetFileInfo());
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
				ErrorLog::Error(LogMessage("error.syntax.expected.after_in", "case expression", LogMessage::Quote("case"), "switch expression"), info.GetFileInfoPrev());
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
			ErrorLog::Error(LogMessage("error.syntax.expected.after", LogMessage::Quote("then"), "case expression"), info.GetFileInfoPrev());
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
			ErrorLog::Error(LogMessage("error.syntax.expected.in", "expression", "switch case"), info.GetFileInfoPrev());
			error = true;
		}

		if (hasThen) {
			if (info.Current().type == TokenType::End) {
				info.index++;
			}
			else {
				ErrorLog::Error(LogMessage("error.syntax.expected.end_at", "switch case", caseLine), info.GetFileInfoPrev());
				error = true;
			}
		}
	}

	if (!switchexpr->def) {
		ErrorLog::Error(LogMessage("error.syntax.switch.default.required"), info.GetFileInfo(switchLine));
	}

	if (info.Current().type == TokenType::End) {
		info.index++;
	}
	else {
		ErrorLog::Error(LogMessage("error.syntax.expected.end_at", "switch expression", switchLine), info.GetFileInfo(switchLine));
		error = true;
	}

	if (error && returnOnError) return nullptr;
	return switchexpr;
}