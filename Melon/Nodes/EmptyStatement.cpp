#include "EmptyStatement.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Symbols;

EmptyStatement::EmptyStatement() : Statement(nullptr, FileInfo()) {

}

EmptyStatement::~EmptyStatement() {
	
}

CompiledNode EmptyStatement::Compile(OldCompileInfo& info) {
	return CompiledNode();
}

StringBuilder EmptyStatement::ToMelon(const UInt indent) const {
	if (statement) return statement->ToMelon(indent);
	return StringBuilder();
}