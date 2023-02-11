#include "EmptyStatement.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Symbols;

EmptyStatement::EmptyStatement() : Statement(nullptr, FileInfo()) {

}

EmptyStatement::~EmptyStatement() {
	
}

Ptr<Kiwi::Value> EmptyStatement::Compile(CompileInfo& info) {
	return nullptr;
}

StringBuilder EmptyStatement::ToMelon(const UInt indent) const {
	if (statement) return statement->ToMelon(indent);
	return StringBuilder();
}