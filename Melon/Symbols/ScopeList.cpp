#include "ScopeList.h"

#include "Symbols.h"

#include "Boxx/Math.h"

using namespace Boxx;
using namespace Melon::Symbols;

const Scope Scope::Self   = Scope("self");
const Scope Scope::Init   = Scope("init");
const Scope Scope::Final  = Scope("final");
const Scope Scope::Super  = Scope("super");
const Scope Scope::This   = Scope("this");
const Scope Scope::Base   = Scope("base");
const Scope Scope::Global = Scope("global");

const Scope Scope::Add  = Scope("+");
const Scope Scope::Sub  = Scope("-");
const Scope Scope::Neg  = Scope("-");
const Scope Scope::Mul  = Scope("*");
const Scope Scope::Div  = Scope("/");
const Scope Scope::IDiv = Scope("//");
const Scope Scope::Sqr  = Scope("*");
const Scope Scope::Sqrt = Scope("/");
const Scope Scope::Mod  = Scope("%");

const Scope Scope::Not  = Scope("not");
const Scope Scope::Or   = Scope("or");
const Scope Scope::And  = Scope("and");
const Scope Scope::Xor  = Scope("xor");
const Scope Scope::Nor  = Scope("nor");
const Scope Scope::Nand = Scope("nand");
const Scope Scope::Xnor = Scope("xnor");

const Scope Scope::BitNot     = Scope("~");
const Scope Scope::BitOr      = Scope("|");
const Scope Scope::BitAnd     = Scope("&");
const Scope Scope::BitXor     = Scope("^");
const Scope Scope::BitNor     = Scope("~|");
const Scope Scope::BitNand    = Scope("~&");
const Scope Scope::BitXnor    = Scope("~^");
const Scope Scope::ShiftLeft  = Scope("<<");
const Scope Scope::ShiftRight = Scope(">>");

const Scope Scope::Equal        = Scope("==");
const Scope Scope::NotEqual     = Scope("~=");
const Scope Scope::LessEqual    = Scope("<=");
const Scope Scope::GreaterEqual = Scope(">=");
const Scope Scope::Less         = Scope("<");
const Scope Scope::Greater      = Scope(">");

const Scope Scope::Call   = Scope("()");
const Scope Scope::Assign = Scope("=");
const Scope Scope::Len    = Scope("#");
const Scope Scope::Index  = Scope("[]");

const Scope Scope::As = Scope("as");

const Scope Scope::TemplateSymbol = Scope("<template>");
const Scope Scope::Optional       = Scope("?");
const Scope Scope::Unwrap         = Scope("!");
const Scope Scope::Default        = Scope("??");
const Scope Scope::HasValue       = Scope("<hasvalue>");
const Scope Scope::Value          = Scope("<value>");

const ScopeList ScopeList::Bool   = ScopeList(true).Add(Scope("bool"));
const ScopeList ScopeList::Byte   = ScopeList(true).Add(Scope("byte"));
const ScopeList ScopeList::UByte  = ScopeList(true).Add(Scope("ubyte"));
const ScopeList ScopeList::Short  = ScopeList(true).Add(Scope("short"));
const ScopeList ScopeList::UShort = ScopeList(true).Add(Scope("ushort"));
const ScopeList ScopeList::Int    = ScopeList(true).Add(Scope("int"));
const ScopeList ScopeList::UInt   = ScopeList(true).Add(Scope("uint"));
const ScopeList ScopeList::Long   = ScopeList(true).Add(Scope("long"));
const ScopeList ScopeList::ULong  = ScopeList(true).Add(Scope("ulong"));
const ScopeList ScopeList::Huge   = ScopeList(true).Add(Scope("huge"));
const ScopeList ScopeList::Nil    = ScopeList(true).Add(Scope("nil"));

Scope::Scope() {
	
}

Scope::Scope(const String& name) {
	this->name = name;
}

Scope::~Scope() {

}

void Scope::AddScope(const String& scope) {
	if (scopes.Contains(scope)) {
		scopes[scope]++;
	}
	else {
		scopes.Add(scope, 0);
	}
}

UInt Scope::GetScope(const String& scope) const {
	return scopes[scope];
}

String Scope::ToString() const {
	String scope = name;

	if (types) {
		scope += "<";

		bool first = true;

		for (const ScopeList& scopes : types.Get()) {
			if (!first) {
				scope += ",";
			}

			first = false;

			scope += scopes.ToString();
		}

		scope += ">";
	}

	if (variant)
		return scope + ":" + String::ToString((Boxx::Int)variant.Get());

	return scope;
}

String Scope::ToSimpleString() const {
	if (name == Scope::Optional.name && !types.Get().IsEmpty()) return types.Get()[0].ToSimpleString() + "?";

	String scope = name;

	if (types) {
		scope += "<";

		bool first = true;

		for (const ScopeList& scopes : types.Get()) {
			if (!first) {
				scope += ",";
			}

			first = false;

			scope += scopes.ToString();
		}

		scope += ">";
	}

	if (variant)
		return scope + ":" + String::ToString((Boxx::Int)variant.Get());

	return scope;
}

Scope Scope::Copy() const {
	Scope scope = *this;
	if (types) scope.types = types.Get().Copy();
	return scope;
}

bool Scope::operator==(const Scope& scope) const {
	if (variant.HasValue() != scope.variant.HasValue()) return false;
	if (variant && variant.Get() != scope.variant.Get()) return false;
	return name == scope.name;
}

bool Scope::operator!=(const Scope& scope) const {
	return !operator==(scope);
}

bool Scope::operator<(const Scope& scope) const {
	if (name < scope.name) return true;
	
	if (name == scope.name) {
		if (variant.HasValue() && scope.variant.HasValue()) {
			return variant.Get() < scope.variant.Get();
		}
		else if (scope.variant) {
			return true;
		}
		else {
			return false;
		}
	}

	return false;
}

ScopeList::ScopeList() {

}

ScopeList::ScopeList(const bool absolute) {
	this->absolute = absolute;
}

ScopeList::ScopeList(const ScopeList& scopeList) {
	baseScope = scopeList.baseScope;
	absolute = scopeList.absolute;

	for (const Scope& scope : scopeList.scopes) {
		scopes.Add(scope.Copy());
	}
}

ScopeList::~ScopeList() {

}

ScopeList ScopeList::Add(const Scope& scope) const {
	ScopeList list = *this;
	list.scopes.Add(scope.Copy());
	return list;
}

ScopeList ScopeList::Add(const ScopeList& scopes) const {
	ScopeList list = *this;

	for (Boxx::UInt i = 0; i < scopes.Size(); i++)
		list.scopes.Add(scopes[i].Copy());

	return list;
}

ScopeList ScopeList::AddNext(const String& scope) const {
	ScopeList list = *this;

	if (list.scopes.IsEmpty()) {
		list.baseScope.AddScope(scope);
		Scope s = Scope("<" + scope + ":" + String::ToString(list.baseScope.GetScope(scope)) + ">");
		list.scopes.Add(s);
	}
	else {
		list.scopes.Last().AddScope(scope);
		Scope s = Scope("<" + scope + ":" + String::ToString((Boxx::Int)list.scopes.Last().GetScope(scope)) + ">");
		list.scopes.Add(s);
	}

	return list;
}

ScopeList ScopeList::Pop() const {
	ScopeList list = *this;
	list.scopes.RemoveAt(list.scopes.Size() - 1);
	return list;
}

Scope ScopeList::Last() const {
	return scopes.Last();
}

String ScopeList::ToString() const {
	String str = "";

	for (const Scope& scope : scopes) {
		if (str.Size() > 0) str += ".";
		str += scope.ToString();
	}

	return str;
}

String ScopeList::ToSimpleString() const {
	ScopeList list = *this;
	Boxx::UInt start = 0;

	if (absolute) {
		Symbol s = Symbol::symbols;

		for (Boxx::UInt i = 0; i < list.Size(); i++) {
			s = s.Get(list[i], FileInfo());

			switch (s.type) {
				case SymbolType::Function:
				case SymbolType::Method: {
					start = i;
					break;
				}

				case SymbolType::Struct: {
					if (list[i].types) {
						List<ScopeList> templateArgs;

						for (const ScopeList& arg : s.templateArgs) {
							templateArgs.Add(arg);
						}

						list[i].types = templateArgs;
						list[i].variant = nullptr;
					}
					
					break;
				}
			}
		}
	}

	String str = "";

	for (Boxx::UInt i = start; i < list.Size(); i++) {
		if (i > start) str += ".";
		str += list[i].ToSimpleString();
	}

	return str;
}

UInt ScopeList::Size() const {
	 return scopes.Size();
}

Scope ScopeList::operator[](const Boxx::UInt i) const {
	return scopes[i];
}

Scope& ScopeList::operator[](const Boxx::UInt i) {
	return scopes[i];
}

bool ScopeList::operator==(const ScopeList& scopeList) const {
	if (Size() != scopeList.Size()) return false;

	for (Boxx::UInt i = 0; i < Size(); i++) {
		if (scopes[i] != scopeList.scopes[i]) return false;
	}

	return true;
}

bool ScopeList::operator!=(const ScopeList& scopeList) const {
	return !operator==(scopeList);
}

bool ScopeList::operator<(const ScopeList& scopeList) const {
	if (Size() < scopeList.Size()) return true;
	if (Size() > scopeList.Size()) return false;

	for (Boxx::UInt i = 0; i < Size(); i++) {
		if ((*this)[i] < scopeList[i]) {
			return true;
		}
		else if ((*this)[i] != scopeList[i]) {
			return false;
		}
	}

	return false; 
}

void ScopeList::operator=(const ScopeList& scopeList) {
	scopes.Clear();
	baseScope = scopeList.baseScope;
	absolute = scopeList.absolute;

	for (const Scope& scope : scopeList.scopes) {
		scopes.Add(scope.Copy());
	}
}

const ScopeList ScopeList::undefined = ScopeList(true).Add(Scope("<undefined>"));