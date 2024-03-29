#include "BreakParser.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Parsing;

Ptr<BreakStatement> BreakParser::Parse(ParsingInfo& info) {
	if (info.Current().type == TokenType::Break) {
		return ParseBreak(info);
	}
	else if (info.Current().type == TokenType::Abort) {
		return ParseAbort(info);
	}

	return nullptr;
}

Ptr<BreakStatement> BreakParser::ParseBreak(ParsingInfo& info) {
	info.index++;

	Ptr<BreakStatement> bn = new BreakStatement(info.scope, info.GetFileInfoPrev());
	bn->isBreak = true;
	bn->loops = 1;
	bn->breakBool = false;

	if (info.Current().type == TokenType::Exclamation) {
		info.index++;
		bn->scopewise = true;

		if (info.Current().type == TokenType::Integer) {
			bn->loops = info.Current().value.ToInt();
			info.index++;
		}

		bn->loops++;

		if (bn->loops > info.scopeCount) {
			ErrorLog::Error(LogMessage("error.syntax.break.scopes"), info.GetFileInfoPrev());
		}
	}
	else {
		if (info.Current().type == TokenType::Boolean) {
			bn->breakBool = info.Current().value == "true" ? true : false;
			info.index++;

			if (info.Current().type == TokenType::Comma) {
				info.index++;

				if (info.Current().type == TokenType::Integer) {
					bn->loops = info.Current().value.ToInt();

					if (bn->loops == 0) {
						ErrorLog::Error(LogMessage("error.syntax.break.int_arg"), info.GetFileInfoPrev());
					}

					info.index++;
				}
				else {
					Parser::UnexpectedToken(info);
					return nullptr;
				}
			}
		}
		else if (info.Current().type == TokenType::Integer) {
			bn->loops = info.Current().value.ToInt();

			if (bn->loops == 0) {
				ErrorLog::Error(LogMessage("error.syntax.break.int_arg"), info.GetFileInfoPrev());
			}

			info.index++;

			if (info.Current().type == TokenType::Comma) {
				info.index++;

				if (info.Current().type == TokenType::Boolean) {
					bn->breakBool = info.Current().value == "true" ? true : false;
					info.index++;
				}
				else {
					Parser::UnexpectedToken(info);
					return nullptr;
				}
			}
		}

		if (bn->loops > info.loops) {
			ErrorLog::Error(LogMessage("error.syntax.break.loops"), info.GetFileInfoPrev());
		}
	}

	info.statementNumber++;
	return bn;
}

Ptr<BreakStatement> BreakParser::ParseAbort(ParsingInfo& info) {
	info.index++;

	Ptr<BreakStatement> bn = new BreakStatement(info.scope, info.GetFileInfoPrev());
	bn->isBreak = false;
	bn->loops = 1;

	if (info.Current().type == TokenType::Integer) {
		bn->loops = info.Current().value.ToInt();

		if (bn->loops == 0) {
			ErrorLog::Error(LogMessage("error.syntax.abort.int_arg"), info.GetFileInfo());
		}

		if (bn->loops > info.loops) {
			ErrorLog::Error(LogMessage("error.syntax.abort.loops"), info.GetFileInfo());
		}

		info.index++;
	}

	info.statementNumber++;
	return bn;
}
