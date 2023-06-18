#include "ArrayParser.h"

#include "TypeParser.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Parsing;

Ptr<Nodes::Array> ArrayParser::Parse(ParsingInfo& info) {
	const UInt startIndex = info.index;

	Optional<Symbols::NameList> type = TypeParser::Parse(info);
	
	if (type) {
		if ((*type).Size() != 1 || (*type)[0].name != Symbols::Name::Array.name) {
			info.index = startIndex;
			return nullptr;
		}
	}

	if (info.Current().type != TokenType::CurlyOpen) {
		info.index = startIndex;
		return nullptr;
	}

	info.index++;

	if (info.Current().type != TokenType::CurlyClose) {
		info.index = startIndex;
		return nullptr;
	}

	info.index++;

	Ptr<Nodes::Array> node = new Nodes::Array(info.scope, info.GetFileInfoPrev());
	node->type = type;
	return node;
}