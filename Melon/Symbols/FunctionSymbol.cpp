#include "FunctionSymbol.h"

#include "SymbolTable.h"
#include "VariableSymbol.h"
#include "TypeSymbol.h"

#include "Melon/Nodes/RootNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

FunctionSymbol::FunctionSymbol(const FileInfo& file) : ScopeSymbol(file) {
	
}

FunctionSymbol::~FunctionSymbol() {
	
}

TypeSymbol* FunctionSymbol::ReturnType(const UInt index) {
	if (index >= returnValues.Size()) return nullptr;
	return SymbolTable::Find<TypeSymbol>(returnValues[index], Parent()->AbsoluteName(), file);
}

TypeSymbol* FunctionSymbol::ArgumentType(const UInt index) {
	if (index >= arguments.Size()) return nullptr;
	
	if (symbolNode) {
		return SymbolTable::FindAbsolute<TypeSymbol>(arguments[index], file);
	}
	else if (Symbol* const arg = Symbol::Find(arguments[index], file)) {
		return arg->Type();
	}
	else {
		return nullptr;
	}
}

VariableSymbol* FunctionSymbol::Argument(const UInt index) {
	if (index >= arguments.Size()) return nullptr;

	if (symbolNode) {
		return nullptr;
	}
	else {
		return Symbol::Find<VariableSymbol>(arguments[index], file);
	}
}

TypeSymbol* FunctionSymbol::TemplateArgument(const UInt index) {
	if (index >= templateArguments.Size()) return nullptr;

	ScopeList arg = templateArguments[index];

	if (arg[0] == Scope("")) {
		return Symbol::Find<TypeSymbol>(arg[1], file);
	}
	else {
		return SymbolTable::FindAbsolute<TypeSymbol>(arg, file);
	}
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

Scope FunctionSymbol::Name() {
	if (!overloads.IsEmpty()) {
		return name;
	}
	else {
		Scope name = this->name;

		if (!templateArguments.IsEmpty()) {
			name.types = templateArguments;
		}
		
		if (symbolNode) {
			name.arguments = arguments;
			return name;
		}
		else {
			List<ScopeList> args;

			for (UInt i = 0; i < arguments.Size(); i++) {
				if (TypeSymbol* const type = ArgumentType(i)) {
					args.Add(type->AbsoluteName());
				}
				else {
					args.Add(ScopeList::undefined);
				}
			}

			name.arguments = args;
			return name;
		}
	}
}

FunctionSymbol* FunctionSymbol::SpecializeTemplate(const ReplacementMap<TypeSymbol*>& replacement, RootNode* const root) {
	FunctionSymbol* const sym = new FunctionSymbol(file);
	sym->arguments  = arguments.Copy();
	sym->attributes = attributes;
	sym->isExplicit = isExplicit;
	sym->name       = name;
	sym->node       = node;
	sym->symbolNode = symbolNode;

	if (overloads.IsEmpty()) {
		for (UInt i = 0; i < returnValues.Size(); i++) {
			sym->returnValues.Add(replacement.GetValue(ReturnType(i))->AbsoluteName());
		}

		for (UInt i = 0; i < templateArguments.Size(); i++) {
			sym->templateArguments.Add(replacement.GetValue(TemplateArgument(i))->AbsoluteName());
		}

		for (const Pair<Scope, Symbol*>& s : symbols) {
			sym->AddSymbol(s.key, s.value->SpecializeTemplate(replacement, root));
		}

		for (ScopeSymbol* const s : scopes) {
			sym->AddScope(s->SpecializeTemplate(replacement, root));
		}
	}
	else for (FunctionSymbol* const s : overloads) {
		sym->overloads.Add(s->SpecializeTemplate(replacement, root));
	}

	return sym;
}
