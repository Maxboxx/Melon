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

template <class T>
T* Symbol::Parent() const {
	return parent->Cast<T>();
}

Scope Symbol::Name() const {
	return name;
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
	return nullptr;
}

bool Symbol::IsType() const {
	return false;
}

Symbol* Symbol::Find(const Scope& scope, const FileInfo& file) {
	return Find(ScopeList().Add(scope), file);
}

Symbol* Symbol::Find(const ScopeList& scopeList, const FileInfo& file) {
	return Find(scopeList, 0, file);
}

template <class T>
T* Symbol::Find(const Scope& scope, const FileInfo& file) {
	return Find(ScopeList().Add(scope), file)->Cast<T>();
}

template <class T>
T* Symbol::Find(const ScopeList& scopeList, const FileInfo& file) {
	return Find(scopeList, 0, file)->Cast<T>();
}

Symbol* Symbol::Contains(const Scope& scope) {
	return Contains(ScopeList().Add(scope));
}

Symbol* Symbol::Contains(const ScopeList& scopeList) {
	ErrorLog::AddMarker();
	Symbol* const symbol = Find(scopeList, 0, FileInfo());
	ErrorLog::Revert();

	return symbol;
}

template <class T>
T* Symbol::Contains(const Scope& scope) {
	return Contains(ScopeList().Add(scope))->Cast<T>();
}

template <class T>
T* Symbol::Contains(const ScopeList& scopeList) {
	ErrorLog::AddMarker();
	Symbol* const symbol = Find(scopeList, 0, FileInfo());
	ErrorLog::Revert();

	return symbol->Cast<T>();
}

Symbol* Symbol::Find(const ScopeList& scopeList, const UInt index, const FileInfo& file) {
	if (index >= scopeList.Size()) return this;
	FindError(scopeList, index, file);
	return nullptr;
}

void Symbol::FindError(const ScopeList& scopeList, const UInt index, const FileInfo& file) {
	ErrorLog::Error(SymbolError(SymbolError::NotFoundStart + scopeList[index].ToString() + SymbolError::NotFoundEnd, file));
}
