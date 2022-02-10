#include "SwitchStatement.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;

SwitchStatement::SwitchStatement(Symbols::Symbol* const scope, const FileInfo& file) : SwitchBaseNode<Statement, Statements>(scope, file) {
	
}

SwitchStatement::~SwitchStatement() {
	
}

bool SwitchStatement::IsScope() const {
	return true;
}

Ptr<Statement> SwitchStatement::Optimize(OptimizeInfo& info) {
	return OptimizeSwitch(info);
}