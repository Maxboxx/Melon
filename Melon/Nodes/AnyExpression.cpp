#include "AnyExpression.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

AnyExpression::AnyExpression(const FileInfo& file) : Expression(nullptr, file) {

}

AnyExpression::~AnyExpression() {
	delete inferred;
}

Symbol* AnyExpression::Symbol() const {
	return *inferred;
}

TypeSymbol* AnyExpression::Type(TypeSymbol* expected) const {
	if (expected) {
		*inferred = expected;
	}

	return *inferred;
}

Ptr<Kiwi::Value> AnyExpression::Compile(CompileInfo& info) {
	return nullptr;
}

StringBuilder AnyExpression::ToMelon(const UInt indent) const {
	if (*inferred) {
		return (*inferred)->AbsoluteName().ToSimpleString();
	}
	else {
		return String("any");
	}
}
