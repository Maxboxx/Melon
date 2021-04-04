#include "SwitchParser.h"

#include "ExpressionParser.h"
#include "StatementParser.h"

#include "Melon/Nodes/SwitchNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

NodePtr SwitchParser::Parse(ParsingInfo& info) {
	const UInt startIndex = info.index;

	if (info.Current().type != TokenType::Switch) {
		return nullptr;
	}

	info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo(info.Current().line));

	const UInt switchLine = info.Current().line;
	info.index++;

	NodePtr value = ExpressionParser::Parse(info);

	if (!value) {
		ErrorLog::Error(LogMessage("error.syntax.expected.after", "match expression", "'switch'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
	}

	Pointer<SwitchNode> switchNode = new SwitchNode(info.scope, info.GetFileInfo(switchLine));
	switchNode->expr = false;
	switchNode->match = value;

	while (info.Current().type == TokenType::Case || info.Current().type == TokenType::Default) {
		info.scope = info.scope->Cast<ScopeSymbol>()->AddScope(info.GetFileInfo(info.Current().line));

		bool isDefault = info.Current().type == TokenType::Default;
		const UInt caseLine = info.Current().line;

		if (isDefault && switchNode->def) {
			ErrorLog::Error(LogMessage("error.syntax.switch.default.multiple.stat"), FileInfo(info.filename, info.Current().line, info.statementNumber));
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
				ErrorLog::Error(LogMessage("error.syntax.expected.after_in", "case expression", "'case'", "switch statement"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
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
			ErrorLog::Error(LogMessage("error.syntax.expected.after", "'then'", "case expression"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
		}

		NodePtr node;

		info.scopeCount++;

		if (hasThen) {
			node = StatementParser::ParseMultiple(info);
		}
		else {
			node = StatementParser::Parse(info);

			if (!node) {
				ErrorLog::Error(LogMessage("error.syntax.expected.after", "statement", LogMessage::Quote(info.Current(-1).value)), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
			}
		}

		info.scopeCount--;

		if (isDefault) {
			switchNode->def = node;
		}
		else {
			switchNode->nodes.Add(node);
			switchNode->cases.Add(caseValues);
		}

		if (hasThen) {
			if (info.Current().type == TokenType::End) {
				info.index++;
			}
			else {
				ErrorLog::Error(LogMessage("error.syntax.expected.end_at", "switch case", caseLine), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
			}
		}

		info.scope = info.scope->Parent<ScopeSymbol>();
	}

	if (info.Current().type == TokenType::End) {
		info.index++;
	}
	else {
		ErrorLog::Error(LogMessage("error.syntax.expected.end_at", "switch statement", switchLine), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
	}

	info.scope = info.scope->Parent<ScopeSymbol>();
	info.statementNumber++;
	return switchNode;
}
