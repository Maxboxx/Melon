#include "Symbols.h"

#include "Boxx/Map.h"
#include "Boxx/Optional.h"

#include "Kiwi/Kiwi.h"

#include "Nodes/SymbolNode.h"
#include "Nodes/IntegerAssignNode.h"
#include "Nodes/IntegerUnaryOperatorNode.h"
#include "Nodes/IntegerBinaryOperatorNode.h"
#include "Nodes/IntegerConvertNode.h"
#include "Nodes/IntegerToBoolNode.h"
#include "Nodes/IntegerNotNode.h"

#include "Nodes/BooleanAssignNode.h"
#include "Nodes/BooleanBinaryOperatorNode.h"
#include "Nodes/BooleanUnaryOperatorNode.h"
#include "Nodes/BooleanToBooleanNode.h"
#include "Nodes/BooleanCompareNode.h"
#include "Nodes/BooleanNotNode.h"
#include "Nodes/BooleanConstantNode.h"

#include "Nodes/OptionalAssignNode.h"
#include "Nodes/OptionalAssignValueNode.h"
#include "Nodes/OptionalUnwrapNode.h"
#include "Nodes/OptionalToBooleanNode.h"
#include "Nodes/OptionalNotNode.h"

#include "ScopeList.h"

#include "Melon/Parsing/Parser.h"
#include "Melon/Nodes/FunctionNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

Symbol Symbol::symbols = Symbol(SymbolType::Namespace);
Symbol Symbol::empty = Symbol(SymbolType::None);

List<Symbol::TemplateSymbol> Symbol::templateSymbols;

Symbol::Symbol() {
	this->type = SymbolType::None;
}

Symbol::Symbol(const SymbolType type) {
	this->type = type;
}

Symbol::~Symbol() {

}

bool Symbol::Add(const Scope& scope, const Symbol& symbol, const FileInfo& file, const bool redefine) {
	try {
		if (symbol.type == SymbolType::Function || symbol.type == SymbolType::Method) {
			if (!scopes.Contains(scope.name)) {
				Symbol f;
				f.type = SymbolType::Function;
				f.scope = this->scope.Add(scope);
				f.scope.absolute = true;
				scopes.Add(scope.name, f);
			}
			else if (scopes[scope.name].variants.Size() == 1) {
				if (scopes[scope.name].variants[0].type == SymbolType::Scope) {
					scopes[scope.name].variants.RemoveAt(0);
				}
			}

			for (const Symbol& s : scopes[scope.name].variants) {
				if (s.arguments.Size() != symbol.arguments.Size()) continue;
				if (s.type != symbol.type) continue;

				bool same = true;

				for (UInt i = 0; i < s.arguments.Size(); i++) {
					if (s.arguments[i] != symbol.arguments[i]) {
						same = false;
						break;
					}
				}

				if (scope.name == Scope::As.name) {
					for (UInt i = 0; i < s.returnValues.Size(); i++) {
						if (s.returnValues[i] != symbol.returnValues[i]) {
							same = false;
							break;
						}
					}
				}

				if (!redefine && same) {
					ErrorLog::Error(SymbolError(SymbolError::RedefinitionStart + s.scope.ToString() + SymbolError::RedefinitionEnd, file));
				}
			}

			if (!redefine) {
				Symbol sym = symbol;
				Scope symScope = scope;
				symScope.variant = scopes[scope.name].variants.Size();
				sym.scope = this->scope.Add(symScope);
				sym.scope.absolute = true;
				scopes[scope.name].variants.Add(sym);
			}
			else {
				Symbol sym = symbol;
				Scope symScope = scope;
				sym.scope = this->scope.Add(scope);
				sym.scope.absolute = true;
				scopes[scope.name].variants[(UInt)scope.variant] = sym;
			}
		}
		else if (!symbol.templateArgs.IsEmpty()) {
			Symbol parent;

			if (!scopes.Contains(scope.name, parent)) {
				parent = Symbol(SymbolType::Scope);
				parent.scope = this->scope.Add(Scope(scope.name));
				parent.scope.absolute = true;
				scopes.Add(scope.name, parent);
			}

			if (redefine) {
				Symbol sym = symbol;
				sym.scope = sym.scope.Pop().Add(scope);
				sym.scope.absolute = true;
				parent.templateVariants[(UInt)scope.variant] = sym;
			}
			else {
				Scope newScope = scope.Copy();
				newScope.variant = parent.templateVariants.Size();

				Symbol sym = symbol;
				sym.scope = sym.scope.Pop().Add(newScope);
				sym.scope.absolute = true;
				parent.templateVariants.Add(sym);
			}
		}
		else {
			if (!redefine && scopes.Contains(scope.name)) {
				ErrorLog::Error(SymbolError(SymbolError::RedefinitionStart + scope.ToString() + SymbolError::RedefinitionEnd, file));
			}

			Symbol sym = symbol;
			sym.scope = this->scope.Add(scope);
			sym.scope.absolute = true;
			scopes.Add(scope.name, sym);
		}
	}
	catch (MapError e) {
		return false;
	}

	return true;
}

bool Symbol::Add(const ScopeList& scopes, const Symbol& symbol, const FileInfo& file, const bool redefine) {
	try {
		Symbol s = symbols;

		for (UInt i = 0; i < scopes.Size() - 1; i++) {
			s = s.Get(scopes[i], file);
		}

		return s.Add(scopes.Last(), symbol, file, redefine);
	}
	catch (MapError e) {
		return false;
	}

	return true;
}

Symbol& Symbol::GetTemplate(const Scope& scope, const FileInfo& file) {
	if (scope.types) {
		List<ScopeList> templateArgs = scope.types.Get();

		bool found = false;

		for (Symbol& sym : templateVariants) {
			if (sym.templateArgs.Size() == templateArgs.Size()) {
				bool match = true;

				for (UInt i = 0; i < templateArgs.Size(); i++) {
					if (sym.templateArgs[i] != templateArgs[i]) {
						match = false;
						break;
					}
				}

				if (match) {
					return sym;
				}
			}
		}

		if (!found) {
			return empty;
		}
	}

	return *this;
}

Tuple<Symbol, List<ScopeList>> Symbol::FindTemplateArgs(const TemplateSymbol& symbol) {
	List<ScopeList> templateArgs = symbol.type.Last().types.Get().Copy();

	for (ScopeList& type : templateArgs) {
		type = Symbol::FindNearestInNamespace(symbol.scope, type, symbol.file).scope;
	}

	Scope last = symbol.type.Last();
	last.types = nullptr;

	Symbol sym = Symbol::FindNearestInNamespace(symbol.scope, symbol.type.Pop().Add(last), symbol.file);

	bool found = false;

	for (const Symbol& variant : sym.templateVariants) {
		if (variant.templateArgs.Size() == templateArgs.Size()) {
			bool match = true;

			for (UInt i = 0; i < templateArgs.Size(); i++) {
				FileInfo info = variant.GetFileInfo();
				info.statementNumber++;
				Symbol symArg = Symbol::FindNearestInNamespace(variant.scope, variant.templateArgs[i], info);

				if (symArg.type == SymbolType::Template) continue;
				if (symArg.scope == templateArgs[i]) continue;

				match = false;
				break;
			}

			if (match) {
				return Tuple<Symbol, List<ScopeList>>(variant, templateArgs);
			}
		}
	}

	ErrorLog::Error(SymbolError("template error", symbol.file));
	return Tuple<Symbol, List<ScopeList>>(empty, templateArgs);;
}

Symbol Symbol::Get(const Scope& scope, const FileInfo& file) const {
	Symbol s = *this;
	return s.Get(scope, file);
}

Symbol& Symbol::Get(const Scope& scope, const FileInfo& file) {
	try {
		if (scope.types) {
			if (scope.variant) {
				return scopes[scope.name].templateVariants[scope.variant.Get()];
			}
			else if (!scope.types.Get().IsEmpty()) {
				return scopes[scope.name].GetTemplate(scope, file);
			}
		}

		if (scope.variant) {
			return scopes[scope.name].variants[scope.variant.Get()];
		}

		return scopes[scope.name];
	}
	catch (MapKeyError e) {
		ErrorLog::Error(SymbolError(SymbolError::NotFoundStart + this->scope.Add(scope).ToString() + SymbolError::NotFoundEnd, file));
	}

	return empty;
}

Symbol Symbol::Get(const ScopeList& scope, const FileInfo& file) const {
	Symbol s = *this;
	
	for (UInt i = 0; i < scope.Size(); i++) {
		s = s.Get(scope[i], file);

		if (s.type == SymbolType::None) {
			break;
		}
	}

	return s;
}

Symbol Symbol::GetType(const FileInfo& file) const {
	FileInfo file1 = file;
	file1.currentNamespace = symbolNamespace;
	file1.includedNamespaces = includedNamespaces;
	return FindNearestInNamespace(scope.Pop(), varType, file1);
}

bool Symbol::IsOfType(const ScopeList& type) const {
	if (
		this->type == SymbolType::Class ||
		this->type == SymbolType::Enum ||
		this->type == SymbolType::Interface ||
		this->type == SymbolType::Struct ||
		this->type == SymbolType::Type
	) {
		return type == scope;
	}
	else {
		return type == varType;
	}

	/*
	FileInfo file;
	file.currentNamespace = symbolNamespace;
	file.includedNamespaces = includedNamespaces;

	ErrorLog::AddMarker();
	Symbol s = FindNearestInNamespace(scope.Pop(), varType, file);
	ErrorLog::Revert();

	return s.IsOfType(type);
	*/
}

Symbol Symbol::GetReturnType(const UInt index) const {
	FileInfo file;
	file.currentNamespace = symbolNamespace;
	file.includedNamespaces = includedNamespaces;
	file.statementNumber = Math::UIntMax();
	return FindNearestInNamespace(scope.Pop(), returnValues[index], file);
}

Symbol Symbol::GetArgumentType(const UInt index) const {
	FileInfo file;
	file.currentNamespace = symbolNamespace;
	file.includedNamespaces = includedNamespaces;
	file.statementNumber = Math::UIntMax();
	return FindNearestInNamespace(scope.Pop(), arguments[index], file);
}

FileInfo Symbol::GetFileInfo() const {
	return FileInfo(
		"",
		0,
		statementNumber,
		symbolNamespace,
		includedNamespaces
	);
}

bool Symbol::Contains(const Scope& scope) const {
	ErrorLog::AddMarker();
	Symbol s = Get(scope, FileInfo());
	ErrorLog::RevertToMarker();
	ErrorLog::RemoveMarker();

	return s.type != SymbolType::None;
}

bool Symbol::Contains(const ScopeList& scope, const FileInfo& file) const {
	Symbol s = *this;
	
	for (UInt i = 0; i < scope.Size(); i++) {
		if (s.Contains(scope[i])) {
			s = s.Get(scope[i], file);
		}
		else {
			return false;
		}
	}

	return true;
}

bool Symbol::IsType() const {
	return 
		type == SymbolType::Type ||
		type == SymbolType::Class ||
		type == SymbolType::Enum ||
		type == SymbolType::Struct ||
		type == SymbolType::Interface;
}

void Symbol::ClearAssign() {
	for (Pair<String, Symbol>& s : scopes) {
		if (s.value.type == SymbolType::Variable) {
			s.value.isAssigned = false;
		}
	}
}

void Symbol::AssignAll() {
	for (Pair<String, Symbol>& s : scopes) {
		if (s.value.type == SymbolType::Variable) {
			s.value.isAssigned = true;
		}
	}
}

bool Symbol::IsAssigned() const {
	for (const Pair<String, Symbol>& s : scopes) {
		if (s.value.type == SymbolType::Variable && !s.value.isAssigned) {
			return false;
		}
	}

	return true;
}

bool Symbol::IsAllRequiredVars(const List<Scope>& vars) const
{
	for (const Pair<String, Symbol>& s : scopes) {
		if (s.value.type == SymbolType::Variable) {
			bool found = false;

			for (const Scope& var : vars) {
				if (var.name == s.key) {
					found = true;
					break;
				}
			}

			if (!found) {
				return false;
			}
		}
	}

	return true;
}

List<Scope> Symbol::GetUnassignedVars() const {
	List<Scope> unassigned;

	for (const Pair<String, Symbol>& s : scopes) {
		if (s.value.type == SymbolType::Variable && !s.value.isAssigned) {
			unassigned.Add(Scope(s.key));
		}
	}

	return unassigned;
}

Set<Scope> Symbol::GetUnassignedVarsSet() const {
	Set<Scope> unassigned;

	for (const Pair<String, Symbol>& s : scopes) {
		if (s.value.type == SymbolType::Variable && !s.value.isAssigned) {
			unassigned.Add(Scope(s.key));
		}
	}

	return unassigned;
}

List<Scope> Symbol::GetUnassignedVars(const List<Scope>& vars) const {
	List<Scope> unassigned;

	for (const Pair<String, Symbol>& s : scopes) {
		if (s.value.type == SymbolType::Variable) {
			bool found = false;

			for (const Scope& var : vars) {
				if (var.name == s.key) {
					found = true;
					break;
				}
			}

			if (!found) {
				unassigned.Add(Scope(s.key));
			}
		}
	}

	return unassigned;
}

bool Symbol::IsValid() const {
	return type != SymbolType::None;
}

bool Symbol::HasTypeArgs() const {
	return type == SymbolType::Function || type == SymbolType::Method;
}

bool Symbol::HasTypeReturns() const {
	return type == SymbolType::Function || type == SymbolType::Method;
}

void Symbol::SpecializeTemplate(Symbol& symbol, const List<ScopeList>& types, ParsingInfo& info) const {
	return SpecializeTemplate(symbol, *this, types, info);
}

void Symbol::SpecializeTemplate(Symbol& symbol, const Symbol& templateSymbol, const List<ScopeList>& types, ParsingInfo& info) const {
	symbol = *this;

	bool isNotTemplateVariant = true;

	if (!symbol.templateArgs.IsEmpty()) {
		isNotTemplateVariant = symbol.varType != templateSymbol.scope;

		if (!isNotTemplateVariant) {
			symbol.templateArgs = types.Copy();
			symbol.varType      = templateSymbol.scope;
		}
		else {
			symbol.varType = ReplaceTemplates(symbol.varType.Pop(), templateSymbol, types).Add(symbol.varType.Last());

			symbol.templateArgs = List<ScopeList>();

			for (const ScopeList& arg : templateArgs) {
				symbol.templateArgs.Add(ReplaceTemplates(arg, templateSymbol, types));
			}
		}
	}
	else if (symbol.varType.Size() > 0) {
		FileInfo file = symbol.GetFileInfo();
		file.statementNumber++;
		symbol.varType = FindNearestInNamespace(symbol.scope, symbol.varType, file).scope;
		symbol.varType = ReplaceTemplates(symbol.varType, templateSymbol, types);
	}

	if (isNotTemplateVariant) {
		symbol.scope = ReplaceTemplates(symbol.scope.Pop(), templateSymbol, types).Add(symbol.scope.Last());
		symbol.scope.absolute = true;
	}
	else {
		Scope last = symbol.scope.Last();

		if (last.types && last.variant) {
			last.types   = nullptr;
			last.variant = nullptr;
			last.variant = Find(symbol.scope.Pop().Add(last), FileInfo()).templateVariants.Size() - 1;
			last.types   = List<ScopeList>();
			symbol.scope = symbol.scope.Pop().Add(last);
			symbol.scope = ReplaceTemplates(symbol.scope, templateSymbol, types);
			symbol.scope.absolute = true;

			if ((UInt)symbol.scope.Last().variant == (UInt)last.variant) {
				SetTemplateValues(symbol.scope, FileInfo());
			}
		}
		else {
			symbol.scope = ReplaceTemplates(symbol.scope, templateSymbol, types);
			symbol.scope.absolute = true;
		}
	}

	if (HasTypeArgs()) {
		symbol.arguments = List<ScopeList>();

		for (const ScopeList& arg : arguments) {
			symbol.arguments.Add(ReplaceTemplates(arg, templateSymbol, types));
		}
	}
	else {
		symbol.arguments = arguments.Copy();
	}

	if (HasTypeReturns()) {
		symbol.returnValues = List<ScopeList>();

		for (const ScopeList& r : returnValues) {
			symbol.returnValues.Add(ReplaceTemplates(r, templateSymbol, types));
		}
	}
	else {
		symbol.returnValues = returnValues.Copy();
	}

	symbol.scopes = Map<String, Symbol>();

	for (const Pair<String, Symbol>& scope : scopes) {
		if (scope.value.type != SymbolType::Template) {
			symbol.scopes.Add(scope.key, Symbol(SymbolType::Scope));
			scope.value.SpecializeTemplate(symbol.scopes[scope.key], templateSymbol, types, info);
		}
		else {
			Symbol t  = scope.value;
			t.varType = t.scope;
			t.scope   = symbol.scope.Add(Scope(scope.key));
			t.scope.absolute = true;

			symbol.scopes.Add(scope.key, t);
		}
	}

	symbol.variants = List<Symbol>();

	for (const Symbol& variant : variants) {
		symbol.variants.Add(Symbol(SymbolType::Scope));
		variant.SpecializeTemplate(symbol.variants.Last(), templateSymbol, types, info);
	}

	symbol.templateVariants = List<Symbol>();

	for (const Symbol& variant : templateVariants) {
		symbol.templateVariants.Add(Symbol(SymbolType::Scope));
		variant.SpecializeTemplate(symbol.templateVariants.Last(), templateSymbol, types, info);
	}

	for (const Symbol& variant : symbol.templateVariants) {
		symbol.templateVariants.Add(Symbol(SymbolType::Scope));
		variant.SpecializeTemplate(symbol.templateVariants.Last(), Find(variant.varType, FileInfo()), types, info);

		if (symbol.templateVariants.Last().scope.Last().variant.Get() != symbol.templateVariants.Size() - 1) {
			symbol.templateVariants.RemoveLast();
		}
	}

	if (type == SymbolType::Function || type == SymbolType::Method) {
		if (node != nullptr) {
			Pointer<FunctionNode> funcNode = node.Cast<FunctionNode>();
			Pointer<FunctionNode> fn = new FunctionNode(funcNode->scope, funcNode->file);
			fn->argNames = funcNode->argNames;
			fn->func = symbol.scope;
			fn->node = funcNode->node;

			symbol.node = fn;
			symbol.size = info.root.funcs.Size();

			fn->s    = symbol;
			info.root.funcs.Add(symbol.node);
		}
	}
}

ScopeList Symbol::ReplaceTemplates(const ScopeList& type, const Symbol& templateSymbol, const List<ScopeList>& types) {
	ScopeList list;

	for (UInt i = 0; i < type.Size(); i++) {
		Scope scope = type[i].Copy();

		if (scope.types) {
			Symbol s = Find(list.Add(scope), FileInfo());
			List<ScopeList> args;

			bool isTemplate = false;

			if (i < type.Size() - 1) {
				if (s.Contains(type[i + 1])) {
					if (s.Get(type[i + 1], FileInfo()).type == SymbolType::Template) {
						isTemplate = true;
					}
				}
			}

			if (!isTemplate) {
				for (const ScopeList& arg : s.templateArgs) {
					if (arg != type) {
						args.Add(ReplaceTemplates(arg, templateSymbol, types));
					}
					else {
						args.Add(arg);
					}
				}

				Scope newScope = scope.Copy();
				newScope.types = args;
				newScope.variant = nullptr;

				if (Symbol::Contains(list.Add(newScope))) {
					scope = Find(list.Add(newScope), FileInfo()).scope.Last();
				}
				else {
					scope = newScope;
				}
			}
		}

		list = list.Add(scope);
	}

	FileInfo file = templateSymbol.GetFileInfo();
	file.statementNumber++;

	ErrorLog::AddMarker();
	Symbol s = Symbol::FindNearestInNamespace(templateSymbol.scope, list, file);
	ErrorLog::RevertToMarker();
	ErrorLog::RemoveMarker();

	if (s.type == SymbolType::Template) {
		for (UInt i = 0; i < templateSymbol.templateArgs.Size(); i++) {
			Symbol a = Symbol::FindNearestInNamespace(templateSymbol.scope, templateSymbol.templateArgs[i], file);

			if (s.scope == a.scope) {
				return types[i];
			}
		}
	}

	return list;
}

ScopeList Symbol::ReplaceTemplates(const ScopeList& type, const FileInfo& file) {
	ScopeList list;
	list.absolute = type.absolute;

	for (UInt i = 0; i < type.Size(); i++) {
		Scope scope = type[i].Copy();

		if (scope.types) {
			Symbol s = Find(list.Add(scope), FileInfo());
			List<ScopeList> args;

			bool isTemplate = false;

			if (i < type.Size() - 1) {
				if (s.Contains(type[i + 1])) {
					Symbol t = s.Get(type[i + 1], FileInfo());

					if (t.type == SymbolType::Template) {
						isTemplate = true;

						if (t.varType != t.scope) {
							list = ReplaceTemplates(t.varType, file);
						}
						else {
							list = t.varType;
						}

						i++;
						continue;
					}
				}
			}

			if (!isTemplate) {
				for (const ScopeList& arg : s.templateArgs) {
					args.Add(ReplaceTemplates(arg, file));
				}

				Scope newScope = scope.Copy();
				newScope.types = args;
				newScope.variant = nullptr;

				if (Symbol::Contains(list.Add(newScope))) {
					scope = Find(list.Add(newScope), FileInfo()).scope.Last();
				}
				else {
					scope = newScope;
				}
			}
		}

		list = list.Add(scope);
	}

	return list;
}

ScopeList Symbol::ReplaceNearestTemplates(const ScopeList& scope, const ScopeList& type, const FileInfo& file) {
	ScopeList typeScope = type;

	for (UInt i = 0; i < typeScope.Size(); i++) {
		if (typeScope[i].types) {
			for (ScopeList& type : typeScope[i].types.Get()) {
				type = ReplaceTemplates(FindNearestInNamespace(scope, type, file).scope, file);
			}
		}
	}

	return typeScope;
}

void Symbol::SetTemplateValues(const ScopeList& scope, const FileInfo& file) {
	Symbol s = symbols;

	for (UInt i = 0; i < scope.Size(); i++) {
		s = s.Get(scope[i], file);

		if (!s.templateArgs.IsEmpty()) {
			Symbol t = Find(s.varType, file);

			for (UInt u = 0; u < s.templateArgs.Size(); u++) {
				Symbol& arg = Find(t.templateArgs[u].Pop(), file).Get(t.templateArgs[u].Last(), file);

				if (arg.type == SymbolType::Template) {
					arg.varType = s.templateArgs[u];
				}
			}
		}
	}
}

Symbol Symbol::Find(const ScopeList& scopes, const FileInfo& file) {
	if (scopes.Size() == 0) return symbols;
	if (scopes[0].name.Size() == 0) return symbols;

	Symbol s = symbols;

	for (UInt i = scopes[0] == Scope::Global ? 1 : 0; i < scopes.Size(); i++) {
		s = s.Get(scopes[i], file);

		if (s.type == SymbolType::None) {
			break;
		}
	}

	return s;
}

Symbol Symbol::FindInNamespace(const ScopeList& scopes, const FileInfo& file) {
	if (scopes.Size() == 0) return symbols;
	if (scopes[0].name.Size() == 0) return symbols;

	if (scopes.absolute || scopes[0] == Scope::Global) {
		return Symbol::Find(scopes, file);
	}

	if (file.currentNamespace.Size() > 0) {
		if (Symbol::Contains(file.currentNamespace.Add(scopes)) && Symbol::Find(file.currentNamespace.Add(scopes[0]), file).type != SymbolType::Namespace) {
			return Symbol::Find(file.currentNamespace.Add(scopes), file);
		}
	}

	Optional<Symbol> foundSymbol = nullptr;

	ScopeList nearestScopes = file.currentNamespace;

	while (nearestScopes.Size() > 0) {
		if (Symbol::Contains(nearestScopes.Add(scopes))) {
			foundSymbol = Symbol::Find(nearestScopes.Add(scopes), file);
			break;
		}

		if (nearestScopes.Size() == 1 && nearestScopes.Last() == scopes[0] && Symbol::Contains(scopes)) {
			foundSymbol = Symbol::Find(scopes, file);
			break;
		}

		nearestScopes = nearestScopes.Pop();
	}

	if (!foundSymbol) {
		if (Symbol::Contains(scopes)) {
			if (Symbol::Find(ScopeList().Add(scopes[0]), file).type != SymbolType::Namespace) {
				foundSymbol = Symbol::Find(scopes, file);
			}
		}
	}

	for (const ScopeList& includedNamespace : file.includedNamespaces) {
		if (Symbol::Contains(includedNamespace.Pop().Add(scopes))) {
			Symbol s = Symbol::Find(includedNamespace.Pop().Add(scopes), file);

			if (includedNamespace.Last() == scopes[0] || s.symbolFile == scopes[0]) {
				ScopeList includeScopes = includedNamespace.Pop();

				for (UInt i = 0; i < scopes.Size(); i++) {
					if (Symbol::Find(includeScopes.Add(scopes[i]), file).type == SymbolType::Namespace) {
						includeScopes = includeScopes.Add(scopes[i]);
					}
				}

				if (s.symbolNamespace == includeScopes) {
					if (foundSymbol && foundSymbol.Get().scope != s.scope) {
						ErrorLog::Error(SymbolError(SymbolError::Ambiguous(scopes.ToString()), file));
					}

					foundSymbol = s;
				}
			}
		}
	}

	if (foundSymbol) return (Symbol)foundSymbol;

	ErrorLog::Error(SymbolError(SymbolError::NotFoundStart + scopes.ToString() + SymbolError::NotFoundEnd, file));
	return Symbol();
}

bool Symbol::ContainsScope(const Scope& scope) {
	return symbols.Contains(scope);
}

bool Symbol::Contains(const ScopeList& scopes) {
	return symbols.Contains(scopes, FileInfo());
}

Symbol Symbol::FindNearest(const ScopeList& scopes, const Scope& scope, const FileInfo& file) {
	return FindNearest(scopes, ScopeList().Add(scope), file);
}

Symbol Symbol::FindNearest(const ScopeList& scopes, const ScopeList& scope, const FileInfo& file) {
	if (scope.absolute) {
		return Find(scope, file);
	}

	if (scopes.Size() > 0) {
		ScopeList list = scopes;
		
		while (list.Size() > 0) {
			Symbol s = Find(list, file);

			if (s.Contains(scope, file)) {
				Symbol sym = s.Get(scope, file);

				if (sym.statementNumber < file.statementNumber) {
					sym.scope = list.Add(scope);
					sym.scope.absolute = true;
					return sym;
				}
			}

			list = list.Pop();
		}
	}

	Symbol s = symbols.Get(scope, file);
	s.scope = scope;
	s.scope.absolute = true;
	return s;
}

Symbol Symbol::FindNearestInNamespace(const ScopeList& scopes, const Scope& scope, const FileInfo& file) {
	return FindNearestInNamespace(scopes, ScopeList().Add(scope), file);
}

Symbol Symbol::FindNearestInNamespace(const ScopeList& scopes, const ScopeList& scope, const FileInfo& file) {
	if (scope.absolute) {
		return Find(scope, file);
	}

	ScopeList scopeList = scope;

	for (UInt i = 0; i < scopeList.Size(); i++) {
		if (scopeList[i].types) {
			for (ScopeList& type : scopeList[i].types.Get()) {
				type = FindNearestInNamespace(scopes, type, file).scope;
			}
		}
	}

	if (scopes.Size() > 0) {
		ScopeList list = scopes;

		while (list.Size() > 0) {
			Symbol s = FindInNamespace(list, file);

			if (s.Contains(scopeList, file)) {
				Symbol sym = s.Get(scopeList, file);

				if (sym.statementNumber < file.statementNumber) {
					return sym;
				}
			}

			list = list.Pop();
		}
	}

	return FindInNamespace(scopeList, file);
}

Symbol Symbol::FindNearestType(const ScopeList& scopes, const Scope& scope, const FileInfo& file) {
	return FindNearest(scopes, ScopeList().Add(scope), file);
}

Symbol Symbol::FindNearestType(const ScopeList& scopes, const ScopeList& scope, const FileInfo& file) {
	if (scope.absolute) {
		Find(scope, file);
	}

	if (scopes.Size() > 0) {
		ScopeList list = scopes;
		
		while (list.Size() > 0) {
			Symbol s = Find(list, file);

			if (s.Contains(scope, file)) {
				Symbol sym = s.Get(scope, file);

				if (sym.IsType()) {
					sym.scope = list.Add(scope);
					sym.scope.absolute = true;
					return sym;
				}
			}

			list = list.Pop();
		}
	}

	Symbol s = symbols.Get(scope, file);
	s.scope = scope;
	s.scope.absolute = true;
	return s;
}

Symbol Symbol::FindNearestTypeInNamespace(const ScopeList& scopes, const Scope& scope, const FileInfo& file) {
	return FindNearestInNamespace(scopes, ScopeList().Add(scope), file);
}

Symbol Symbol::FindNearestTypeInNamespace(const ScopeList& scopes, const ScopeList& scope, const FileInfo& file) {
	if (scope.absolute) {
		return Find(scope, file);
	}

	if (scopes.Size() > 0) {
		ScopeList list = scopes;

		while (list.Size() > 0) {
			Symbol s = FindInNamespace(list, file);

			if (s.Contains(scope, file)) {
				Symbol sym = s.Get(scope, file);

				if (sym.IsType()) {
					sym.scope = list.Add(scope);
					sym.scope.absolute = true;
					return sym;
				}
			}

			list = list.Pop();
		}
	}

	return FindInNamespace(scope, file);
}

bool Symbol::IsOfType(const ScopeList& scope, const ScopeList& type, const FileInfo& file) {
	return Find(scope, file).IsOfType(type);
}

Symbol Symbol::FindCurrentFunction(const ScopeList& scopes, const FileInfo& file) {
	if (scopes.Size() > 0) {
		ScopeList list = scopes;

		while (list.Size() > 0) {
			Symbol s = Find(list, file);

			if (s.type == SymbolType::Function || s.type == SymbolType::Method) {
				s.scope = list;
				s.scope.absolute = true;
				return s;
			}

			list = list.Pop();
		}
	}

	return Symbol();
}

Symbol Symbol::FindOperator(const Scope& op, const ScopeList& type1, const ScopeList& type2, const FileInfo& file) {
	Symbol s = Find(type1.Add(op), file);

	for (UInt i = 0; i < s.variants.Size(); i++) {
		if (
			s.variants[i].type == SymbolType::Function &&
			s.variants[i].arguments.Size() == 2 &&
			FindNearest(type1, s.variants[i].arguments[0], file).scope == type1 &&
			FindNearest(type1, s.variants[i].arguments[1], file).scope == type2
		) {
			Symbol sym = s.variants[i];

			Scope os = op;
			os.variant = i;

			sym.scope = type1.Add(os);
			sym.scope.absolute = true;
			return sym;
		}
	}

	s = Find(type2.Add(op), file);

	for (UInt i = 0; i < s.variants.Size(); i++) {
		if (
			s.variants[i].type == SymbolType::Function &&
			s.variants[i].arguments.Size() == 2 &&
			FindNearest(type2, s.variants[i].arguments[0], file).scope == type1 &&
			FindNearest(type2, s.variants[i].arguments[1], file).scope == type2
		) {
			Symbol sym = s.variants[i];

			Scope os = op;
			os.variant = i;

			sym.scope = type2.Add(os);
			sym.scope.absolute = true;
			return sym;
		}
	}

	return Symbol();
}

Symbol Symbol::FindFunction(const ScopeList& func, const List<ScopeList>& types, const FileInfo& file) {
	return FindFunction(func, types, file, 0, SymbolType::Function);
}

Symbol Symbol::FindMethod(const ScopeList& func, const List<ScopeList>& types, const FileInfo& file) {
	return FindFunction(func, types, file, 1, SymbolType::Method);
}

Symbol Symbol::FindFunction(const ScopeList& func, const List<ScopeList>& types, const FileInfo& file, const UInt offset, const SymbolType type) {
	Symbol s = Find(func, file);

	Optional<Symbol> implicitFunc = nullptr;
	bool multipleImplicit = false;

	for (UInt i = 0; i < s.variants.Size(); i++) {
		if (s.variants[i].type == type && s.variants[i].arguments.Size() == types.Size() + offset) {
			// Find exact match
			bool match = true;

			for (UInt a = 0; a < types.Size(); a++) {
				if (types[a] != FindNearestType(func.Pop(), s.variants[i].arguments[a + offset], file).scope) {
					match = false;
					break;
				}
			}

			if (match) {
				Symbol sym = s.variants[i];

				ScopeList list = func;
				list[list.Size() - 1].variant = i;

				sym.scope = list;
				sym.scope.absolute = true;
				return sym;
			}

			// Find implict match
			match = true;

			for (UInt a = 0; a < types.Size(); a++) {
				if (!HasImplicitConversion(types[a], FindNearestType(func.Pop(), s.variants[i].arguments[a + offset], file).scope)) {
					match = false;
					break;
				}
			}

			if (match) {
				if (implicitFunc) multipleImplicit = true;

				Symbol sym = s.variants[i];

				ScopeList list = func;
				list[list.Size() - 1].variant = i;

				sym.scope = list;
				sym.scope.absolute = true;
				implicitFunc = sym;
			}
		}
	}

	if (multipleImplicit) {
		List<String> args;

		for (const ScopeList& type : types) {
			args.Add(type.ToString());
		}

		ErrorLog::Error(SymbolError(SymbolError::AmbiguousCall(func.ToString(), args), file));
		return Symbol();
	}

	if (implicitFunc) {
		return implicitFunc.Get();
	}

	List<String> args;

	for (const ScopeList& type : types) {
		args.Add(type.ToString());
	}

	ErrorLog::Error(SymbolError(SymbolError::Function(func.ToString(), args), file));

	return Symbol();
}

Symbol Symbol::FindImplicitConversion(const ScopeList& from, const ScopeList& to, const FileInfo& file) {
	Symbol convert = FindExplicitConversion(from, to, file);

	if (convert.isExplicit) {
		ErrorLog::Error(SymbolError(SymbolError::ImplicitConvert(from.ToString(), to.ToString()), file));
		return Symbol();
	}

	return convert;
}

Symbol Symbol::FindExplicitConversion(const ScopeList& from, const ScopeList& to, const FileInfo& file) {
	Symbol s = Find(from.Add(Scope::As), file);

	for (UInt i = 0; i < s.variants.Size(); i++) {
		if (
			s.variants[i].type == SymbolType::Function &&
			s.variants[i].arguments.Size() == 1 &&
			s.variants[i].returnValues.Size() == 1 &&
			FindNearest(from, s.variants[i].arguments[0], file).scope == from &&
			FindNearest(from, s.variants[i].returnValues[0], file).scope == to
		) {
			Symbol sym = s.variants[i];

			Scope as = Scope::As;
			as.variant = i;

			sym.scope = from.Add(as);
			sym.scope.absolute = true;
			return sym;
		}
	}

	s = Find(to.Add(Scope::As), file);

	for (UInt i = 0; i < s.variants.Size(); i++) {
		if (
			s.variants[i].type == SymbolType::Function &&
			s.variants[i].arguments.Size() == 1 &&
			s.variants[i].returnValues.Size() == 1 &&
			FindNearest(to, s.variants[i].arguments[0], file).scope == from &&
			FindNearest(to, s.variants[i].returnValues[0], file).scope == to
		) {
			Symbol sym = s.variants[i];

			Scope as = Scope::As;
			as.variant = i;

			sym.scope = to.Add(as);
			sym.scope.absolute = true;
			return sym;
		}
	}
	
	ErrorLog::Error(SymbolError(SymbolError::ExplicitConvert(from.ToString(), to.ToString()), file));
	return Symbol();
}

bool Symbol::HasImplicitConversion(const ScopeList& from, const ScopeList& to) {
	if (from == to) return true;

	ErrorLog::AddMarker();
	Symbol s = FindImplicitConversion(from, to, FileInfo());
	ErrorLog::Revert();

	return s.type == SymbolType::Function || s.type == SymbolType::Method;
}

bool Symbol::HasExplicitConversion(const ScopeList& from, const ScopeList& to) {
	if (from == to) return true;

	ErrorLog::AddMarker();
	Symbol s = FindExplicitConversion(from, to, FileInfo());
	ErrorLog::Revert();

	return s.type == SymbolType::Function || s.type == SymbolType::Method;
}

Mango Symbol::ToMango(const bool ignoreBasic) {
	Mango symbols = Mango("symbols", MangoType::List);
	
	for (Pair<String, Symbol>& pair : Symbol::symbols.scopes) {
		if (!ignoreBasic || !pair.value.basic) {
			for (const Mango& m : pair.value.ToMangoList(ScopeList().Add(Scope(pair.key)))) {
				symbols.Add(m);
			}
		}
	}

	return symbols;
}

List<Mango> Symbol::ToMangoList(const ScopeList& scope) {
	List<Mango> symbols;

	if (type == SymbolType::Type || type == SymbolType::Struct || type == SymbolType::Class || type == SymbolType::Enum || type == SymbolType::Interface) {
		Mango m = Mango(scope.ToString(), MangoType::Map);

		if (type != SymbolType::Interface) {
			m.Add("size", Mango((Long)size));
		}

		switch (type) {
			case SymbolType::Type:      m.Add("type", Mango(String("basic")));     break;
			case SymbolType::Struct:    m.Add("type", Mango(String("struct")));    break;
			case SymbolType::Class:     m.Add("type", Mango(String("class")));     break;
			case SymbolType::Enum:      m.Add("type", Mango(String("enum")));      break;
			case SymbolType::Interface: m.Add("type", Mango(String("interface"))); break;
		}

		symbols.Add(m);
	}
	else if (type == SymbolType::Variable) {
		Mango m = Mango(scope.ToString(), MangoType::Map);
		this->scope = scope;

		FileInfo file;
		file.statementNumber++;
		m.Add("type", GetType(file).scope.ToString());

		bool onlyType = true;

		if (!attributes.IsEmpty()) {
			Mango attr = Mango(MangoType::List);

			for (const SymbolAttribute a : attributes) {
				switch (a) {
					case SymbolAttribute::Const: attr.Add(Mango(String("const"))); break;
					case SymbolAttribute::Copy: attr.Add(Mango(String("copy"))); break;
					case SymbolAttribute::Ref: attr.Add(Mango(String("ref"))); break;
				}
			}

			m.Add("attributes", attr);
			onlyType = false;
		}

		if (!onlyType) {
			symbols.Add(m);
		}
		else {
			symbols.Add(Mango(m.GetLabel(), (String)m[String("type")]));
		}
	}
	else if (variants.IsEmpty() && (type == SymbolType::Function || type == SymbolType::Method)) {
		String r = "";

		for (UInt i = 0; i < returnValues.Size(); i++) {
			if (i > 0) r += ", ";
			r += GetReturnType(i).scope.ToString();
		}

		if (r.Size() > 0) r += " ";

		String a = "";

		for (UInt i = 0; i < arguments.Size(); i++) {
			if (i > 0) a += ", ";
			a += GetArgumentType(i).scope.ToString();
		}

		symbols.Add(Mango(scope.ToString(), r + "(" + a + ")"));
	}

	for (UInt i = 0; i < variants.Size(); i++) {
		ScopeList list = scope;
		list[list.Size() - 1].variant = i;

		for (const Mango& m : variants[i].ToMangoList(list)) {
			symbols.Add(m);
		}
	}

	for (UInt i = 0; i < templateVariants.Size(); i++) {
		ScopeList list = scope;
		list[list.Size() - 1].types = List<ScopeList>();
		list[list.Size() - 1].variant = i;

		for (const Mango& m : templateVariants[i].ToMangoList(list)) {
			symbols.Add(m);
		}
	}
	
	for (Pair<String, Symbol>& pair : scopes) {
		for (const Mango& m : pair.value.ToMangoList(scope.Add(Scope(pair.key)))) {
			symbols.Add(m);
		}
	}

	return symbols;
}

void Symbol::Setup() {
	// -------------- Integers --------------
	Map<ScopeList, Byte> integers;
	integers.Add(ScopeList::Byte, -1);
	integers.Add(ScopeList::UByte, 1);
	integers.Add(ScopeList::Short, -2);
	integers.Add(ScopeList::UShort, 2);
	integers.Add(ScopeList::Int, -4);
	integers.Add(ScopeList::UInt, 4);
	integers.Add(ScopeList::Long, -8);
	integers.Add(ScopeList::ULong, 8);

	Map<Scope, InstructionType> binOps;
	binOps.Add(Scope::Add, InstructionType::Add);
	binOps.Add(Scope::Sub, InstructionType::Sub);
	binOps.Add(Scope::Mul, InstructionType::Mul);
	binOps.Add(Scope::IDiv, InstructionType::Div);
	binOps.Add(Scope::Mod, InstructionType::Mod);
	binOps.Add(Scope::BitOr, InstructionType::Or);
	binOps.Add(Scope::BitAnd, InstructionType::And);
	binOps.Add(Scope::BitXor, InstructionType::Xor);
	binOps.Add(Scope::BitNor, InstructionType::Nor);
	binOps.Add(Scope::BitNand, InstructionType::Nand);
	binOps.Add(Scope::BitXnor, InstructionType::Xnor);
	binOps.Add(Scope::ShiftLeft, InstructionType::ShL);
	binOps.Add(Scope::ShiftRight, InstructionType::ShR);
	binOps.Add(Scope::Equal, InstructionType::Eq);
	binOps.Add(Scope::NotEqual, InstructionType::Ne);
	binOps.Add(Scope::LessEqual, InstructionType::Le);
	binOps.Add(Scope::GreaterEqual, InstructionType::Ge);
	binOps.Add(Scope::Less, InstructionType::Lt);
	binOps.Add(Scope::Greater, InstructionType::Gt);

	for (const Pair<ScopeList, Byte> integer : integers) {
		// Type
		Symbol intSym = Symbol(SymbolType::Type);
		intSym.basic = true;
		intSym.size = integer.value < 0 ? -integer.value : integer.value;
		intSym.isSigned = integer.value < 0;

		// Min max
		/*
		Symbol min = Symbol(SymbolType::Variable);
		min.attributes.Add(SymbolAttribute::Static);
		min.attributes.Add(SymbolAttribute::Const);
		min.varType = integer.key;
		*/

		// Neg
		Symbol neg = Symbol(SymbolType::Function);
		neg.symbolNode = new IntegerUnaryOperatorNode(abs(integer.value), InstructionType::Neg);

		if (integer.value > 0) {
			neg.arguments.Add(ScopeList(true).Add(Scope(integer.key[0].name.Sub(1))));
			neg.returnValues.Add(ScopeList(true).Add(Scope(integer.key[0].name.Sub(1))));
		}
		else {
			neg.arguments.Add(integer.key);
			neg.returnValues.Add(integer.key);
		}

		intSym.Add(Scope::Neg, neg, FileInfo());

		// Bit Not
		Symbol bnot = Symbol(SymbolType::Function);
		bnot.symbolNode = new IntegerUnaryOperatorNode(abs(integer.value), InstructionType::Not);
		bnot.arguments.Add(integer.key);
		bnot.returnValues.Add(integer.key);

		intSym.Add(Scope::BitNot, bnot, FileInfo());

		// To Bool
		Symbol intToBool = Symbol(SymbolType::Function);
		intToBool.arguments.Add(integer.key);
		intToBool.returnValues.Add(ScopeList::Bool);
		intToBool.symbolNode = new IntegerToBoolNode();
		intToBool.isExplicit = true;
		intSym.Add(Scope::As, intToBool, FileInfo());

		// Not
		Symbol intNot = Symbol(SymbolType::Function);
		intNot.arguments.Add(integer.key);
		intNot.returnValues.Add(ScopeList::Bool);
		intNot.symbolNode = new IntegerNotNode();
		intSym.Add(Scope::Not, intNot, FileInfo());

		// Assign
		Symbol intAssign = Symbol(SymbolType::Function);
		intAssign.arguments.Add(integer.key);
		intAssign.symbolNode = new IntegerAssignNode(integer.value);
		intSym.Add(Scope::Assign, intAssign, FileInfo());

		// Binary operators
		for (const Pair<ScopeList, Byte> integer2 : integers) {
			Symbol intConvert = Symbol(SymbolType::Function);
			intConvert.arguments.Add(integer.key);
			intConvert.returnValues.Add(integer2.key);

			Pointer<IntegerConvertNode> cn = new IntegerConvertNode();
			cn->size = Math::Abs(integer.value);
			cn->sign = integer.value < 0;
			cn->targetSize = Math::Abs(integer2.value);

			intConvert.symbolNode = cn;
			intConvert.isExplicit = false;
			intSym.Add(Scope::As, intConvert, FileInfo());

			for (const Pair<Scope, InstructionType>& op : binOps) {
				Symbol intOp = Symbol(SymbolType::Function);

				UByte v1 = integer.value < 0 ? -integer.value : integer.value;
				UByte v2 = integer2.value < 0 ? -integer2.value : integer2.value;
				bool sign = integer.value < 0 || integer2.value < 0;

				if (Instruction::IsComp(op.value))
					intOp.returnValues.Add(ScopeList::Bool);
				else {
					const ScopeList name = v1 > v2 ? integer.key : integer2.key;

					if (sign && name[0].name[0] == 'u')
						intOp.returnValues.Add(ScopeList(true).Add(Scope(name[0].name.Sub(1))));
					else
						intOp.returnValues.Add(name);
				}

				intOp.arguments.Add(integer.key);
				intOp.arguments.Add(integer2.key);
				intOp.symbolNode = new IntegerBinaryOperatorNode(
					v1 > v2 ? v1 : v2,
					sign,
					op.value
				);

				intSym.Add(op.key, intOp, FileInfo());
			}
		}

		symbols.Add(integer.key[0], intSym, FileInfo());
	}

	// ----------------- Boolean ---------------------
	Symbol boolSym = Symbol(SymbolType::Type);
	boolSym.size = 1;
	boolSym.isSigned = false;
	boolSym.basic = true;

	Symbol boolAssign = Symbol(SymbolType::Function);
	boolAssign.arguments.Add(ScopeList::Bool);
	boolAssign.symbolNode = new BooleanAssignNode();
	boolSym.Add(Scope::Assign, boolAssign, FileInfo());

	Symbol boolEq = Symbol(SymbolType::Function);
	boolEq.arguments.Add(ScopeList::Bool);
	boolEq.arguments.Add(ScopeList::Bool);
	boolEq.returnValues.Add(ScopeList::Bool);
	boolEq.symbolNode = new BooleanCompareNode(InstructionType::Eq);
	boolSym.Add(Scope::Equal, boolEq, FileInfo());

	Symbol boolNe = Symbol(SymbolType::Function);
	boolNe.arguments.Add(ScopeList::Bool);
	boolNe.arguments.Add(ScopeList::Bool);
	boolNe.returnValues.Add(ScopeList::Bool);
	boolNe.symbolNode = new BooleanCompareNode(InstructionType::Ne);
	boolSym.Add(Scope::NotEqual, boolNe, FileInfo());

	Symbol boolNot = Symbol(SymbolType::Function);
	boolNot.arguments.Add(ScopeList::Bool);
	boolNot.returnValues.Add(ScopeList::Bool);
	boolNot.symbolNode = new BooleanNotNode();
	boolSym.Add(Scope::Not, boolNot, FileInfo());

	Symbol boolToBool = Symbol(SymbolType::Function);
	boolToBool.arguments.Add(ScopeList::Bool);
	boolToBool.returnValues.Add(ScopeList::Bool);
	boolToBool.isExplicit = false;
	boolToBool.symbolNode = new BooleanToBooleanNode();
	boolSym.Add(Scope::As, boolToBool, FileInfo());

	symbols.Add(ScopeList::Bool[0], boolSym, FileInfo());

	// ----------------- Nil ---------------------

	Symbol nilSym = Symbol(SymbolType::Type);
	nilSym.scope  = ScopeList::Nil;
	nilSym.size   = 0;
	nilSym.basic  = true;

	Symbol nilNot = Symbol(SymbolType::Function);
	nilNot.arguments.Add(ScopeList::Nil);
	nilNot.returnValues.Add(ScopeList::Bool);
	nilNot.symbolNode = new BooleanConstantNode(true);
	nilSym.Add(Scope::Not, nilNot, FileInfo());

	Symbol nilToBool = Symbol(SymbolType::Function);
	nilToBool.arguments.Add(ScopeList::Nil);
	nilToBool.returnValues.Add(ScopeList::Bool);
	nilToBool.isExplicit = true;
	nilToBool.symbolNode = new BooleanConstantNode(false);
	nilSym.Add(Scope::As, nilToBool, FileInfo());

	symbols.Add(ScopeList::Nil[0], nilSym, FileInfo());

	// ----------------- Optional ---------------------
	Symbol optionalScope = Symbol(SymbolType::Scope);
	optionalScope.scope  = ScopeList(true).Add(Scope::Optional);
	optionalScope.basic  = true;

	{
		Symbol optionalSym = Symbol(SymbolType::Struct);
		Scope last   = optionalScope.scope.Last();
		last.types   = List<ScopeList>();
		last.variant = (UInt)0;

		optionalSym.scope  = optionalScope.scope.Pop().Add(last);
		optionalSym.varType = optionalSym.scope;

		optionalSym.names.Add(Scope::HasValue);
		optionalSym.names.Add(Scope::Value);

		optionalSym.templateArgs.Add(ScopeList().Add(Scope("T")));

		{
			Symbol templateArg  = Symbol(SymbolType::Template);
			templateArg.scope   = optionalSym.scope.Add(ScopeList().Add(Scope("T")));
			templateArg.varType = templateArg.scope;
			optionalSym.Add(templateArg.scope.Last(), templateArg, FileInfo());

			Symbol hasSym  = Symbol(SymbolType::Variable);
			hasSym.varType = ScopeList::Bool;
			hasSym.scope   = optionalSym.scope.Add(Scope::HasValue);
			optionalSym.Add(Scope::HasValue, hasSym, FileInfo());

			Symbol valueSym  = Symbol(SymbolType::Variable);
			valueSym.varType = templateArg.scope;
			valueSym.scope   = optionalSym.scope.Add(Scope::Value);
			optionalSym.Add(Scope::Value, valueSym, FileInfo());

			Symbol optionalAssign = Symbol(SymbolType::Function);
			optionalAssign.arguments.Add(optionalSym.scope);
			optionalAssign.symbolNode = new OptionalAssignNode();
			optionalSym.Add(Scope::Assign, optionalAssign, FileInfo());

			Symbol valueAssign = Symbol(SymbolType::Function);
			valueAssign.arguments.Add(valueSym.varType);
			valueAssign.symbolNode = new OptionalAssignValueNode();
			optionalSym.Add(Scope::Assign, valueAssign, FileInfo());

			Symbol nilAssign = Symbol(SymbolType::Function);
			nilAssign.arguments.Add(ScopeList::Nil);
			nilAssign.symbolNode = new OptionalAssignValueNode();
			optionalSym.Add(Scope::Assign, nilAssign, FileInfo());

			Symbol unwrap = Symbol(SymbolType::Function);
			unwrap.arguments.Add(optionalSym.scope);
			unwrap.returnValues.Add(templateArg.scope);
			unwrap.symbolNode = new OptionalUnwrapNode();
			optionalSym.Add(Scope::Unwrap, unwrap, FileInfo());

			Symbol toBool = Symbol(SymbolType::Function);
			toBool.arguments.Add(optionalSym.scope);
			toBool.returnValues.Add(ScopeList::Bool);
			toBool.isExplicit = true;
			toBool.symbolNode = new OptionalToBooleanNode();
			optionalSym.Add(Scope::As, toBool, FileInfo());

			Symbol optionalNot = Symbol(SymbolType::Function);
			optionalNot.arguments.Add(optionalSym.scope);
			optionalNot.returnValues.Add(ScopeList::Bool);
			optionalNot.symbolNode = new OptionalNotNode();
			optionalSym.Add(Scope::Not, optionalNot, FileInfo());
		}

		optionalScope.templateVariants.Add(optionalSym);
	}

	symbols.Add(Scope::Optional, optionalScope, FileInfo());
}