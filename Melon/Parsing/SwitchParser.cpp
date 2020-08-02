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

	info.scopes = info.scopes.AddNext("switch");
	Symbol::Add(info.scopes, Symbol(SymbolType::Scope), FileInfo(info.filename, info.Current().line));

	const UInt switchLine = info.Current().line;
	info.index++;

	NodePtr value = ExpressionParser::Parse(info);

	if (!value) {
		ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfterIn("match expression", "'switch'", "switch statement"), FileInfo(info.filename, info.Current(-1).line)));
	}

	Pointer<SwitchNode> switchNode = new SwitchNode(info.scopes, FileInfo(info.filename, switchLine));
	switchNode->expr = false;
	switchNode->match = value;

	while (info.Current().type == TokenType::Case || info.Current().type == TokenType::Default) {
		if (info.Current().type == TokenType::Case) {
			info.scopes = info.scopes.AddNext("case");
			Symbol::Add(info.scopes, Symbol(SymbolType::Scope), FileInfo(info.filename, info.Current().line));
		}
		else {
			info.scopes = info.scopes.AddNext("default");
			Symbol::Add(info.scopes, Symbol(SymbolType::Scope), FileInfo(info.filename, info.Current().line));
		}

		bool isDefault = info.Current().type == TokenType::Default;
		const UInt caseLine = info.Current().line;

		if (isDefault && switchNode->def) {
			ErrorLog::Error(SyntaxError(SyntaxError::MultipleDefault, FileInfo(info.filename, info.Current().line)));
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
				ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfterIn("case expression", "'case'", "switch statement"), FileInfo(info.filename, info.Current(-1).line)));
			}
		}

		bool hasThen = false;

		if (info.Current().type == TokenType::Then) {
			info.index++;
			hasThen = true;
		}

		NodePtr node;

		if (hasThen) {
			node = StatementParser::ParseMultiple(info);
		}
		else {
			node = StatementParser::Parse(info);

			if (!node) {
				ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfterIn("'then'", "case values", "switch statement"), FileInfo(info.filename, info.Current(-1).line)));
			}
		}

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
				ErrorLog::Error(SyntaxError(SyntaxError::EndExpected("switch case", caseLine), FileInfo(info.filename, info.Current(-1).line)));
			}
		}

		info.scopes = info.scopes.Pop();
	}

	if (info.Current().type == TokenType::End) {
		info.index++;
	}
	else {
		ErrorLog::Error(SyntaxError(SyntaxError::EndExpected("switch statement", switchLine), FileInfo(info.filename, info.Current(-1).line)));
	}

	info.scopes = info.scopes.Pop();
	return switchNode;
}