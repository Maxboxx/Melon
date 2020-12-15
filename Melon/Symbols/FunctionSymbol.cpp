#include "FunctionSymbol.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Symbols;

FunctionSymbol::FunctionSymbol(const FileInfo& file) : ScopeSymbol(file) {
	
}

FunctionSymbol::~FunctionSymbol() {
	
}

Symbol* FunctionSymbol::ReturnType(const UInt index) const {
	return nullptr;
}

Symbol* FunctionSymbol::Argument(const UInt index) const {
	return nullptr;
}

Symbol* FunctionSymbol::TemplateArgument(const UInt index) const {
	return nullptr;
}

FunctionSymbol* FunctionSymbol::AddOverload(FunctionSymbol* const overload) {
	overload->parent = this;
	overloads.Add(overload);
	return overload;
}

Symbol* FunctionSymbol::Find(const ScopeList& scopeList, const UInt index, const FileInfo& file) {
	if (index >= scopeList.Size()) return this;
	const Scope& scope = scopeList[index];

	if (scope.name.Size() == 0 && scope.arguments) {
		for (FunctionSymbol* const overload : overloads) {
			if (overload->Name() == scope) {
				return overload->Find(scopeList, index + 1, file);
			}
		}
	}
	else {
		return MapSymbol::Find(scopeList, index, file);
	}

	FindError(scopeList, index, file);
	return nullptr;
}