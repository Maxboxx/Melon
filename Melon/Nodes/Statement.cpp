#include "Statement.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

Statement::Statement(Symbol* const scope, const FileInfo& file) : Node(scope, file) {

}

Statement::~Statement() {

}

bool Statement::IsScope() const {
	return false;
}

Ptr<Statement> Statement::Optimize(OptimizeInfo& info) {
	return nullptr;
}

