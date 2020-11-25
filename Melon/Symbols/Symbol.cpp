#include "Symbol.h"

#include "SymbolTable.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Symbols;

Symbol::Symbol(const FileInfo& file) : file(file) {
	
}

Symbol::~Symbol() {

}

Symbol* Symbol::Parent() const {
	return parent;
}

Scope Symbol::Name() const {
	return Scope(name);
}

ScopeList Symbol::AbsoluteName() const {
	if (parent) {
		return parent->AbsoluteName().Add(Name());
	}
	else {
		return ScopeList(true).Add(Name());
	}
}

Symbol* Symbol::Type() {
	return this;
}

bool Symbol::IsType() const {
	return false;
}

UInt Symbol::Size() const {
	return 0;
}
