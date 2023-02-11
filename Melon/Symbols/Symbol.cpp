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

Name Symbol::Name() {
	return name;
}

NameList Symbol::AbsoluteName() {
	if (parent) {
		return parent->AbsoluteName().Add(Name());
	}
	else {
		return NameList(true);
	}
}

String Symbol::KiwiName() {
	return AbsoluteName().ToString();
}

Kiwi::Type Symbol::KiwiType() {
	if (TypeSymbol* type = Type()) {
		return type->KiwiType();
	}

	return Kiwi::Type();
}

TypeSymbol* Symbol::Type() {
	return nullptr;
}

Symbol* Symbol::Find(const Symbols::Name& scope, const FileInfo& file) {
	return Find(NameList(scope), file);
}

Symbol* Symbol::Find(const NameList& scopeList, const FileInfo& file) {
	return FindSymbol(scopeList.Split(), 0, file);
}

Symbol* Symbol::Contains(const Symbols::Name& scope) {
	return Contains(NameList(scope));
}

Symbol* Symbol::Contains(const NameList& scopeList) {
	ErrorLog::AddMarker();
	Symbol* const symbol = FindSymbol(scopeList.Split(), 0, FileInfo());
	ErrorLog::Revert();

	return symbol;
}

Symbol* Symbol::FindSymbol(const NameList& scopeList, const UInt index, const FileInfo& file) {
	if (index >= scopeList.Size()) return this;
	FindError(scopeList, index, file);
	return nullptr;
}

void Symbol::FindError(const NameList& scopeList, const UInt index, const FileInfo& file) {
	ErrorLog::Error(LogMessage("error.symbol.not_found", scopeList[index].ToSimpleString()), file);
}

FileInfo Symbol::File() const {
	return file;
}

String Symbol::ToString() {
	return AbsoluteName().ToSimpleString();
}

TypeSymbol* Symbol::ReplaceType(TypeSymbol* const type, const ReplacementMap<TypeSymbol*>& replacement, const FileInfo& file) {
	return SymbolTable::FindAbsolute<TypeSymbol>(ReplaceTypeScope(type, replacement, file), file);
}

NameList Symbol::ReplaceTypeScope(TypeSymbol* const type, const ReplacementMap<TypeSymbol*>& replacement, const FileInfo& file) {
	Map<TemplateSymbol*, NameList> templateTypes;

	for (const Pair<TypeSymbol*, TypeSymbol*>& pair : replacement) {
		if (TemplateSymbol* const t = pair.key->Cast<TemplateSymbol>()) {
			templateTypes.Add(t, t->type);
			t->type = pair.value->AbsoluteName();
		}
	}

	NameList scope = SymbolTable::ReplaceTemplatesAbsolute(type->AbsoluteName(), file);

	for (const Pair<TemplateSymbol*, NameList>& pair : templateTypes) {
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
