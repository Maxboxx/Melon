#include "Symbol.h"

#include "SymbolTable.h"
#include "TypeSymbol.h"
#include "TemplateSymbol.h"
#include "FunctionSymbol.h"

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

TypeSymbol* Symbol::ParentType() const {
	if (parent == nullptr) return nullptr;

	if (TypeSymbol* const type = parent->Cast<TypeSymbol>()) {
		return type;
	}

	return parent->ParentType();
}

TypeSymbol* Symbol::CurrentType() {
	if (TypeSymbol* const type = this->Cast<TypeSymbol>()) return type;
	return ParentType();
}

FunctionSymbol* Symbol::ParentFunction() const {
	if (parent == nullptr) return nullptr;

	if (FunctionSymbol* const func = parent->Cast<FunctionSymbol>()) {
		return func;
	}

	return parent->ParentFunction();
}

FunctionSymbol* Symbol::CurrentFunction() {
	if (FunctionSymbol* const type = this->Cast<FunctionSymbol>()) return type;
	return ParentFunction();
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
	return FindSymbol(scopeList.Split(), 0, file);
}

Symbol* Symbol::Contains(const Scope& scope) {
	return Contains(ScopeList().Add(scope));
}

Symbol* Symbol::Contains(const ScopeList& scopeList) {
	ErrorLog::AddMarker();
	Symbol* const symbol = FindSymbol(scopeList.Split(), 0, FileInfo());
	ErrorLog::Revert();

	return symbol;
}

Symbol* Symbol::FindSymbol(const ScopeList& scopeList, const UInt index, const FileInfo& file) {
	if (index >= scopeList.Size()) return this;
	FindError(scopeList, index, file);
	return nullptr;
}

void Symbol::FindError(const ScopeList& scopeList, const UInt index, const FileInfo& file) {
	ErrorLog::Error(SymbolError(SymbolError::NotFoundStart + scopeList[index].ToString() + SymbolError::NotFoundEnd, file));
}

FileInfo Symbol::File() const {
	return file;
}

TypeSymbol* Symbol::ReplaceType(TypeSymbol* const type, const ReplacementMap<TypeSymbol*>& replacement, const FileInfo& file) {
	return SymbolTable::FindAbsolute<TypeSymbol>(ReplaceTypeScope(type, replacement, file), file);
}

ScopeList Symbol::ReplaceTypeScope(TypeSymbol* const type, const ReplacementMap<TypeSymbol*>& replacement, const FileInfo& file) {
	Map<TemplateSymbol*, ScopeList> templateTypes;

	for (const Pair<TypeSymbol*, TypeSymbol*>& pair : replacement) {
		if (TemplateSymbol* const t = pair.key->Cast<TemplateSymbol>()) {
			templateTypes.Add(t, t->type);
			t->type = pair.value->AbsoluteName();
		}
	}

	ScopeList scope = SymbolTable::ReplaceTemplatesAbsolute(type->AbsoluteName(), file);

	for (const Pair<TemplateSymbol*, ScopeList>& pair : templateTypes) {
		pair.key->type = pair.value;
	}

	return scope;
}

bool Symbol::IsNotSpecialized() {
	return parent ? parent->IsNotSpecialized() : false;
}

void Symbol::SetTemplateValues(Symbol* const symbol) {
	if (parent) {
		if (symbol->parent) {
			parent->SetTemplateValues(symbol->parent);
		}
		else {
			// TODO: error
		}
	}
}
