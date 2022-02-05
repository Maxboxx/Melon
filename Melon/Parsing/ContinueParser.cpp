#include "ContinueParser.h"

#include "Melon/Nodes/ContinueNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Parsing;

NodePtr ContinueParser::Parse(ParsingInfo& info) {
	if (info.Current().type == TokenType::Continue) {
		info.index++;

		Pointer<ContinueStatement> cn = new ContinueStatement(info.scope, info.GetFileInfoPrev());
		cn->loops = 1;

		if (info.Current().type == TokenType::Integer) {
			cn->loops = info.Current().value.ToInt();

			if (cn->loops == 0) {
				ErrorLog::Error(LogMessage("error.syntax.continue.int_arg"), info.GetFileInfo());
			}

			if (cn->loops > info.loops) {
				ErrorLog::Error(LogMessage("error.syntax.continue.loops"), info.GetFileInfo());
			}

			info.index++;
		}

		info.statementNumber++;
		return cn;
	}

	return nullptr;
}