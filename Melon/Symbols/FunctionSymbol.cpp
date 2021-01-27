#include "FunctionSymbol.h"

#include "SymbolTable.h"
#include "VariableSymbol.h"
#include "TypeSymbol.h"
#include "TemplateSymbol.h"
#include "TemplateTypeSymbol.h"

#include "Melon/Nodes/RootNode.h"
#include "Melon/Nodes/FunctionNode.h"

#include "Boxx/ReplacementMap.h"
#include "Boxx/Map.h"

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
	return SymbolTable::Find<TypeSymbol>(returnValues[index], AbsoluteName(), file, SymbolTable::SearchOptions::ReplaceTemplates);
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
	return FindOverload(args, nullptr, file);
}

FunctionSymbol* FunctionSymbol::FindOverload(const List<TypeSymbol*>& templateArgs, const List<TypeSymbol*>& args, const FileInfo& file) {
	return FindOverload(templateArgs, args, nullptr, file);
}

FunctionSymbol* FunctionSymbol::FindStaticOverload(const List<TypeSymbol*>& args, const FileInfo& file) {
	return FindOverload(args, true, file);
}

FunctionSymbol* FunctionSymbol::FindStaticOverload(const List<TypeSymbol*>& templateArgs, const List<TypeSymbol*>& args, const FileInfo& file) {
	return FindOverload(templateArgs, args, true, file);
}

FunctionSymbol* FunctionSymbol::FindMethodOverload(const List<TypeSymbol*>& args, const FileInfo& file) {
	List<TypeSymbol*> argList = args.Copy();
	argList.Insert(0, Parent<TypeSymbol>());
	return FindOverload(argList, false, file);
}

FunctionSymbol* FunctionSymbol::FindMethodOverload(const List<TypeSymbol*>& templateArgs, const List<TypeSymbol*>& args, const FileInfo& file) {
	List<TypeSymbol*> argList = args.Copy();
	argList.Insert(0, Parent<TypeSymbol>());
	return FindOverload(templateArgs, argList, false, file);
}

Tuple<List<TypeSymbol*>, List<ScopeList>> FunctionSymbol::FindTemplateArguments(FunctionSymbol* const func, const List<TypeSymbol*>& templateArgs, const List<TypeSymbol*>& args, const FileInfo& file) {
	Map<TemplateSymbol*, TypeSymbol*> templateMap;
	Map<TemplateSymbol*, ScopeList> templateTypes;

	for (UInt i = 0; i < func->templateArguments.Size(); i++) {
		TypeSymbol* const type = func->TemplateArgument(i);
		if (!type) return Tuple<List<TypeSymbol*>, List<ScopeList>>();

		if (TemplateSymbol* const templateType = type->Cast<TemplateSymbol>()) {
			templateTypes.Add(templateType, templateType->type);

			if (i < templateArgs.Size()) {
				templateMap.Add(templateType, templateArgs[i]);
			}
		}
	}

	for (UInt i = 0; i < func->arguments.Size(); i++) {
		if (i >= args.Size()) continue;

		if (TypeSymbol* const arg = func->ArgumentType(i)) {
			if (!templateTypes.IsEmpty()) {
				for (const Pair<TemplateSymbol*, TypeSymbol*>& pair : arg->DeduceTemplates(args[i])) {
					TypeSymbol* t = nullptr;

					if (templateMap.Contains(pair.key, t)) {
						if (t != pair.value) {
							templateMap[pair.key] = nullptr;
						}
					}
					else if (templateTypes.Contains(pair.key)) {
						templateMap.Add(pair);
					}
				}
			}
		}
		else {
			return Tuple<List<TypeSymbol*>, List<ScopeList>>();
		}
	}

	for (const Pair<TemplateSymbol*, TypeSymbol*>& pair : templateMap) {
		if (pair.value == nullptr) {
			return Tuple<List<TypeSymbol*>, List<ScopeList>>();
		}
	}

	Tuple<List<TypeSymbol*>, List<ScopeList>> types;

	for (UInt i = 0; i < func->templateArguments.Size(); i++) {
		TypeSymbol* const type = func->TemplateArgument(i);

		if (TemplateSymbol* const templateType = type->Cast<TemplateSymbol>()) {
			TypeSymbol* t = nullptr;
			
			if (templateMap.Contains(templateType, t)) {
				types.value1.Add(t);
			}
			else {
				return Tuple<List<TypeSymbol*>, List<ScopeList>>();
			}
		}
		else {
			types.value1.Add(type);
		}
	}

	for (const Pair<TemplateSymbol*, TypeSymbol*>& pair : templateMap) {
		pair.key->type = pair.value->AbsoluteName();
	}

	for (UInt i = 0; i < func->arguments.Size(); i++) {
		types.value2.Add(func->ArgumentType(i)->AbsoluteName());
	}

	for (const Pair<TemplateSymbol*, ScopeList>& pair : templateTypes) {
		pair.key->type = pair.value;
	}

	return types;
}

FunctionSymbol* FunctionSymbol::FindOverload(const List<FunctionSymbol*>& overloads, const List<TypeSymbol*>& templateArgs, const List<TypeSymbol*>& args, const FileInfo& file) {
	FunctionSymbol* best = nullptr;

	UInt bestNum    = 0;
	bool isTemplate = true;
	bool isAmbig    = false;

	for (FunctionSymbol* const overload : overloads) {
		bool perfect = true;
		bool match   = true;

		Tuple<List<TypeSymbol*>, List<ScopeList>> specialized = FindTemplateArguments(overload, templateArgs, args, file);
		
		if (!overload->templateArguments.IsEmpty()) {
			if (specialized.value1.IsEmpty()) continue;
		}

		for (UInt i = 0; i < args.Size(); i++) {
			TypeSymbol* const arg = SymbolTable::FindAbsolute<TypeSymbol>(specialized.value2[i], file);

			if (!arg) {
				match = false;
				perfect = false;
				break;
			}

			if (arg != args[i]) {
				perfect = false;

				if (!SymbolTable::FindImplicitConversion(arg, args[i], file)) {
					match = false;
					break;
				}
			}
		}

		if (perfect) return overload;

		if (match) {
			if (overload->templateArguments.IsEmpty()) {
				if (isTemplate) {
					best = overload;
					isTemplate = false;
					isAmbig = false;
				}
				else {
					isAmbig = true;
				}
			}
			else if (isTemplate) {
				UInt num = 0;

				for (UInt i = 0; i < templateArgs.Size(); i++) {
					TypeSymbol* const type = overload->TemplateArgument(i);
					
					if (type->Is<TemplateSymbol>()) {
						num++;
					}
				}

				if (num < bestNum) {
					best = overload;
					bestNum = num;
					isAmbig = false;
				}
				else if (num == bestNum) {
					isAmbig = true;
				}
			}
		}
	}

	if (!best) {
		// TODO: error
		return nullptr;
	}
	else if (isAmbig) {
		// TODO: error
		return nullptr;
	}
	else {
		return best;
	}
}

FunctionSymbol* FunctionSymbol::FindOverload(const List<TypeSymbol*>& args, const Boxx::Optional<bool>& isStatic, const FileInfo& file) {
	List<FunctionSymbol*> matches;

	for (FunctionSymbol* const overload : overloads) {
		if (isStatic.HasValue() && ((overload->attributes & FunctionAttributes::Static) != FunctionAttributes::None) != isStatic.Get()) continue;
		if (overload->RequiredArguments() > args.Size()) continue;
		if (overload->arguments.Size() < args.Size()) continue;

		bool match = true;

		for (UInt i = 0; i < args.Size(); i++) {
			TypeSymbol* const arg = overload->ArgumentType(i);
			if (!arg) continue;

			if (arg->Is<TemplateSymbol>()) continue;

			if (TemplateTypeSymbol* const template1 = arg->Cast<TemplateTypeSymbol>()) {
				if (TemplateTypeSymbol* const template2 = args[i]->Cast<TemplateTypeSymbol>()) {
					if (template1->CanBeDeduced(template2)) continue;
				}
			}

			if (!SymbolTable::FindImplicitConversion(arg, args[i], file)) {
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

FunctionSymbol* FunctionSymbol::FindOverload(const List<TypeSymbol*>& templateArgs, const List<TypeSymbol*>& args, const Boxx::Optional<bool>& isStatic, const FileInfo& file) {
	UInt bestNum = 0;
	bool ambig = false;

	List<FunctionSymbol*> matches;

	for (FunctionSymbol* const overload : overloads) {
		if (isStatic.HasValue() && ((overload->attributes & FunctionAttributes::Static) != FunctionAttributes::None) != isStatic.Get()) continue;
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
	sym->symbolNode = symbolNode;

	if (overloads.IsEmpty()) {
		if (node) {
			Pointer<FunctionNode> fn = new FunctionNode(node->scope, node->file);
			fn->sym = sym;
			fn->node = node.Cast<FunctionNode>()->node;
			sym->node = fn;
			root->funcs.Add(sym->node);
		}

		for (UInt i = 0; i < returnValues.Size(); i++) {
			sym->returnValues.Add(ReplaceTypeScope(ReturnType(i), replacement, file));
		}

		for (UInt i = 0; i < templateArguments.Size(); i++) {
			TypeSymbol* const type = TemplateArgument(i);

			if (templateArguments[i].IsTemplate() && type == replacement.GetValue(type)) {
				sym->templateArguments.Add(templateArguments[i]);
			}
			else {
				sym->templateArguments.Add(ReplaceTypeScope(type, replacement, file));
			}
		}

		for (const Pair<Scope, Symbol*>& s : symbols) {
			sym->AddSymbol(s.key, s.value->SpecializeTemplate(replacement, root));
		}

		for (ScopeSymbol* const s : scopes) {
			sym->AddScope(s->SpecializeTemplate(replacement, root));
		}
	}
	else for (FunctionSymbol* const s : overloads) {
		sym->AddOverload(s->SpecializeTemplate(replacement, root));
	}

	return sym;
}
