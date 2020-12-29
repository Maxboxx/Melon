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

Scope Symbol::Name() {
	return name;
}

ScopeList Symbol::AbsoluteName() {
	if (parent) {
		return parent->AbsoluteName().Add(Name());
	}
	else {
		return ScopeList(true);
	}
}

TypeSymbol* Symbol::Type() {
	return nullptr;
}

Symbol* Symbol::Find(const Scope& scope, const FileInfo& file) {
	return Find(ScopeList().Add(scope), file);
}

Symbol* Symbol::Find(const ScopeList& scopeList, const FileInfo& file) {
	return Find(scopeList.Split(), 0, file);
}

Symbol* Symbol::Contains(const Scope& scope) {
	return Contains(ScopeList().Add(scope));
}

Symbol* Symbol::Contains(const ScopeList& scopeList) {
	ErrorLog::AddMarker();
	Symbol* const symbol = Find(scopeList.Split(), 0, FileInfo());
	ErrorLog::Revert();

	return symbol;
}

Symbol* Symbol::Find(const ScopeList& scopeList, const UInt index, const FileInfo& file) {
	if (index >= scopeList.Size()) return this;
	FindError(scopeList, index, file);
	return nullptr;
}

void Symbol::FindError(const ScopeList& scopeList, const UInt index, const FileInfo& file) {
	ErrorLog::Error(SymbolError(SymbolError::NotFoundStart + scopeList[index].ToString() + SymbolError::NotFoundEnd, file));
}
