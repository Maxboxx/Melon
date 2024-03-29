#include "FunctionSymbol.h"

#include "SymbolTable.h"
#include "VariableSymbol.h"
#include "TypeSymbol.h"
#include "TemplateSymbol.h"
#include "TemplateTypeSymbol.h"

#include "Melon/Nodes/RootNode.h"
#include "Melon/Nodes/FunctionBody.h"

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
	if (index >= returnValues.Count()) return nullptr;
	return SymbolTable::Find<TypeSymbol>(returnValues[index], AbsoluteName(), file, SymbolTable::SearchOptions::ReplaceTemplates);
}

TypeSymbol* FunctionSymbol::ArgumentType(const UInt index) {
	if (index >= arguments.Count()) return nullptr;
	
	if (symbolNode) {
		return SymbolTable::Find<TypeSymbol>(arguments[index], AbsoluteName(), file);
	}
	else if (Symbol* const arg = Symbol::Find(arguments[index], file)) {
		return arg->Type();
	}
	else {
		return nullptr;
	}
}

VariableSymbol* FunctionSymbol::Argument(const UInt index) {
	if (index >= arguments.Count()) return nullptr;

	if (symbolNode) {
		return nullptr;
	}
	else {
		return Symbol::Find<VariableSymbol>(arguments[index], file);
	}
}

TypeSymbol* FunctionSymbol::TemplateArgument(const UInt index) {
	if (index >= templateArguments.Count()) return nullptr;

	NameList arg = templateArguments[index];

	if (arg.IsTemplate()) {
		return Symbol::Find<TypeSymbol>(arg[1], file);
	}
	else {
		return SymbolTable::Find<TypeSymbol>(arg, Parent()->AbsoluteName(), file, SymbolTable::SearchOptions::ReplaceTemplates);
	}
}

UInt FunctionSymbol::RequiredArguments() const {
	return arguments.Count();
}

UInt FunctionSymbol::RequiredTemplateArguments() const {
	return templateArguments.Count();
}

FunctionSymbol* FunctionSymbol::AddOverload(FunctionSymbol* const overload) {
	overload->parent = this;
	overload->name = Symbols::Name();

	List<NameList> args;
	args.Add(NameList(Symbols::Name(String::ToString(overloads.Count()))));
	overload->name.arguments = args;

	overloads.Add(overload);
	return overload;
}

Symbol* FunctionSymbol::FindSymbol(const NameList& scopeList, const UInt index, const FileInfo& file) {
	static const Regex numReg = Regex("^%d+$");

	if (index >= scopeList.Size()) return this;
	const Symbols::Name& scope = scopeList[index];

	if (scope.name.IsEmpty() && scope.arguments) {
		if (scope.arguments->Count() == 1 && numReg.Match(scope.arguments.Value()[0][0].name)) {
			return overloads[scope.arguments.Value()[0][0].name.ToUInt()]->FindSymbol(scopeList, index + 1, file);
		}
		else for (FunctionSymbol* const overload : overloads) {
			if (overload->Name() == scope) {
				return overload->FindSymbol(scopeList, index + 1, file);
			}
		}
	}
	else {
		return ScopeSymbol::FindSymbol(scopeList, index, file);
	}

	FindError(scopeList, index, file);
	return nullptr;
}

FunctionSymbol* FunctionSymbol::FindOverload(const List<TypeSymbol*>& args, const FileInfo& file, FunctionAttributes attr) {
	return FindOverload(args, nullptr, file, attr);
}

FunctionSymbol* FunctionSymbol::FindOverload(const List<TypeSymbol*>& templateArgs, const List<TypeSymbol*>& args, const FileInfo& file, FunctionAttributes attr) {
	return FindOverload(templateArgs, args, nullptr, file, attr);
}

FunctionSymbol* FunctionSymbol::FindStaticOverload(const List<TypeSymbol*>& args, const FileInfo& file) {
	return FindOverload(args, true, file);
}

FunctionSymbol* FunctionSymbol::FindStaticOverload(const List<TypeSymbol*>& templateArgs, const List<TypeSymbol*>& args, const FileInfo& file) {
	return FindOverload(templateArgs, args, true, file);
}

FunctionSymbol* FunctionSymbol::FindMethodOverload(const List<TypeSymbol*>& args, const FileInfo& file, FunctionAttributes attr) {
	List<TypeSymbol*> argList = args.Copy();
	argList.Insert(0, Parent<TypeSymbol>());
	return FindOverload(argList, false, file, attr);
}

FunctionSymbol* FunctionSymbol::FindMethodOverload(const List<TypeSymbol*>& templateArgs, const List<TypeSymbol*>& args, const FileInfo& file, FunctionAttributes attr) {
	List<TypeSymbol*> argList = args.Copy();
	argList.Insert(0, Parent<TypeSymbol>());
	return FindOverload(templateArgs, argList, false, file, attr);
}

Tuple<List<TypeSymbol*>, List<NameList>> FunctionSymbol::FindTemplateArguments(FunctionSymbol* const func, const List<TypeSymbol*>& templateArgs, const List<TypeSymbol*>& args, const FileInfo& file) {
	Map<TemplateSymbol*, TypeSymbol*> templateMap;
	Map<TemplateSymbol*, NameList> templateTypes;

	for (UInt i = 0; i < func->templateArguments.Count(); i++) {
		TypeSymbol* const type = func->TemplateArgument(i);
		if (!type) return Tuple<List<TypeSymbol*>, List<NameList>>();

		if (TemplateSymbol* const templateType = type->Cast<TemplateSymbol>()) {
			templateTypes.Add(templateType, templateType->type);

			if (i < templateArgs.Count()) {
				templateMap.Add(templateType, templateArgs[i]);
			}
		}
	}

	for (UInt i = 0; i < func->arguments.Count(); i++) {
		if (i >= args.Count()) continue;

		if (TypeSymbol* const arg = func->ArgumentType(i)) {
			if (!templateTypes.IsEmpty()) {
				for (const Pair<TemplateSymbol*, TypeSymbol*>& pair : arg->DeduceTemplates(args[i])) {
					TypeSymbol* t = nullptr;

					if (templateMap.Contains(pair.key, t)) {
						if (!SymbolTable::FindImplicitConversion(pair.value, t, file)) {
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
			return Tuple<List<TypeSymbol*>, List<NameList>>();
		}
	}

	for (const Pair<TemplateSymbol*, TypeSymbol*>& pair : templateMap) {
		if (pair.value == nullptr) {
			return Tuple<List<TypeSymbol*>, List<NameList>>();
		}
	}

	Tuple<List<TypeSymbol*>, List<NameList>> types;

	for (UInt i = 0; i < func->templateArguments.Count(); i++) {
		TypeSymbol* const type = func->TemplateArgument(i);

		if (TemplateSymbol* const templateType = type->Cast<TemplateSymbol>()) {
			TypeSymbol* t = nullptr;
			
			if (templateMap.Contains(templateType, t)) {
				types.value1.Add(t);
			}
			else {
				return Tuple<List<TypeSymbol*>, List<NameList>>();
			}
		}
		else {
			types.value1.Add(type);
		}
	}

	for (const Pair<TemplateSymbol*, TypeSymbol*>& pair : templateMap) {
		pair.key->type = pair.value->AbsoluteName();
	}

	for (UInt i = 0; i < func->arguments.Count(); i++) {
		//types.value2.Add(func->ArgumentType(i)->AbsoluteName());
		TypeSymbol* type = func->ArgumentType(i);

		if (TemplateSymbol* t = type->Cast<TemplateSymbol>()) {
			templateMap.Contains(t, type);
		}

		types.value2.Add(type->AbsoluteName());
	}

	for (const Pair<TemplateSymbol*, NameList>& pair : templateTypes) {
		pair.key->type = pair.value;
	}

	return types;
}

FunctionSymbol* FunctionSymbol::FindOverload(const List<FunctionSymbol*>& overloads, const List<TypeSymbol*>& templateArgs, const List<TypeSymbol*>& args, const FileInfo& file, FunctionAttributes attr) {
	FunctionSymbol* best = nullptr;

	UInt bestNum    = Math::UIntMax();
	bool isTemplate = true;
	bool isAmbig    = false;

	ReplacementMap<TypeSymbol*> replacement;

	for (FunctionSymbol* const overload : overloads) {
		if (overload->attributes != attr) continue;

		bool perfect = true;
		bool match   = true;

		Tuple<List<TypeSymbol*>, List<NameList>> specialized = FindTemplateArguments(overload, templateArgs, args, file);
		
		if (!overload->templateArguments.IsEmpty()) {
			if (specialized.value1.IsEmpty()) continue;
		}

		for (UInt i = 0; i < args.Count(); i++) {
			TypeSymbol* const arg = SymbolTable::FindAbsolute<TypeSymbol>(specialized.value2[i], file);

			if (!arg) {
				match = false;
				perfect = false;
				break;
			}

			if (arg != args[i]) {
				perfect = false;

				if (!SymbolTable::FindImplicitConversion(args[i], arg, file, false)) {
					match = false;
					break;
				}
			}
		}

		if (match) {
			if (overload->templateArguments.IsEmpty()) {
				if (perfect) return overload;

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

				for (UInt i = 0; i < overload->templateArguments.Count(); i++) {
					TypeSymbol* const type = overload->TemplateArgument(i);
					
					if (type->Is<TemplateSymbol>()) {
						num++;
					}
				}

				if (perfect && num == 0) return overload;

				if (num < bestNum) {
					best = overload;
					bestNum = num;
					isAmbig = false;

					for (UInt i = 0; i < overload->templateArguments.Count(); i++) {
						TypeSymbol* const type = overload->TemplateArgument(i);

						if (type->Is<TemplateSymbol>()) {
							replacement.Add(type, specialized.value1[i]);
						}
					}
				}
				else if (num == bestNum) {
					isAmbig = true;
				}
			}
		}
	}

	if (!best) {
		// TODO: error
		ErrorLog::Error(LogMessage::Message("Overload not found (temporary error message)"), file);
		return nullptr;
	}
	else if (isAmbig) {
		// TODO: error
		ErrorLog::Error(LogMessage::Message("Overload is ambiguous (temporary error message)"), file);
		return nullptr;
	}
	else if (!isTemplate || bestNum == 0) {
		return best;
	}
	else {
		FunctionSymbol* newSym = best->InitializeSpecialize();
		best->Parent<FunctionSymbol>()->AddOverload(newSym);
		best->SpecializeTemplate(newSym, replacement, Node::Root());
		return newSym;
	}
}

FunctionSymbol* FunctionSymbol::FindOverload(const List<TypeSymbol*>& args, const Boxx::Optional<bool>& isStatic, const FileInfo& file, FunctionAttributes attr) {
	List<FunctionSymbol*> matches;

	for (FunctionSymbol* const overload : overloads) {
		if (overload->attributes != attr) continue;
		if (isStatic && ((overload->modifiers & FunctionModifiers::Static) != FunctionModifiers::None) != isStatic) continue;
		if (overload->RequiredArguments() > args.Count()) continue;
		if (overload->arguments.Count() < args.Count()) continue;

		bool match = true;

		for (UInt i = 0; i < args.Count(); i++) {
			TypeSymbol* const arg = overload->ArgumentType(i);
			if (!arg) continue;

			if (arg->Is<TemplateSymbol>()) continue;

			if (TemplateTypeSymbol* const template1 = arg->Cast<TemplateTypeSymbol>()) {
				if (TemplateTypeSymbol* const template2 = args[i]->Cast<TemplateTypeSymbol>()) {
					if (template1->CanBeDeduced(template2)) continue;
				}
			}

			if (args[i] != arg && !SymbolTable::FindImplicitConversion(args[i], arg, file, false)) {
				match = false;
				break;
			}
		}

		if (match) {
			matches.Add(overload);
		}
	}

	return FindOverload(matches, List<TypeSymbol*>(), args, file, attr);
}

FunctionSymbol* FunctionSymbol::FindOverload(const List<TypeSymbol*>& templateArgs, const List<TypeSymbol*>& args, const Boxx::Optional<bool>& isStatic, const FileInfo& file, FunctionAttributes attr) {
	UInt bestNum = 0;
	bool ambig = false;

	List<FunctionSymbol*> matches;

	for (FunctionSymbol* const overload : overloads) {
		if (isStatic && ((overload->modifiers & FunctionModifiers::Static) != FunctionModifiers::None) != isStatic) continue;
		if (overload->RequiredArguments() > args.Count()) continue;
		if (overload->arguments.Count() < args.Count()) continue;
		if (overload->RequiredTemplateArguments() > templateArgs.Count()) continue;
		if (overload->templateArguments.Count() < templateArgs.Count()) continue;

		bool match = true;
		UInt num = 0;

		for (UInt i = 0; i < templateArgs.Count(); i++) {
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

bool FunctionSymbol::IsNotSpecialized() {
	for (UInt i = 0; i < templateArguments.Count(); i++) {
		if (TypeSymbol* const type = TemplateArgument(i)) {
			if (type->Is<TemplateSymbol>()) {
				return true;
			}
		}
	}

	return Symbol::IsNotSpecialized();
}

void FunctionSymbol::SetTemplateValues(Symbol* const symbol) {
	FunctionSymbol* const sym = symbol->Cast<FunctionSymbol>();
	replace = sym;

	if (!sym) {
		// TODO: error?
		return;
	}

	if (templateArguments.Count() != sym->templateArguments.Count()) {
		// TODO: error?
		return;
	}

	for (UInt i = 0; i < templateArguments.Count(); i++) {
		TypeSymbol* const type1 = TemplateArgument(i);
		TypeSymbol* const type2 = sym->TemplateArgument(i);

		if (type1 && type2) {
			if (TemplateSymbol* const t = type1->Cast<TemplateSymbol>()) {
				t->type = type2->AbsoluteName();
			}
		}
	}

	Symbol::SetTemplateValues(symbol);
}

FunctionSymbol* FunctionSymbol::InitializeSpecialize() {
	FunctionSymbol* const sym = new FunctionSymbol(file);
	sym->arguments  = arguments.Copy();
	sym->modifiers  = modifiers;
	sym->isExplicit = isExplicit;
	sym->name       = name;
	sym->symbolNode = symbolNode;
	sym->attributes = attributes;
	return sym;
}

void FunctionSymbol::SpecializeTemplate(Symbol* initSym, const ReplacementMap<TypeSymbol*>& replacement, RootNode* const root) {
	FunctionSymbol* const sym = initSym->Cast<FunctionSymbol>();

	if (overloads.IsEmpty()) {
		if (node) {
			Ptr<FunctionBody> fn = new FunctionBody(node->scope, node->file);
			fn->sym = sym;

			// TODO: Copy instead of reference?
			fn->statements = new Statements(node->statements->scope, node->statements->File());
			fn->statements->statements = node->statements->statements;

			sym->node = fn;
			root->funcs.Add(fn);
		}

		for (UInt i = 0; i < returnValues.Count(); i++) {
			sym->returnValues.Add(ReplaceTypeScope(ReturnType(i), replacement, file));
		}

		for (UInt i = 0; i < templateArguments.Count(); i++) {
			TypeSymbol* const type = TemplateArgument(i);

			if (templateArguments[i].IsTemplate() && type == replacement.GetValue(type)) {
				sym->templateArguments.Add(templateArguments[i]);
			}
			else {
				sym->templateArguments.Add(ReplaceTypeScope(type, replacement, file));
			}
		}

		if (symbolNode) {
			for (UInt i = 0; i < arguments.Count(); i++) {
				sym->arguments[i] = ReplaceTypeScope(ArgumentType(i), replacement, file);
			}
		}

		for (const Pair<Symbols::Name, Symbol*>& s : symbols) {
			Symbol* newSym = s.value->InitializeSpecialize();
			sym->AddSymbol(s.key, newSym);
			s.value->SpecializeTemplate(newSym, replacement, root);
		}

		for (ScopeSymbol* const s : scopes) {
			ScopeSymbol* newSym = s->InitializeSpecialize();
			sym->AddScope(newSym);
			s->SpecializeTemplate(newSym, replacement, root);
		}
	}
	else for (FunctionSymbol* const s : overloads) {
		FunctionSymbol* newSym = s->InitializeSpecialize();
		sym->AddOverload(newSym);
		s->SpecializeTemplate(newSym, replacement, root);
	}
}
