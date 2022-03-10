#include "ReturnParser.h"

#include "ExpressionParser.h"

#include "Melon/Symbols/FunctionSymbol.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

Ptr<ReturnStatement> ReturnParser::Parse(ParsingInfo& info) {
	const UInt startIndex = info.index;

	if (info.Current().type == TokenType::Return) {
		info.index++;

		Ptr<ReturnStatement> ret = new ReturnStatement(info.scope, info.GetFileInfoPrev());
		
		Symbol* sym = info.scope;

		while (sym && !sym->Is<FunctionSymbol>()) {
			sym = sym->Parent();
		}

		ret->func = sym->AbsoluteName();

		while (Ptr<Expression> node = ExpressionParser::Parse(info)) {
			ret->values.Add(node);

			if (info.Current().type == TokenType::Comma) {
				info.index++;
			}
			else {
				break;
			}
		}

		info.statementNumber++;
		return ret;
	}

	info.index = startIndex;
	return nullptr;
}