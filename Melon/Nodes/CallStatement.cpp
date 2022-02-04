#include "CallStatement.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

CallStatement::CallStatement(Symbols::Symbol* const scope, const FileInfo& file) : BaseCallNode<Statement>(scope, file) {
	isStatement = true;
}

CallStatement::~CallStatement() {

}

