#include "ValueSymbol.h"

#include "TypeSymbol.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Symbols;

ValueSymbol::ValueSymbol(const FileInfo& file) : Symbol(file) {

}

ValueSymbol::~ValueSymbol() {

}

TypeSymbol* ValueSymbol::Type() {
	return parent->Cast<TypeSymbol>();
}
