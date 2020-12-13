#include "IntegerSymbol.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Symbols;

IntegerSymbol::IntegerSymbol(const UInt size, const bool isSigned, const FileInfo& file) : TypeSymbol(size, file) {
	this->isSigned = isSigned;
}

IntegerSymbol::~IntegerSymbol() {

}

bool IntegerSymbol::IsSigned() const {
	return isSigned;
}
