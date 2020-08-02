#include "TypeParser.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

Optional<ScopeList> TypeParser::Parse(ParsingInfo& info) {
	if (info.Current().type == TokenType::Name) {
		ScopeList type = ScopeList().Add(Scope(info.Current().value));
		info.index++;
		return type;
	}

	return nullptr;
}