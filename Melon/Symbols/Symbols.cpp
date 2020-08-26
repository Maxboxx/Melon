#include "Symbols.h"

#include "Boxx/Map.h"
#include "Boxx/Optional.h"

#include "Kiwi/Kiwi.h"

#include "Nodes/SymbolNode.h"
#include "Nodes/IntegerAssignNode.h"
#include "Nodes/IntegerUnaryOperatorNode.h"
#include "Nodes/IntegerBinaryOperatorNode.h"
#include "Nodes/BooleanAssignNode.h"
#include "Nodes/BooleanBinaryOperatorNode.h"
#include "Nodes/BooleanUnaryOperatorNode.h"
#include "Nodes/BooleanToBooleanNode.h"

#include "ScopeList.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

Symbol Symbol::symbols = Symbol(SymbolType::Namespace);
Symbol Symbol::empty = Symbol(SymbolType::None);

Symbol::Symbol() {

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
				scopes.Add(scope.name, f);
			}

			for (const Symbol& s : scopes[scope.name].variants) {
				if (s.args.Size() != symbol.args.Size()) continue;
				if (s.type != symbol.type) continue;

				bool same = true;

				for (UInt i = 0; i < s.args.Size(); i++) {
					if (s.args[i] != symbol.args[i]) {
						same = false;
						break;
					}
				}

				if (!redefine && same) {
					ErrorLog::Error(SymbolError(SymbolError::RedefinitionStart + s.scope.ToString() + SymbolError::RedefinitionEnd, file));
				}
			}

			Symbol sym = symbol;
			Scope symScope = scope;
			symScope.variant = scopes[scope.name].variants.Size();
			sym.scope = this->scope.Add(symScope);
			scopes[scope.name].variants.Add(sym);
		}
		else {
			if (!redefine && scopes.Contains(scope.name)) {
				ErrorLog::Error(SymbolError(SymbolError::RedefinitionStart + this->scope.Add(scope).ToString() + SymbolError::RedefinitionEnd, file));
			}

			Symbol sym = symbol;
			sym.scope = this->scope.Add(scope);
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

		if (symbol.type == SymbolType::Function || symbol.type == SymbolType::Method) {
			if (!s.scopes.Contains(scopes.Last().name)) {
				Symbol f;
				f.type = SymbolType::Function;
				f.scope = scopes;
				s.scopes.Add(scopes.Last().name, f);
			}
			else if (s.scopes[scopes.Last().name].variants.Size() == 1) {
				if (s.scopes[scopes.Last().name].variants[0].type == SymbolType::Scope) {
					s.scopes[scopes.Last().name].variants.RemoveAt(0);
				}
			}

			for (const Symbol& s : s.scopes[scopes.Last().name].variants) {
				if (s.args.Size() != symbol.args.Size()) continue;
				if (s.type != symbol.type) continue;

				bool same = true;

				for (UInt i = 0; i < s.args.Size(); i++) {
					if (s.args[i] != symbol.args[i]) {
						same = false;
						break;
					}
				}

				if (!redefine && same) {
					ErrorLog::Error(SymbolError(SymbolError::RedefinitionStart + s.scope.ToString() + SymbolError::RedefinitionEnd, file));
				}
			}

			if (!redefine) {
				Symbol sym = symbol;
				Scope symScope = scopes.Last();
				symScope.variant = s.scopes[scopes.Last().name].variants.Size();
				sym.scope = scopes.Pop().Add(symScope);
				s.scopes[scopes.Last().name].variants.Add(sym);
			}
			else {
				Symbol sym = symbol;
				Scope symScope = scopes.Last();
				sym.scope = scopes;
				s.scopes[scopes.Last().name].variants[scopes.Last().variant] = sym;
			}
		}
		else {
			if (!redefine && s.scopes.Contains(scopes.Last().name)) {
				ErrorLog::Error(SymbolError(SymbolError::RedefinitionStart + scopes.ToString() + SymbolError::RedefinitionEnd, file));
			}

			Symbol sym = symbol;
			sym.scope = scopes;
			s.scopes.Add(scopes.Last().name, sym);
		}
	}
	catch (MapError e) {
		return false;
	}

	return true;
}

Symbol Symbol::Get(const Scope& scope, const FileInfo& file) const {
	try {
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

Symbol& Symbol::Get(const Scope& scope, const FileInfo& file) {
	try {
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

Symbol Symbol::GetReturnType(const UInt index) const {
	FileInfo file;
	file.currentNamespace = symbolNamespace;
	file.includedNamespaces = includedNamespaces;
	return FindNearestInNamespace(scope.Pop(), ret[index], file);
}

Symbol Symbol::GetArgumentType(const UInt index) const {
	FileInfo file;
	file.currentNamespace = symbolNamespace;
	file.includedNamespaces = includedNamespaces;
	return FindNearestInNamespace(scope.Pop(), args[index], file);
}

bool Symbol::Contains(const Scope& scope) const {
	if (!scope.variant.HasValue()) return scopes.Contains(scope.name);

	Symbol s;

	if (scopes.Contains(scope.name, s)) {
		return s.variants.Size() > scope.variant.Get();
	}

	return false;
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
			s.value.sign = false;
		}
	}
}

void Symbol::AssignAll() {
	for (Pair<String, Symbol>& s : scopes) {
		if (s.value.type == SymbolType::Variable) {
			s.value.sign = true;
		}
	}
}

bool Symbol::IsAssigned() const {
	for (const Pair<String, Symbol>& s : scopes) {
		if (s.value.type == SymbolType::Variable && !s.value.sign) {
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
		if (s.value.type == SymbolType::Variable && !s.value.sign) {
			unassigned.Add(Scope(s.key));
		}
	}

	return unassigned;
}

Set<Scope> Symbol::GetUnassignedVarsSet() const {
	Set<Scope> unassigned;

	for (const Pair<String, Symbol>& s : scopes) {
		if (s.value.type == SymbolType::Variable && !s.value.sign) {
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

	if (scopes[0] == Scope::Global) {
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
						ErrorLog::Error(SymbolError(SymbolError::AmbiguousStart + scopes.ToString() + SymbolError::AmbiguousEnd, file));
					}

					foundSymbol = s;
				}
			}
		}
	}

	if (foundSymbol) return foundSymbol;

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
	if (scopes.Size() > 0) {
		ScopeList list = scopes;
		
		while (list.Size() > 0) {
			Symbol s = Find(list, file);

			if (s.Contains(scope, file)) {
				Symbol sym = s.Get(scope, file);

				if (sym.statementNumber < file.statementNumber) {
					sym.scope = list.Add(scope);
					return sym;
				}
			}

			list = list.Pop();
		}
	}

	Symbol s = symbols.Get(scope, file);
	s.scope = scope;
	return s;
}

Symbol Symbol::FindNearestInNamespace(const ScopeList& scopes, const Scope& scope, const FileInfo& file) {
	return FindNearestInNamespace(scopes, ScopeList().Add(scope), file);
}

Symbol Symbol::FindNearestInNamespace(const ScopeList& scopes, const ScopeList& scope, const FileInfo& file) {
	if (scopes.Size() > 0) {
		ScopeList list = scopes;

		while (list.Size() > 0) {
			Symbol s = FindInNamespace(list, file);

			if (s.Contains(scope, file)) {
				Symbol sym = s.Get(scope, file);

				if (sym.statementNumber < file.statementNumber) {
					sym.scope = list.Add(scope);
					return sym;
				}
			}

			list = list.Pop();
		}
	}

	return FindInNamespace(scope, file);
}

Symbol Symbol::FindNearestType(const ScopeList& scopes, const Scope& scope, const FileInfo& file) {
	return FindNearest(scopes, ScopeList().Add(scope), file);
}

Symbol Symbol::FindNearestType(const ScopeList& scopes, const ScopeList& scope, const FileInfo& file) {
	if (scopes.Size() > 0) {
		ScopeList list = scopes;
		
		while (list.Size() > 0) {
			Symbol s = Find(list, file);

			if (s.Contains(scope, file)) {
				Symbol sym = s.Get(scope, file);

				if (sym.IsType()) {
					sym.scope = list.Add(scope);
					return sym;
				}
			}

			list = list.Pop();
		}
	}

	Symbol s = symbols.Get(scope, file);
	s.scope = scope;
	return s;
}

Symbol Symbol::FindNearestTypeInNamespace(const ScopeList& scopes, const Scope& scope, const FileInfo& file) {
	return FindNearestInNamespace(scopes, ScopeList().Add(scope), file);
}

Symbol Symbol::FindNearestTypeInNamespace(const ScopeList& scopes, const ScopeList& scope, const FileInfo& file) {
	if (scopes.Size() > 0) {
		ScopeList list = scopes;

		while (list.Size() > 0) {
			Symbol s = FindInNamespace(list, file);

			if (s.Contains(scope, file)) {
				Symbol sym = s.Get(scope, file);

				if (sym.IsType()) {
					sym.scope = list.Add(scope);
					return sym;
				}
			}

			list = list.Pop();
		}
	}

	return FindInNamespace(scope, file);
}

Symbol Symbol::FindCurrentFunction(const ScopeList& scopes, const FileInfo& file) {
	if (scopes.Size() > 0) {
		ScopeList list = scopes;

		while (list.Size() > 0) {
			Symbol s = Find(list, file);

			if (s.type == SymbolType::Function || s.type == SymbolType::Method) {
				s.scope = list;
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
			s.variants[i].args.Size() == 2 &&
			FindNearest(type1, s.variants[i].args[0], file).scope == type1 &&
			FindNearest(type1, s.variants[i].args[1], file).scope == type2
		) {
			Symbol sym = s.variants[i];

			Scope os = op;
			os.variant = i;

			sym.scope = type1.Add(os);
			return sym;
		}
	}

	s = Find(type2.Add(op), file);

	for (UInt i = 0; i < s.variants.Size(); i++) {
		if (
			s.variants[i].type == SymbolType::Function &&
			s.variants[i].args.Size() == 2 &&
			FindNearest(type2, s.variants[i].args[0], file).scope == type1 &&
			FindNearest(type2, s.variants[i].args[1], file).scope == type2
		) {
			Symbol sym = s.variants[i];

			Scope os = op;
			os.variant = i;

			sym.scope = type2.Add(os);
			return sym;
		}
	}

	return Symbol();
}

Symbol Symbol::FindFunction(const ScopeList& func, const List<ScopeList>& types, const FileInfo& file) {
	Symbol s = Find(func, file);

	for (UInt i = 0; i < s.variants.Size(); i++) {
		if (s.variants[i].type == SymbolType::Function && s.variants[i].args.Size() == types.Size()) {
			bool match = true;

			for (UInt a = 0; a < types.Size(); a++) {
				if (types[a] != FindNearestType(func.Pop(), s.variants[i].args[a], file).scope) {
					match = false;
					break;
				}
			}

			if (match) {
				Symbol sym = s.variants[i];

				ScopeList list = func;
				list[list.Size() - 1].variant = i;

				sym.scope = list;
				return sym;
			}
		}
	}

	List<String> args;

	for (const ScopeList& type : types) {
		args.Add(type.ToString());
	}

	ErrorLog::Error(SymbolError(SymbolError::Function(func.ToString(), args), file));

	return Symbol();
}

Symbol Symbol::FindMethod(const ScopeList& func, const List<ScopeList>& types, const FileInfo& file) {
	Symbol s = Find(func, file);

	for (UInt i = 0; i < s.variants.Size(); i++) {
		if (s.variants[i].type == SymbolType::Method && s.variants[i].args.Size() == types.Size() + 1) {
			bool match = true;

			for (UInt a = 0; a < types.Size(); a++) {
				if (types[a] != FindNearest(func, s.variants[i].args[a + 1], file).scope) {
					match = false;
					break;
				}
			}

			if (match) {
				Symbol sym = s.variants[i];

				ScopeList list = func;
				list[list.Size() - 1].variant = i;

				sym.scope = list;
				return sym;
			}
		}
	}

	return Symbol();
}

Symbol Symbol::FindImplicitConversion(const ScopeList& from, const ScopeList& to, const FileInfo& file) {
	Symbol convert = FindExplicitConversion(from, to, file);

	if (convert.sign) {
		ErrorLog::Error(SymbolError("implicit convert error", file));
		return Symbol();
	}

	return convert;
}

Symbol Symbol::FindExplicitConversion(const ScopeList& from, const ScopeList& to, const FileInfo& file) {
	Symbol s = Find(from.Add(Scope::As), file);

	for (UInt i = 0; i < s.variants.Size(); i++) {
		if (
			s.variants[i].type == SymbolType::Function &&
			s.variants[i].args.Size() == 1 &&
			s.variants[i].ret.Size() == 1 &&
			FindNearest(from, s.variants[i].args[0], file).scope == from &&
			FindNearest(from, s.variants[i].ret[0], file).scope == to
		) {
			Symbol sym = s.variants[i];

			Scope as = Scope::As;
			as.variant = i;

			sym.scope = from.Add(as);
			return sym;
		}
	}

	s = Find(to.Add(Scope::As), file);

	for (UInt i = 0; i < s.variants.Size(); i++) {
		if (
			s.variants[i].type == SymbolType::Function &&
			s.variants[i].args.Size() == 1 &&
			s.variants[i].ret.Size() == 1 &&
			FindNearest(to, s.variants[i].args[0], file).scope == from &&
			FindNearest(to, s.variants[i].ret[0], file).scope == to
		) {
			Symbol sym = s.variants[i];

			Scope as = Scope::As;
			as.variant = i;

			sym.scope = to.Add(as);
			return sym;
		}
	}
	
	ErrorLog::Error(SymbolError("explicit convert error", file));
	return Symbol();
}

Mango Symbol::ToMango(const bool ignoreBasic) {
	Mango symbols = Mango("symbols", MangoType::List);
	
	for (const Pair<String, Symbol>& pair : Symbol::symbols.scopes) {
		if (!ignoreBasic || !pair.value.basic) {
			for (const Mango& m : pair.value.ToMangoList(ScopeList().Add(Scope(pair.key)))) {
				symbols.Add(m);
			}
		}
	}

	return symbols;
}

List<Mango> Symbol::ToMangoList(const ScopeList& scope) const {
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
		m.Add("type", GetType(FileInfo()).scope.ToString());

		bool onlyType = true;

		if (!attributes.IsEmpty()) {
			Mango attr = Mango(MangoType::List);

			for (const SymbolAttribute a : attributes) {
				switch (a) {
					case SymbolAttribute::Const: attr.Add(Mango(String("const"))); break;
					case SymbolAttribute::Copy: attr.Add(Mango(String("copy"))); break;
					case SymbolAttribute::Ref: attr.Add(Mango(String("ref"))); break;
					case SymbolAttribute::Nil: attr.Add(Mango(String("nil"))); break;
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

		for (UInt i = 0; i < ret.Size(); i++) {
			if (i > 0) r += ", ";
			r += GetReturnType(i).scope.ToString();
		}

		if (r.Size() > 0) r += " ";

		String a = "";

		for (UInt i = 0; i < args.Size(); i++) {
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
	
	for (const Pair<String, Symbol>& pair : scopes) {
		for (const Mango& m : pair.value.ToMangoList(scope.Add(Scope(pair.key)))) {
			symbols.Add(m);
		}
	}

	return symbols;
}

void Symbol::Setup() {
	// -------------- Integers --------------
	Map<Scope, Byte> integers;
	integers.Add(Scope::Byte, -1);
	integers.Add(Scope::UByte, 1);
	integers.Add(Scope::Short, -2);
	integers.Add(Scope::UShort, 2);
	integers.Add(Scope::Int, -4);
	integers.Add(Scope::UInt, 4);
	integers.Add(Scope::Long, -8);
	integers.Add(Scope::ULong, 8);

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

	for (const Pair<Scope, Byte> integer : integers) {
		// Type
		Symbol intSym = Symbol(SymbolType::Type);
		intSym.basic = true;
		intSym.size = integer.value < 0 ? -integer.value : integer.value;
		intSym.sign = integer.value < 0;

		// Min max
		/*
		Symbol min = Symbol(SymbolType::Variable);
		min.attributes.Add(SymbolAttribute::Static);
		min.attributes.Add(SymbolAttribute::Const);
		min.varType = ScopeList().Add(integer.key);
		*/

		// Neg
		Symbol neg = Symbol(SymbolType::Function);
		neg.node = new IntegerUnaryOperatorNode(abs(integer.value), InstructionType::Neg);

		if (integer.key.name[0] == 'u') {
			neg.args.Add(ScopeList().Add(Scope(integer.key.name.Sub(1))));
			neg.ret.Add(ScopeList().Add(Scope(integer.key.name.Sub(1))));
		}
		else {
			neg.args.Add(ScopeList().Add(integer.key));
			neg.ret.Add(ScopeList().Add(integer.key));
		}

		intSym.Add(Scope::Neg, neg, FileInfo());

		// Not
		Symbol bnot = Symbol(SymbolType::Function);
		bnot.node = new IntegerUnaryOperatorNode(abs(integer.value), InstructionType::Not);

		if (integer.key.name[0] == 'u') {
			bnot.args.Add(ScopeList().Add(Scope(integer.key.name.Sub(1))));
			bnot.ret.Add(ScopeList().Add(Scope(integer.key.name.Sub(1))));
		}
		else {
			bnot.args.Add(ScopeList().Add(integer.key));
			bnot.ret.Add(ScopeList().Add(integer.key));
		}

		intSym.Add(Scope::BitNot, bnot, FileInfo());

		// Binary operators
		for (const Pair<Scope, Byte> integer2 : integers) {
			Symbol intAssign = Symbol(SymbolType::Function);
			intAssign.args.Add(ScopeList().Add(Scope(integer2.key)));
			intAssign.node = new IntegerAssignNode(integer.value);
			intSym.Add(Scope::Assign, intAssign, FileInfo());

			for (const Pair<Scope, InstructionType>& op : binOps) {
				Symbol intOp = Symbol(SymbolType::Function);

				UByte v1 = integer.value < 0 ? -integer.value : integer.value;
				UByte v2 = integer2.value < 0 ? -integer2.value : integer2.value;
				bool sign = integer.value < 0 || integer2.value < 0;

				if (Instruction::IsComp(op.value))
					intOp.ret.Add(ScopeList().Add(Scope::Bool));
				else {
					const Scope name = v1 > v2 ? integer.key : integer2.key;

					if (sign && name.name[0] == 'u')
						intOp.ret.Add(ScopeList().Add(Scope(name.name.Sub(1))));
					else
						intOp.ret.Add(ScopeList().Add(name));
				}

				intOp.args.Add(ScopeList().Add(integer.key));
				intOp.args.Add(ScopeList().Add(integer2.key));
				intOp.node = new IntegerBinaryOperatorNode(
					v1 > v2 ? v1 : v2,
					sign,
					op.value
				);

				intSym.Add(op.key, intOp, FileInfo());
			}
		}

		symbols.Add(integer.key, intSym, FileInfo());
	}

	// ----------------- Boolean ---------------------
	Symbol boolSym = Symbol(SymbolType::Type);
	boolSym.size = 1;
	boolSym.sign = false;
	boolSym.basic = true;

	Symbol boolAssign = Symbol(SymbolType::Function);
	boolAssign.args.Add(ScopeList().Add(Scope::Bool));
	boolAssign.node = new BooleanAssignNode();
	boolSym.Add(Scope::Assign, boolAssign, FileInfo());

	Symbol boolToBool = Symbol(SymbolType::Function);
	boolToBool.args.Add(ScopeList().Add(Scope::Bool));
	boolToBool.ret.Add(ScopeList().Add(Scope::Bool));
	boolToBool.node = new BooleanToBooleanNode();
	boolSym.Add(Scope::As, boolToBool, FileInfo());

	symbols.Add(Scope::Bool, boolSym, FileInfo());
}