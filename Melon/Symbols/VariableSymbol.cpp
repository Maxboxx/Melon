#include "VariableSymbol.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Symbols;

VariableSymbol::VariableSymbol(const FileInfo& file) : Symbol(file) {

}

VariableSymbol::~VariableSymbol() {
	
}

TypeSymbol* VariableSymbol::Type() {
	return nullptr;
}
