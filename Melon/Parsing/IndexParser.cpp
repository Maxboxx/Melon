#include "IndexParser.h"

#include "ExpressionParser.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Parsing;

Ptr<IndexExpression> IndexParser::Parse(ParsingInfo& info) {
	if (info.EndOfFile()) return nullptr;

	const UInt startIndex = info.index;

	// Parse argument list
	if (info.Current().type == TokenType::SquareOpen) {
		info.index++;

		Ptr<IndexExpression> index = new IndexExpression(info.scope, info.GetFileInfoPrev());

		// Parse arguments
		while (info.Current().type != TokenType::SquareClose) {
			if (!index->args.IsEmpty()) {
				if (info.Current().type != TokenType::Comma) {
					ErrorLog::Error(LogMessage("error.syntax.expected.after", LogMessage::Quote("]"), LogMessage::Quote(info.Prev().value)), info.GetFileInfoPrev());
					info.index = startIndex;
					return nullptr;
				}

				info.index++;
			}

			if (Ptr<Expression> node = ExpressionParser::Parse(info)) {
				index->args.Add(node);
			}
			else {
				ErrorLog::Error(LogMessage("error.syntax.expected.after", LogMessage::Quote("]"), LogMessage::Quote(info.Prev().value)), info.GetFileInfoPrev());
				info.index = startIndex;
				return nullptr;
			}
		}

		info.index++;

		return index;
	}

	info.index = startIndex;
	return nullptr;
}
