#include "NameList.h"

#include "Boxx/Math.h"

using namespace Boxx;
using namespace Melon::Symbols;

const Name Name::Self   = Name("self");
const Name Name::Init   = Name("init");
const Name Name::Final  = Name("final");
const Name Name::Super  = Name("super");
const Name Name::This   = Name("this");
const Name Name::Base   = Name("base");
const Name Name::Global = Name("global");

const Name Name::Add  = Name("+");
const Name Name::Sub  = Name("-");
const Name Name::Neg  = Name("-");
const Name Name::Mul  = Name("*");
const Name Name::Div  = Name("/");
const Name Name::IDiv = Name("//");
const Name Name::Sqr  = Name("*");
const Name Name::Sqrt = Name("/");
const Name Name::Mod  = Name("%");

const Name Name::Not  = Name("not");
const Name Name::Or   = Name("or");
const Name Name::And  = Name("and");
const Name Name::Xor  = Name("xor");
const Name Name::Nor  = Name("nor");
const Name Name::Nand = Name("nand");
const Name Name::Xnor = Name("xnor");

const Name Name::BitNot     = Name("~");
const Name Name::BitOr      = Name("|");
const Name Name::BitAnd     = Name("&");
const Name Name::BitXor     = Name("^");
const Name Name::BitNor     = Name("~|");
const Name Name::BitNand    = Name("~&");
const Name Name::BitXnor    = Name("~^");
const Name Name::ShiftLeft  = Name("<<");
const Name Name::ShiftRight = Name(">>");

const Name Name::Equal        = Name("==");
const Name Name::NotEqual     = Name("!=");
const Name Name::LessEqual    = Name("<=");
const Name Name::GreaterEqual = Name(">=");
const Name Name::Less         = Name("<");
const Name Name::Greater      = Name(">");

const Name Name::Call   = Name("()");
const Name Name::Assign = Name("=");
const Name Name::Len    = Name("#");
const Name Name::Index  = Name("[]");

const Name Name::As = Name("as");

const Name Name::TemplateSymbol = Name("<template>");
const Name Name::Optional       = Name("?");
const Name Name::Unwrap         = Name("!");
const Name Name::Default        = Name("??");
const Name Name::HasValue       = Name("<hasvalue>");
const Name Name::Value          = Name("<value>");
const Name Name::Array          = Name("[#]");
const Name Name::Length         = Name("<length>");
const Name Name::Items          = Name("<items>");

const NameList NameList::Bool    = NameList(true, Name("bool"));
const NameList NameList::Byte    = NameList(true, Name("byte"));
const NameList NameList::Tiny    = NameList(true, Name("tiny"));
const NameList NameList::UTiny   = NameList(true, Name("utiny"));
const NameList NameList::Short   = NameList(true, Name("short"));
const NameList NameList::UShort  = NameList(true, Name("ushort"));
const NameList NameList::Int     = NameList(true, Name("int"));
const NameList NameList::UInt    = NameList(true, Name("uint"));
const NameList NameList::Long    = NameList(true, Name("long"));
const NameList NameList::ULong   = NameList(true, Name("ulong"));
const NameList NameList::Huge    = NameList(true, Name("huge"));
const NameList NameList::Char    = NameList(true, Name("char"));
const NameList NameList::Nil     = NameList(true, Name("nil"));
const NameList NameList::Discard = NameList(true, Name("_"));
const NameList NameList::Any     = NameList(true, Name("any"));

Name::Name() {
	
}

Name::Name(const String& name) {
	this->name = name;
}

Name::~Name() {

}

void Name::AddScope(const String& scope) {
	if (scopes.Contains(scope)) {
		scopes[scope]++;
	}
	else {
		scopes.Add(scope, 0);
	}
}

UInt Name::GetScope(const String& scope) const {
	return scopes[scope];
}

String Name::ToString() const {
	String scope = name;

	if (types) {
		scope += "<";

		bool first = true;

		for (const NameList& scopes : *types) {
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

		for (const NameList& scopes : *arguments) {
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

String Name::ToSimpleString() const {
	if (name == Name::Optional.name && types && !types->IsEmpty()) return types.Value()[0].ToSimpleString() + "?";

	String scope = name;

	if (types) {
		scope += "<";

		bool first = true;

		for (const NameList& scopes : *types) {
			if (!first) {
				scope += ", ";
			}

			first = false;

			scope += scopes.ToSimpleString();
		}

		scope += ">";
	}

	if (arguments) {
		scope += "(";

		bool first = true;

		for (const NameList& scopes : *arguments) {
			if (!first) {
				scope += ", ";
			}

			first = false;

			scope += scopes.ToSimpleString();
		}

		scope += ")";
	}

	return scope;
}

Name Name::Copy() const {
	Name scope = *this;
	if (types)     scope.types     = types->Copy();
	if (arguments) scope.arguments = arguments->Copy();
	return scope;
}

bool Name::IsEmpty() const {
	return name.IsEmpty() && !types && !arguments;
}

bool Name::operator==(const Name& scope) const {
	if (name != scope.name) return false;
	if (types.HasValue() != scope.types.HasValue()) return false;
	if (arguments.HasValue() != scope.arguments.HasValue()) return false;

	if (types) {
		const List<NameList> types1 = *types;
		const List<NameList> types2 = *scope.types;

		if (types1.Count() != types2.Count()) return false;

		for (UInt i = 0; i < types1.Count(); i++) {
			if (types1[i] != types2[i]) return false;
		}
	}

	if (arguments) {
		const List<NameList> arguments1 = *arguments;
		const List<NameList> arguments2 = *scope.arguments;

		if (arguments1.Count() != arguments2.Count()) return false;

		for (UInt i = 0; i < arguments1.Count(); i++) {
			if (arguments1[i] != arguments2[i]) return false;
		}
	}

	return true;
}

bool Name::operator!=(const Name& scope) const {
	return !operator==(scope);
}

bool Name::operator<(const Name& scope) const {
	if (name < scope.name) return true;
	if (types.HasValue() != scope.types.HasValue()) return scope.types.HasValue();
	if (arguments.HasValue() != scope.arguments.HasValue()) return scope.arguments.HasValue();

	if (types) {
		const List<NameList> types1 = *types;
		const List<NameList> types2 = *scope.types;

		if (types1.Count() != types2.Count()) return types1.Count() < types2.Count();

		for (UInt i = 0; i < types1.Count(); i++) {
			if (types1[i] < types2[i]) return true;
		}
	}

	if (arguments) {
		const List<NameList> arguments1 = *types;
		const List<NameList> arguments2 = *scope.types;

		if (arguments1.Count() != arguments2.Count()) return arguments1.Count() < arguments2.Count();

		for (UInt i = 0; i < arguments1.Count(); i++) {
			if (arguments1[i] < arguments2[i]) return true;
		}
	}

	return false;
}

NameList::NameList() {

}

NameList::NameList(const bool absolute) {
	this->absolute = absolute;
}

NameList::NameList(const Name& name) {
	names.Add(name);
}

NameList::NameList(const Name& name1, const Name& name2) {
	names.Add(name1);
	names.Add(name2);
}

NameList::NameList(const Name& name1, const Name& name2, const Name& name3) {
	names.Add(name1);
	names.Add(name2);
	names.Add(name3);
}

NameList::NameList(const bool absolute, const Name& name) {
	this->absolute = true;
	names.Add(name);
}

NameList::NameList(const bool absolute, const Name& name1, const Name& name2) {
	this->absolute = true;
	names.Add(name1);
	names.Add(name2);
}

NameList::NameList(const bool absolute, const Name& name1, const Name& name2, const Name& name3) {
	this->absolute = true;
	names.Add(name1);
	names.Add(name2);
	names.Add(name3);
}

NameList::NameList(const NameList& scopeList) {
	baseName = scopeList.baseName;
	absolute = scopeList.absolute;

	for (const Name& scope : scopeList.names) {
		names.Add(scope.Copy());
	}
}

NameList::~NameList() {

}

NameList NameList::Add(const Name& scope) const {
	NameList list = *this;
	list.names.Add(scope.Copy());
	return list;
}

NameList NameList::Add(const NameList& scopes) const {
	NameList list = *this;

	for (Boxx::UInt i = 0; i < scopes.Size(); i++)
		list.names.Add(scopes[i].Copy());

	return list;
}

NameList NameList::AddNext(const String& scope) const {
	NameList list = *this;

	if (list.names.IsEmpty()) {
		list.baseName.AddScope(scope);
		Name s = Name("<" + scope + ":" + String::ToString(list.baseName.GetScope(scope)) + ">");
		list.names.Add(s);
	}
	else {
		list.names.Last().AddScope(scope);
		Name s = Name("<" + scope + ":" + String::ToString((Boxx::Int)list.names.Last().GetScope(scope)) + ">");
		list.names.Add(s);
	}

	return list;
}

NameList NameList::Pop() const {
	NameList list = *this;
	list.names.RemoveAt(list.names.Count() - 1);
	return list;
}

Name NameList::Last() const {
	return names.Last();
}

String NameList::ToString() const {
	String str = "";

	for (const Name& scope : names) {
		if (str.Length() > 0) str += ".";
		str += scope.ToString();
	}

	return str;
}

String NameList::ToSimpleString() const {
	if (IsTemplate()) return names[1].ToSimpleString();

	NameList list = *this;
	Boxx::UInt start = 0;

	String str = "";

	for (Boxx::UInt i = start; i < list.Size(); i++) {
		if (i > start && list[i].name.Length() > 0) {
			str += ".";
		}

		str += list[i].ToSimpleString();
	}

	return str;
}

UInt NameList::Size() const {
	 return names.Count();
}

NameList NameList::Split() const {
	NameList list;
	list.absolute = absolute;

	for (Boxx::UInt i = 0; i < Size(); i++) {
		Name scope = names[i].Copy();

		if (scope.name.Length() > 0 && (scope.types || scope.arguments)) {
			list = list.Add(Name(scope.name));
			scope.name = "";
		}
		
		if (scope.types) {
			for (NameList& type : *scope.types) {
				type = type.Split();
			}
		}

		if (scope.arguments) {
			for (NameList& arg : *scope.arguments) {
				arg = arg.Split();
			}
		}

		list = list.Add(scope);
	}

	return list;
}

bool NameList::IsTemplate() const {
	return names.Count() > 0 && names[0] == Name();
}

bool NameList::HasTemplates() const {
	for (const Name& name : names) {
		if (name.types) return true;
	}

	return false;
}

Name NameList::operator[](const Boxx::UInt i) const {
	return names[i];
}

Name& NameList::operator[](const Boxx::UInt i) {
	return names[i];
}

bool NameList::operator==(const NameList& scopeList) const {
	if (absolute != scopeList.absolute) return false;
	if (Size() != scopeList.Size()) return false;

	for (Boxx::UInt i = 0; i < Size(); i++) {
		if (names[i] != scopeList.names[i]) return false;
	}

	return true;
}

bool NameList::operator!=(const NameList& scopeList) const {
	return !operator==(scopeList);
}

bool NameList::operator<(const NameList& scopeList) const {
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

void NameList::operator=(const NameList& scopeList) {
	names = List<Name>(scopeList.names.Count());
	baseName = scopeList.baseName;
	absolute = scopeList.absolute;

	for (const Name& scope : scopeList.names) {
		names.Add(scope.Copy());
	}
}

const NameList NameList::undefined = NameList(true).Add(Name("???"));