#include "FunctionSymbol.h"

#include "SymbolTable.h"
#include "VariableSymbol.h"
#include "TypeSymbol.h"
#include "TemplateSymbol.h"

#include "Melon/Nodes/RootNode.h"

#include "Boxx/ReplacementMap.h"

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
	return SymbolTable::Find<TypeSymbol>(returnValues[index], AbsoluteName(), file);
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

UInt FunctionSymbol::RequiredArguments() const {
	return arguments.Size();
}

UInt FunctionSymbol::RequiredTemplateArguments() const {
	return templateArguments.Size();
}

FunctionSymbol* FunctionSymbol::AddOverload(FunctionSymbol* const overload) {
	overload->parent = this;
	overload->name = Scope("");

	List<ScopeList> args;
	args.Add(ScopeList().Add(Scope(String::ToString(overloads.Size()))));
	overload->name.arguments = args;

	overloads.Add(overload);
	return overload;
}

Symbol* FunctionSymbol::FindSymbol(const ScopeList& scopeList, const UInt index, const FileInfo& file) {
	static const Regex numReg = Regex("^%d+$");

	if (index >= scopeList.Size()) return this;
	const Scope& scope = scopeList[index];

	if (scope.name.Size() == 0 && scope.arguments) {
		if (scope.arguments.Get().Size() == 1 && numReg.Match(scope.arguments.Get()[0][0].name)) {
			return overloads[scope.arguments.Get()[0][0].name.ToUInt()]->FindSymbol(scopeList, index + 1, file);
		}
		else for (FunctionSymbol* const overload : overloads) {
			if (overload->Name() == scope) {
				return overload->FindSymbol(scopeList, index + 1, file);
			}
		}
	}
	else {
		return MapSymbol::FindSymbol(scopeList, index, file);
	}

	FindError(scopeList, index, file);
	return nullptr;
}

FunctionSymbol* FunctionSymbol::FindOverload(const List<TypeSymbol*>& args, const FileInfo& file) {
	return nullptr;
}

FunctionSymbol* FunctionSymbol::FindOverload(const List<TypeSymbol*>& templateArgs, const List<TypeSymbol*>& args, const FileInfo& file) {
	return nullptr;
}

FunctionSymbol* FunctionSymbol::FindStaticOverload(const List<TypeSymbol*>& args, const FileInfo& file) {
	return nullptr;
}

FunctionSymbol* FunctionSymbol::FindStaticOverload(const List<TypeSymbol*>& templateArgs, const List<TypeSymbol*>& args, const FileInfo& file) {
	return nullptr;
}

FunctionSymbol* FunctionSymbol::FindMethodOverload(const List<TypeSymbol*>& args, const FileInfo& file) {
	return nullptr;
}

FunctionSymbol* FunctionSymbol::FindMethodOverload(const List<TypeSymbol*>& templateArgs, const List<TypeSymbol*>& args, const FileInfo& file) {
	return nullptr;
}

Tuple<List<TypeSymbol*>, List<TypeSymbol*>> FunctionSymbol::FindTemplateArguments(FunctionSymbol* const func, const List<TypeSymbol*>& templateArgs, const List<TypeSymbol*>& args, const FileInfo& file) {
	ReplacementMap<TypeSymbol*> templateMap;

	for (UInt i = 0; i < func->templateArguments.Size(); i++) {
		if (i < templateArgs.Size()) {
			templateMap.Add(func->TemplateArgument(i), templateArgs[i]);
		}
	}

	return {};
}

FunctionSymbol* FunctionSymbol::FindOverload(const List<FunctionSymbol*>& overloads, const List<TypeSymbol*>& templateArgs, const List<TypeSymbol*>& args, const FileInfo& file) {
	FunctionSymbol* best = nullptr;

	UInt bestNum    = 0;
	bool isTemplate = false;
	bool isAmbig    = false;

	for (FunctionSymbol* const overload : overloads) {
		bool perfect = true;
		bool match   = true;

		Tuple<List<TypeSymbol*>, List<TypeSymbol*>> specialized = FindTemplateArguments(overload, templateArgs, args, file);

		for (UInt i = 0; i < args.Size(); i++) {
			TypeSymbol* const arg = overload->ArgumentType(i);

			if (arg != args[i]) {
				perfect = false;

				if (!arg->ImplicitConversionFrom(args[i])) {
					match = false;
					break;
				}
			}
		}

		if (perfect) return overload;

		if (match) {
			if (best) {
				// TODO: error
				return nullptr;
			}

			best = overload;
		}
	}

	// TODO: error
	return nullptr;
}

FunctionSymbol* FunctionSymbol::FindOverload(const List<TypeSymbol*>& args, const bool isStatic, const FileInfo& file) {
	List<FunctionSymbol*> matches;

	for (FunctionSymbol* const overload : overloads) {
		if (((overload->attributes & FunctionAttributes::Static) != FunctionAttributes::None) != isStatic) continue;
		if (overload->RequiredArguments() > args.Size()) continue;
		if (overload->arguments.Size() < args.Size()) continue;

		bool match = true;
		UInt num = 0;

		for (UInt i = 0; i < args.Size(); i++) {
			TypeSymbol* const arg = overload->ArgumentType(i);

			if (arg != args[i]) {
				match = false;
				break;
			}
		}

		if (match) {
			matches.Add(overload);
		}
	}

	return FindOverload(matches, List<TypeSymbol*>(), args, file);
}

FunctionSymbol* FunctionSymbol::FindOverload(const List<TypeSymbol*>& templateArgs, const List<TypeSymbol*>& args, const bool isStatic, const FileInfo& file) {
	UInt bestNum = 0;
	bool ambig = false;

	List<FunctionSymbol*> matches;

	for (FunctionSymbol* const overload : overloads) {
		if (((overload->attributes & FunctionAttributes::Static) != FunctionAttributes::None) != isStatic) continue;
		if (overload->RequiredArguments() > args.Size()) continue;
		if (overload->arguments.Size() < args.Size()) continue;
		if (overload->RequiredTemplateArguments() > templateArgs.Size()) continue;
		if (overload->templateArguments.Size() < templateArgs.Size()) continue;

		bool match = true;
		UInt num = 0;

		for (UInt i = 0; i < templateArgs.Size(); i++) {
			TypeSymbol* const arg = overload->TemplateArgument(i);

			if (!arg->Is<TemplateSymbol>()) {
				if (arg != templateArgs[i]) {
					match = false;
					break;
				}
				else {
					num++;
				}
			}
		}

		if (match && num >= bestNum) {
			if (num > bestNum) {
				bestNum = num;
				matches.Clear();
			}

			matches.Add(overload);
		}
	}

	return FindOverload(matches, templateArgs, args, file);
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
