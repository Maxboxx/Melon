#include "ValueSymbol.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Symbols;

ValueSymbol::ValueSymbol(const FileInfo& file) : Symbol(file) {

}

ValueSymbol::~ValueSymbol() {

}

Symbol* ValueSymbol::Type() {
	return parent;
}
