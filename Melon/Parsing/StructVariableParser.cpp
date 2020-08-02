#include "StructVariableParser.h"

#include "NewVariableParser.h"

using namespace Melon::Nodes;
using namespace Melon::Parsing;

NodePtr StructVariableParser::Parse(ParsingInfo& info) {
	if (NodePtr node = NewVariableParser::Parse(info)) {
		return node;
	}

	return nullptr;
}