#include "MatchStatement.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;

MatchStatement::MatchStatement(Symbols::Symbol* const scope, const FileInfo& file) : MatchBaseNode<Statement, Statements>(scope, file) {
	
}

MatchStatement::~MatchStatement() {
	
}

bool MatchStatement::IsScope() const {
	return true;
}

Ptr<Statement> MatchStatement::Optimize(OptimizeInfo& info) {
	return OptimizeSwitch(info);
}
