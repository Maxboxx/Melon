#include "BreakParser.h"

#include "Melon/Nodes/BreakNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Parsing;

NodePtr BreakParser::Parse(ParsingInfo& info) {
	if (info.Current().type == TokenType::Break) {
		info.index++;

		Pointer<BreakNode> bn = new BreakNode(info.scopes, FileInfo(info.filename, info.Current(-1).line));
		bn->isBreak = true;
		bn->loops = 1;
		bn->breakBool = false;

		if (info.Current().type == TokenType::Boolean) {
			bn->breakBool = info.Current().value == "true" ? true : false;
			info.index++;

			if (info.Current().type == TokenType::Comma) {
				info.index++;

				if (info.Current().type == TokenType::Integer) {
					bn->loops = info.Current().value.ToInt();

					if (bn->loops == 0)
						ErrorLog::Error(SyntaxError(SyntaxError::BreakIntLow, FileInfo(info.filename, info.Current().line)));
					if (bn->loops > info.loops)
						ErrorLog::Error(SyntaxError(SyntaxError::BreakLoops, FileInfo(info.filename, info.Current().line)));

					info.index++;
				}
				else {
					return Parser::UnexpectedToken(info);
				}
			}
		}
		else if (info.Current().type == TokenType::Integer) {
			bn->loops = info.Current().value.ToInt();

			if (bn->loops == 0)
				ErrorLog::Error(SyntaxError(SyntaxError::BreakIntLow, FileInfo(info.filename, info.Current().line)));
			if (bn->loops > info.loops)
				ErrorLog::Error(SyntaxError(SyntaxError::BreakLoops, FileInfo(info.filename, info.Current().line)));

			info.index++;

			if (info.Current().type == TokenType::Comma) {
				info.index++;

				if (info.Current().type == TokenType::Boolean) {
					bn->breakBool = info.Current().value == "true" ? true : false;
					info.index++;
				}
				else {
					return Parser::UnexpectedToken(info);
				}
			}
		}

		return bn;
	}
	else if (info.Current().type == TokenType::Abort) {
		info.index++;

		Pointer<BreakNode> bn = new BreakNode(info.scopes, FileInfo(info.filename, info.Current(-1).line));
		bn->isBreak = false;
		bn->loops = 1;

		if (info.Current().type == TokenType::Integer) {
			bn->loops = info.Current().value.ToInt();

			if (bn->loops == 0)
				ErrorLog::Error(SyntaxError(SyntaxError::AbortIntLow, FileInfo(info.filename, info.Current().line)));
			if (bn->loops > info.loops)
				ErrorLog::Error(SyntaxError(SyntaxError::AbortLoops, FileInfo(info.filename, info.Current().line)));

			info.index++;
		}

		return bn;
	}

	return nullptr;
}