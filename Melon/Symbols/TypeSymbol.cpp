#include "TypeSymbol.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Symbols;

TypeSymbol::TypeSymbol(const UInt size, const FileInfo& file) : MapSymbol(file) {
	this->size = size;
}

TypeSymbol::~TypeSymbol() {

}

UInt TypeSymbol::Size() const {
	return size;
}
