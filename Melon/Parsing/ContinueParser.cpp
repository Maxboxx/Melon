#include "ContinueParser.h"

#include "Melon/Nodes/ContinueNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Parsing;

NodePtr ContinueParser::Parse(ParsingInfo& info) {
	if (info.Current().type == TokenType::Continue) {
		info.index++;

		Pointer<ContinueNode> cn = new ContinueNode(info.scopes, FileInfo(info.filename, info.Current(-1).line));
		cn->loops = 1;

		if (info.Current().type == TokenType::Integer) {
			cn->loops = info.Current().value.ToInt();

			if (cn->loops == 0)
				ErrorLog::Error(SyntaxError(SyntaxError::ContinueIntLow, FileInfo(info.filename, info.Current().line)));
			if (cn->loops > info.loops)
				ErrorLog::Error(SyntaxError(SyntaxError::ContinueLoops, FileInfo(info.filename, info.Current().line)));

			info.index++;
		}

		return cn;
	}

	return nullptr;
}