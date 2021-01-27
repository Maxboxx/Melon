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

const ScopeList ScopeList::Bool    = ScopeList(true).Add(Scope("bool"));
const ScopeList ScopeList::Byte    = ScopeList(true).Add(Scope("byte"));
const ScopeList ScopeList::UByte   = ScopeList(true).Add(Scope("ubyte"));
const ScopeList ScopeList::Short   = ScopeList(true).Add(Scope("short"));
const ScopeList ScopeList::UShort  = ScopeList(true).Add(Scope("ushort"));
const ScopeList ScopeList::Int     = ScopeList(true).Add(Scope("int"));
const ScopeList ScopeList::UInt    = ScopeList(true).Add(Scope("uint"));
const ScopeList ScopeList::Long    = ScopeList(true).Add(Scope("long"));
const ScopeList ScopeList::ULong   = ScopeList(true).Add(Scope("ulong"));
const ScopeList ScopeList::Huge    = ScopeList(true).Add(Scope("huge"));
const ScopeList ScopeList::Nil     = ScopeList(true).Add(Scope("nil"));
const ScopeList ScopeList::Discard = ScopeList(true).Add(Scope("_"));

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

	if (arguments) {
		scope += "(";

		bool first = true;

		for (const ScopeList& scopes : arguments.Get()) {
			if (!first) {
				scope += ",";
			}

			first = false;

			scope += scopes.ToString();
		}

		scope += ")";
	}

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

	if (arguments) {
		scope += "(";

		bool first = true;

		for (const ScopeList& scopes : arguments.Get()) {
			if (!first) {
				scope += ",";
			}

			first = false;

			scope += scopes.ToString();
		}

		scope += ")";
	}

	return scope;
}

Scope Scope::Copy() const {
	Scope scope = *this;
	if (types)     scope.types     = types.Get().Copy();
	if (arguments) scope.arguments = arguments.Get().Copy();
	return scope;
}

bool Scope::IsEmpty() const {
	return name.Size() == 0 && !types && !arguments;
}

bool Scope::operator==(const Scope& scope) const {
	if (name != scope.name) return false;
	if (types.HasValue() != scope.types.HasValue()) return false;
	if (arguments.HasValue() != scope.arguments.HasValue()) return false;

	if (types) {
		const List<ScopeList> types1 = types.Get();
		const List<ScopeList> types2 = scope.types.Get();

		if (types1.Size() != types2.Size()) return false;

		for (UInt i = 0; i < types1.Size(); i++) {
			if (types1[i] != types2[i]) return false;
		}
	}

	if (arguments) {
		const List<ScopeList> arguments1 = arguments.Get();
		const List<ScopeList> arguments2 = scope.arguments.Get();

		if (arguments1.Size() != arguments2.Size()) return false;

		for (UInt i = 0; i < arguments1.Size(); i++) {
			if (arguments1[i] != arguments2[i]) return false;
		}
	}

	return true;
}

bool Scope::operator!=(const Scope& scope) const {
	return !operator==(scope);
}

bool Scope::operator<(const Scope& scope) const {
	if (name < scope.name) return true;
	if (types.HasValue() != scope.types.HasValue()) return scope.types.HasValue();
	if (arguments.HasValue() != scope.arguments.HasValue()) return scope.arguments.HasValue();

	if (types) {
		const List<ScopeList> types1 = types.Get();
		const List<ScopeList> types2 = scope.types.Get();

		if (types1.Size() != types2.Size()) return types1.Size() < types2.Size();

		for (UInt i = 0; i < types1.Size(); i++) {
			if (types1[i] < types2[i]) return true;
		}
	}

	if (arguments) {
		const List<ScopeList> arguments1 = types.Get();
		const List<ScopeList> arguments2 = scope.types.Get();

		if (arguments1.Size() != arguments2.Size()) return arguments1.Size() < arguments2.Size();

		for (UInt i = 0; i < arguments1.Size(); i++) {
			if (arguments1[i] < arguments2[i]) return true;
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

ScopeList ScopeList::Split() const {
	ScopeList list;
	list.absolute = absolute;

	for (Boxx::UInt i = 0; i < Size(); i++) {
		Scope scope = scopes[i].Copy();

		if (scope.name.Size() > 0 && (scope.types || scope.arguments)) {
			list = list.Add(Scope(scope.name));
			scope.name = "";
		}
		
		if (scope.types) {
			for (ScopeList& type : scope.types.Get()) {
				type = type.Split();
			}
		}

		if (scope.arguments) {
			for (ScopeList& arg : scope.arguments.Get()) {
				arg = arg.Split();
			}
		}

		list = list.Add(scope);
	}

	return list;
}

bool ScopeList::IsTemplate() const {
	return scopes.Size() > 0 && scopes[0] == Scope();
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
	scopes = List<Scope>(scopeList.scopes.Size());
	baseScope = scopeList.baseScope;
	absolute = scopeList.absolute;

	for (const Scope& scope : scopeList.scopes) {
		scopes.Add(scope.Copy());
	}
}

const ScopeList ScopeList::undefined = ScopeList(true).Add(Scope("???"));