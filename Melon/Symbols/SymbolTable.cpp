#include "SymbolTable.h"

#include "TemplateSymbol.h"
#include "TemplateTypeSymbol.h"
#include "FunctionSymbol.h"
#include "NamespaceSymbol.h"
#include "IntegerSymbol.h"
#include "VariableSymbol.h"
#include "StructSymbol.h"
#include "ClassSymbol.h"
#include "PtrSymbol.h"

#include "Nodes/SymbolNode.h"
#include "Nodes/IntegerUnaryOperatorNode.h"
#include "Nodes/IntegerBinaryOperatorNode.h"
#include "Nodes/IntegerToBoolNode.h"
#include "Nodes/IntegerNotNode.h"
#include "Nodes/IntegerAssignNode.h"
#include "Nodes/IntegerConvertNode.h"
#include "Nodes/BooleanAssignNode.h"
#include "Nodes/BooleanBinaryOperatorNode.h"
#include "Nodes/BooleanNotNode.h"
#include "Nodes/BooleanConstantNode.h"
#include "Nodes/EmptySymbolNode.h"
#include "Nodes/OptionalAssignNode.h"
#include "Nodes/OptionalAssignValueNode.h"
#include "Nodes/OptionalUnwrapNode.h"
#include "Nodes/OptionalToBooleanNode.h"
#include "Nodes/OptionalNotNode.h"
#include "Nodes/PtrAssignNode.h"
#include "Nodes/PtrUnwrapNode.h"
#include "Nodes/IndexGetNode.h"
#include "Nodes/IndexSetNode.h"
#include "Nodes/ListAddNode.h"
#include "Nodes/LengthNode.h"
#include "Nodes/StructAssignNode.h"
#include "Nodes/StringConcatNode.h"

#include "Kiwi/Old/Kiwi.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

Pointer<ScopeSymbol> SymbolTable::symbols = new ScopeSymbol(FileInfo());
List<SymbolTable::TemplateInfo> SymbolTable::templateSymbols;

Symbol* SymbolTable::FindAbsolute(const NameList& name, const FileInfo& file) {
	if (name.Size() > 0 && name[0] == Name::Global) {
		NameList list;

		for (Boxx::UInt i = 1; i < name.Size(); i++) {
			list = list.Add(name[i]);
		}

		return symbols->Symbol::Find(list, file);
	}

	return symbols->Symbol::Find(name, file);
}

Symbol* SymbolTable::ContainsAbsolute(const NameList& name) {
	if (name.Size() > 0 && name[0] == Name::Global) {
		NameList list;

		for (Boxx::UInt i = 1; i < name.Size(); i++) {
			list = list.Add(name[i]);
		}

		return symbols->Symbol::Contains(list);
	}

	return symbols->Symbol::Contains(name);
}

Symbol* SymbolTable::Contains(const Name& name, const NameList& scope, const FileInfo& file, const SearchOptions options) {
	return Contains(NameList().Add(name), scope, file, options);
}

Symbol* SymbolTable::Contains(const NameList& name, const NameList& scope, const FileInfo& file, const SearchOptions options) {
	ErrorLog::AddMarker();
	Symbol* const sym = Find(name, scope, file, options);
	ErrorLog::Revert();

	return sym;
}

Symbol* SymbolTable::Find(const Name& name, const NameList& scope, const FileInfo& file, const SearchOptions options) {
	return Find(NameList().Add(name), scope, file, options);
}

Symbol* SymbolTable::Find(NameList name, NameList scope, const FileInfo& file, const SearchOptions options) {
	if ((options & SearchOptions::ReplaceTemplates) != SearchOptions::None) {
		scope = ReplaceTemplatesAbsolute(scope, file);
		name  = ReplaceTemplates(name, scope, file);
	}

	if (name.absolute || (name.Size() > 0 && name[0] == Name::Global)) {
		return FindAbsolute(name, file);
	}

	for (Boxx::UInt i = 0; i < name.Size(); i++) {
		if (name[i].types) {
			for (NameList& type : *name[i].types) {
				if (Symbol* const s = Find(type, scope, file)) {
					type = s->AbsoluteName();
				}
			}
		}
	}

	const bool ignoreOrder = (options & SearchOptions::IgnoreOrder) != SearchOptions::None; 

	if (scope.Size() > 0) {
		NameList list = scope;

		while (list.Size() > 0) {
			Symbol* const s = FindInNamespaces(list, file);

			if (s) {
				if (Symbol* const sym = s->Contains(name)) {
					if (ignoreOrder || sym->file.statement < file.statement || sym->Is<TemplateSymbol>() || sym->Is<FunctionSymbol>()) {
						return sym;
					}
				}
			}

			list = list.Pop();
		}
	}

	return FindInNamespaces(name, file);
}

Symbol* SymbolTable::FindInNamespaces(const NameList& name, const FileInfo& file) {
	if (name.absolute || (name.Size() > 0 && name[0] == Name::Global)) {
		return FindAbsolute(name, file);
	}

	if (file.currentNamespace.Size() > 0) {
		if (Symbol* const sym = ContainsAbsolute(file.currentNamespace.Add(name))) {
			if (FindAbsolute<NamespaceSymbol>(file.currentNamespace.Add(name[0]), file)) {
				return sym;
			}
		}
	}

	Symbol* foundSymbol = nullptr;
	Symbol* namespaceSymbol = FindAbsolute(file.currentNamespace, file);

	while (namespaceSymbol) {
		if (foundSymbol = namespaceSymbol->Contains(name)) {
			break;
		}

		namespaceSymbol = namespaceSymbol->Parent();
	}

	if (!foundSymbol) {
		if (file.currentNamespace.Size() > 0 && file.currentNamespace[0] == name[0]) {
			foundSymbol = ContainsAbsolute(name);
		}

		if (!foundSymbol) {
			if (Symbol* const sym = ContainsAbsolute(name)) {
				if (FindAbsolute<NamespaceSymbol>(NameList().Add(name[0]), file)) {
					foundSymbol = sym;
				}
			}
		}
	}

	bool hasMatch = false;

	// TODO: Only allow static stuff to be included
	for (const NameList& includedNamespace : file.includedNamespaces) {
		if (includedNamespace.Last() != name[0]) continue;

		if (Symbol* const s = ContainsAbsolute(includedNamespace)) {
			if (hasMatch) {
				ErrorLog::Error(LogMessage("error.symbol.ambiguous", name[0].ToSimpleString()), file);
			}

			hasMatch = true;

			Symbol* ns = s;

			bool found = true;

			for (Boxx::UInt i = 1; i < name.Size(); i++) {
				Symbol* const sym = ns->Find(name[i], file);

				if (i + 1 >= name.Size() || sym->Is<MapSymbol>()) {
					ns = sym;
				}
				else {
					found = false;
					break;
				}
			}

			if (found) {
				foundSymbol = ns;
			}
		}
	}

	if (foundSymbol) return foundSymbol;

	ErrorLog::Error(LogMessage("error.symbol.not_found", name.ToSimpleString()), file);
	return nullptr;
}

FunctionSymbol* SymbolTable::FindOperator(const Name& op, TypeSymbol* const type1, TypeSymbol* const type2, const FileInfo& file) {
	FunctionSymbol* const f1 = type1->Contains<FunctionSymbol>(op);
	FunctionSymbol* f = nullptr;
	bool isAmbig = false;

	if (f1) for (FunctionSymbol* const overload : f1->overloads) {
		if (overload->ArgumentType(0) != type1) continue;

		TypeSymbol* const arg = overload->ArgumentType(1);
		if (arg == type2) return overload;
		
		if (type2->ImplicitConversionTo(arg)) {
			if (f) {
				isAmbig = true;
			}

			f = overload;
		}
	}

	if (isAmbig) {
		// TODO: Ambig error
		return nullptr;
	}

	if (f) return f;

	FunctionSymbol* const f2 = type2->Contains<FunctionSymbol>(op);
	isAmbig = false;

	if (f2) for (FunctionSymbol* const overload : f2->overloads) {
		if (overload->ArgumentType(1) != type2) continue;

		TypeSymbol* const arg = overload->ArgumentType(0);
		if (arg == type1) return overload;

		if (type1->ImplicitConversionTo(arg)) {
			if (f) {
				isAmbig = true;
			}

			f = overload;
		}
	}

	if (isAmbig) {
		// TODO: Ambig error
		return nullptr;
	}

	if (f) return f;
	
	// TODO: error
	return nullptr;
}

FunctionSymbol* SymbolTable::FindImplicitConversion(TypeSymbol* const from, TypeSymbol* const to, const FileInfo& file, bool logErrors) {
	if (!from || !to) return nullptr;

	if (FunctionSymbol* const op = from->ImplicitConversionTo(to)) {
		return op;
	}

	if (FunctionSymbol* const op = to->ImplicitConversionFrom(from)) {
		return op;
	}

	if (logErrors) {
		ErrorLog::Error(LogMessage("error.symbol.implicit", from->AbsoluteName().ToSimpleString(), to->AbsoluteName().ToSimpleString()), file);
	}

	return nullptr;
}

FunctionSymbol* SymbolTable::FindExplicitConversion(TypeSymbol* const from, TypeSymbol* const to, const FileInfo& file, bool logErrors) {
	if (!from || !to) return nullptr;

	if (FunctionSymbol* const op = from->ExplicitConversionTo(to)) {
		return op;
	}

	if (FunctionSymbol* const op = to->ExplicitConversionFrom(from)) {
		return op;
	}

	if (logErrors) {
		ErrorLog::Error(LogMessage("error.symbol.explicit", from->AbsoluteName().ToSimpleString(), to->AbsoluteName().ToSimpleString()), file);
	}

	return nullptr;
}

void SymbolTable::SpecializeTemplate(const NameList& name, Symbol* const scope, const FileInfo& file) {
	TemplateInfo info;
	info.name  = name;
	info.scope = scope;
	info.file  = file;
	templateSymbols.Add(info);
}

NameList SymbolTable::ReplaceTemplatesAbsolute(const NameList& name, const FileInfo& file) {
	NameList list;
	list.absolute = true;

	for (Boxx::UInt i = 0; i < name.Size(); i++) {
		Name scope = name[i].Copy();
		
		if (scope.types) {
			Symbol* const s = FindAbsolute(list.Add(scope), FileInfo());
			List<NameList> args;

			if (i < name.Size() - 1) {
				if (TemplateSymbol* const t = s->Contains<TemplateSymbol>(name[i + 1])) {
					if (TypeSymbol* const type = t->Type()) {
						if (type != t) {
							list = ReplaceTemplatesAbsolute(type->AbsoluteName(), file);
						}
						else {
							list = type->AbsoluteName();
						}

						i++;
						continue;
					}
				}
			}

			Optional<List<NameList>> templateArgs = nullptr;

			if (TemplateTypeSymbol* const tts = s->Cast<TemplateTypeSymbol>()) {
				templateArgs = tts->templateArguments;
			}
			else if (FunctionSymbol* const fs = s->Cast<FunctionSymbol>()) {
				templateArgs = fs->templateArguments;
			}

			if (templateArgs) {
				for (const NameList& arg : *templateArgs) {
					if (arg.Size() > 0 && arg[0] == Name()) {
						if (TemplateSymbol* const t = s->Contains<TemplateSymbol>(arg[1])) {
							if (TypeSymbol* const type = t->Type()) {
								if (type != t) {
									args.Add(ReplaceTemplatesAbsolute(list.Add(scope).Add(arg[1]), file));
									continue;
								}
							}
						}

						args.Add(arg);
					}
					else {
						args.Add(ReplaceTemplatesAbsolute(arg, file));
					}
				}

				Name newName = scope.Copy();

				newName.types = args;

				if (Symbol* const sym = ContainsAbsolute(list.Add(newName))) {
					scope = sym->Name();
				}
				else {
					scope = newName;
				}
			}
		}
		else if (scope.arguments) {
			FunctionSymbol* const s = FindAbsolute<FunctionSymbol>(list.Add(scope), FileInfo());

			if (s && s->replace) {
				scope.arguments = s->replace->AbsoluteName().Last().arguments;
			}
		}

		list = list.Add(scope);
	}

	if (TemplateSymbol* const type = FindAbsolute<TemplateSymbol>(list, file)) {
		if (TypeSymbol* const t = type->Type()) {
			return t->AbsoluteName();
		}
	}

	return list;
}

NameList SymbolTable::ReplaceTemplates(const NameList& name, const NameList& scope, const FileInfo& file) {
	NameList typeName = name;

	for (Boxx::UInt i = 0; i < typeName.Size(); i++) {
		if (typeName[i].types) {
			for (NameList& type : *typeName[i].types) {
				NameList temp = type;

				if (temp.IsTemplate()) {
					temp = typeName;

					while (temp.Size() > i + 1) {
						temp = temp.Pop();
					}

					temp = temp.Add(type[1]);
				}

				if (Symbol* const sym = Find(temp, scope, file, temp == name ? SymbolTable::SearchOptions::None : SymbolTable::SearchOptions::ReplaceTemplates)) {
					NameList absolute = ReplaceTemplatesAbsolute(sym->AbsoluteName(), file);
					
					if (temp != absolute) {
						if (TemplateSymbol* const t = Contains<TemplateSymbol>(type, scope, file)) {
							if (TypeSymbol* const templateType = t->Type()) {
								if (!templateType->Is<TemplateSymbol>()) {
									type = templateType->AbsoluteName();
								}
							}
						}
						else {
							type = absolute;
						}
					}
				}
			}
		}
		else if (typeName[i].arguments) {
			NameList list;

			for (Boxx::UInt u = 0; u < i; u++) {
				list = list.Add(typeName[u]);
			}

			if (FunctionSymbol* const s = Find<FunctionSymbol>(list.Add(typeName[i]), scope, file)) {
				if (s && s->replace) {
					typeName[i].arguments = s->replace->AbsoluteName().Last().arguments;
				}
			}
		}
	}

	return typeName;
}

void SymbolTable::Setup() {
	SetupIntegers();
	SetupChars();
	SetupBoolean();
	SetupNil();
	SetupString();
	SetupOptional();
	SetupPointer();
	SetupArray();
	SetupList();
}

IntegerSymbol* SymbolTable::Byte   = nullptr;
IntegerSymbol* SymbolTable::Tiny   = nullptr;
IntegerSymbol* SymbolTable::UTiny  = nullptr;
IntegerSymbol* SymbolTable::Short  = nullptr;
IntegerSymbol* SymbolTable::UShort = nullptr;
IntegerSymbol* SymbolTable::Int    = nullptr;
IntegerSymbol* SymbolTable::UInt   = nullptr;
IntegerSymbol* SymbolTable::Long   = nullptr;
IntegerSymbol* SymbolTable::ULong  = nullptr;

void SymbolTable::SetupIntegers() {
	Map<NameList, Boxx::Byte> integers;
	integers.Add(NameList::Tiny, -1);
	integers.Add(NameList::Byte, 1);
	integers.Add(NameList::Short, -2);
	integers.Add(NameList::UShort, 2);
	integers.Add(NameList::Int, -4);
	integers.Add(NameList::UInt, 4);
	integers.Add(NameList::Long, -8);
	integers.Add(NameList::ULong, 8);

	Map<NameList, IntegerSymbol*> intSymbols;

	Map<Name, bool> binOps;
	binOps.Add(Name::Add, false);
	binOps.Add(Name::Sub, false);
	binOps.Add(Name::Mul, false);
	binOps.Add(Name::IDiv, false);
	binOps.Add(Name::Mod, false);
	binOps.Add(Name::BitOr, false);
	binOps.Add(Name::BitAnd, false);
	binOps.Add(Name::BitXor, false);
	binOps.Add(Name::BitNor, false);
	binOps.Add(Name::BitNand, false);
	binOps.Add(Name::BitXnor, false);
	binOps.Add(Name::ShiftLeft, false);
	binOps.Add(Name::ShiftRight, false);
	binOps.Add(Name::Equal, true);
	binOps.Add(Name::NotEqual, true);
	binOps.Add(Name::LessEqual, true);
	binOps.Add(Name::GreaterEqual, true);
	binOps.Add(Name::Less, true);
	binOps.Add(Name::Greater, true);

	for (const Pair<NameList, Boxx::Byte> integer : integers) {
		List<Name> aliases;

		if (integer.key == NameList::Byte) {
			aliases.Add(NameList::UTiny[0]);
		}

		// Type
		IntegerSymbol* const intSym = symbols->AddSymbol(integer.key[0], aliases, new IntegerSymbol(
			integer.value < 0 ? -integer.value : integer.value,
			integer.value < 0,
			FileInfo()
		));

		intSymbols.Add(integer.key, intSym);

		// Min max
		VariableSymbol* const min = intSym->AddSymbol(Name("min"), new VariableSymbol(FileInfo()));
		min->modifiers = VariableModifiers::Const | VariableModifiers::Static;
		min->type = intSym->AbsoluteName();

		VariableSymbol* const max = intSym->AddSymbol(Name("max"), new VariableSymbol(FileInfo()));
		max->modifiers = VariableModifiers::Const | VariableModifiers::Static;
		max->type = intSym->AbsoluteName();

		// Neg
		FunctionSymbol* const neg  = intSym->AddSymbol(Name::Neg, new FunctionSymbol(FileInfo()));
		FunctionSymbol* const neg1 = neg->AddOverload(new FunctionSymbol(FileInfo()));
		neg1->symbolNode = new IntegerUnaryOperatorNode(Name::Neg);

		if (integer.value > 0) {
			neg1->arguments.Add(NameList(true).Add(Name(integer.key[0].name.Sub(1))));
			neg1->returnValues.Add(NameList(true).Add(Name(integer.key[0].name.Sub(1))));
		}
		else {
			neg1->arguments.Add(integer.key);
			neg1->returnValues.Add(integer.key);
		}

		// Bit Not
		FunctionSymbol* const bnot  = intSym->AddSymbol(Name::BitNot, new FunctionSymbol(FileInfo()));
		FunctionSymbol* const bnot1 = bnot->AddOverload(new FunctionSymbol(FileInfo()));
		bnot1->symbolNode = new IntegerUnaryOperatorNode(Name::BitNot);
		bnot1->arguments.Add(integer.key);
		bnot1->returnValues.Add(integer.key);

		// To Bool
		FunctionSymbol* const toBool  = intSym->AddSymbol(Name::As, new FunctionSymbol(FileInfo()));
		FunctionSymbol* const toBool1 = toBool->AddOverload(new FunctionSymbol(FileInfo()));
		toBool1->symbolNode = new IntegerToBoolNode();
		toBool1->arguments.Add(integer.key);
		toBool1->returnValues.Add(NameList::Bool);
		toBool1->isExplicit = true;

		// Not
		FunctionSymbol* const intNot  = intSym->AddSymbol(Name::Not, new FunctionSymbol(FileInfo()));
		FunctionSymbol* const intNot1 = intNot->AddOverload(new FunctionSymbol(FileInfo()));
		intNot1->symbolNode = new IntegerNotNode();
		intNot1->arguments.Add(integer.key);
		intNot1->returnValues.Add(NameList::Bool);

		// Assign
		FunctionSymbol* const assign  = intSym->AddSymbol(Name::Assign, new FunctionSymbol(FileInfo()));
		FunctionSymbol* const assign1 = assign->AddOverload(new FunctionSymbol(FileInfo()));
		assign1->symbolNode = new IntegerAssignNode();
		assign1->arguments.Add(integer.key);
	}

	Kiwi::Type boolType = intSymbols[NameList::Byte]->KiwiType();

	// Binary operators
	for (const Pair<NameList, Boxx::Byte>& integer : integers) {
		IntegerSymbol* const intSym = intSymbols[integer.key];

		for (const Pair<NameList, Boxx::Byte>& integer2 : integers) {
			IntegerSymbol* const intSym2 = intSymbols[integer2.key];

			FunctionSymbol* const convert1 = intSym->Contains<FunctionSymbol>(Name::As)->AddOverload(new FunctionSymbol(FileInfo()));
			convert1->arguments.Add(integer.key);
			convert1->returnValues.Add(integer2.key);
			convert1->symbolNode = new IntegerConvertNode(intSym2->KiwiType());
			convert1->isExplicit = false;

			for (const Pair<Name, bool>& op : binOps) {
				FunctionSymbol* binOp = intSym->Contains<FunctionSymbol>(op.key);
				if (!binOp) binOp = intSym->AddSymbol(op.key, new FunctionSymbol(FileInfo()));

				FunctionSymbol* const binOp1 = binOp->AddOverload(new FunctionSymbol(FileInfo()));

				const Boxx::UByte v1 = integer.value < 0 ? -integer.value : integer.value;
				const Boxx::UByte v2 = integer2.value < 0 ? -integer2.value : integer2.value;
				const bool sign = integer.value < 0 || integer2.value < 0;

				if (op.value) {
					binOp1->returnValues.Add(NameList::Bool);
				}
				else {
					const NameList name = v1 > v2 ? integer.key : integer2.key;

					if (sign && name[0].name[0] == 'u') {
						binOp1->returnValues.Add(NameList(true).Add(Name(name[0].name.Sub(1))));
					}
					else {
						binOp1->returnValues.Add(name);
					}
				}

				binOp1->arguments.Add(integer.key);
				binOp1->arguments.Add(integer2.key);

				binOp1->symbolNode = new IntegerBinaryOperatorNode(op.key, op.value ? boolType : (v1 > v2 ? intSym->KiwiType() : intSym2->KiwiType()));
			}
		}
	}

	SymbolTable::Tiny   = intSymbols[NameList::Tiny];
	SymbolTable::UTiny  = intSymbols[NameList::Byte];
	SymbolTable::Byte   = intSymbols[NameList::Byte];
	SymbolTable::Short  = intSymbols[NameList::Short];
	SymbolTable::UShort = intSymbols[NameList::UShort];
	SymbolTable::Int    = intSymbols[NameList::Int];
	SymbolTable::UInt   = intSymbols[NameList::UInt];
	SymbolTable::Long   = intSymbols[NameList::Long];
	SymbolTable::ULong  = intSymbols[NameList::ULong];
}

IntegerSymbol* SymbolTable::Char = nullptr;

void SymbolTable::SetupChars() {
	IntegerSymbol* const charSym = symbols->AddSymbol(NameList::Char[0], new IntegerSymbol(1, false, FileInfo()));

	FunctionSymbol* const assign  = charSym->AddSymbol(Name::Assign, new FunctionSymbol(FileInfo()));
	FunctionSymbol* const assign1 = assign->AddOverload(new FunctionSymbol(FileInfo()));
	assign1->symbolNode = new IntegerAssignNode();
	assign1->arguments.Add(NameList::Char);

	SymbolTable::Char = charSym;
}

IntegerSymbol* SymbolTable::Bool = nullptr;

void SymbolTable::SetupBoolean() {
	IntegerSymbol* const boolSym = symbols->AddSymbol(NameList::Bool[0], new IntegerSymbol(1, false, FileInfo()));

	FunctionSymbol* const assign  = boolSym->AddSymbol(Name::Assign, new FunctionSymbol(FileInfo()));
	FunctionSymbol* const assign1 = assign->AddOverload(new FunctionSymbol(FileInfo()));
	assign1->symbolNode = new BooleanAssignNode();
	assign1->arguments.Add(NameList::Bool);

	List<Name> binOps;
	binOps.Add(Name::BitOr);
	binOps.Add(Name::BitAnd);
	binOps.Add(Name::BitXor);
	binOps.Add(Name::Equal);
	binOps.Add(Name::NotEqual);

	for (const Name& op : binOps) {
		FunctionSymbol* const func  = boolSym->AddSymbol(op, new FunctionSymbol(FileInfo()));
		FunctionSymbol* const func1 = func->AddOverload(new FunctionSymbol(FileInfo()));
		func1->symbolNode = new BooleanBinaryOperatorNode(op);
		func1->arguments.Add(NameList::Bool);
		func1->arguments.Add(NameList::Bool);
		func1->returnValues.Add(NameList::Bool);
	}

	FunctionSymbol* const boolNot  = boolSym->AddSymbol(Name::Not, new FunctionSymbol(FileInfo()));
	FunctionSymbol* const boolNot1 = boolNot->AddOverload(new FunctionSymbol(FileInfo()));
	boolNot1->symbolNode = new BooleanNotNode();
	boolNot1->arguments.Add(NameList::Bool);
	boolNot1->returnValues.Add(NameList::Bool);

	SymbolTable::Bool = boolSym;
}

TypeSymbol* SymbolTable::Nil = nullptr;

void SymbolTable::SetupNil() {
	TypeSymbol* const nilSym = symbols->AddSymbol(NameList::Nil[0], new IntegerSymbol(0, false, FileInfo()));

	FunctionSymbol* const nilNot  = nilSym->AddSymbol(Name::Not, new FunctionSymbol(FileInfo()));
	FunctionSymbol* const nilNot1 = nilNot->AddOverload(new FunctionSymbol(FileInfo()));
	nilNot1->symbolNode = new BooleanConstantNode(true);
	nilNot1->arguments.Add(NameList::Bool);
	nilNot1->returnValues.Add(NameList::Bool);

	FunctionSymbol* const toBool  = nilSym->AddSymbol(Name::As, new FunctionSymbol(FileInfo()));
	FunctionSymbol* const toBool1 = toBool->AddOverload(new FunctionSymbol(FileInfo()));
	toBool1->symbolNode = new BooleanConstantNode(false);
	toBool1->arguments.Add(NameList::Nil);
	toBool1->returnValues.Add(NameList::Bool);
	toBool1->isExplicit = true;

	FunctionSymbol* const assign  = nilSym->AddSymbol(Name::Assign, new FunctionSymbol(FileInfo()));
	FunctionSymbol* const assign1 = assign->AddOverload(new FunctionSymbol(FileInfo()));
	assign1->symbolNode = new EmptySymbolNode();
	assign1->arguments.Add(NameList::Nil);

	SymbolTable::Nil = nilSym;
}

StructSymbol* SymbolTable::String = nullptr;

void SymbolTable::SetupString() {
	StructSymbol* const str = symbols->AddSymbol(NameList::String[0], new StructSymbol(FileInfo()));

	PtrSymbol* const ptr = str->AddSymbol(Name::Array, new PtrSymbol(FileInfo()));
	ptr->type = NameList::Char;

	VariableSymbol* const length = str->AddSymbol(Name::Length, new VariableSymbol(FileInfo()));
	length->type = NameList::UInt;
	str->members.Add(Name::Length);

	VariableSymbol* const items = str->AddSymbol(Name::Items, new VariableSymbol(FileInfo()));
	items->type = ptr->AbsoluteName();
	str->members.Add(Name::Items);

	FunctionSymbol* const assign  = str->AddSymbol(Name::Assign, new FunctionSymbol(FileInfo()));
	FunctionSymbol* const assign1 = assign->AddOverload(new FunctionSymbol(FileInfo()));
	assign1->symbolNode = new StructAssignNode();
	assign1->arguments.Add(str->AbsoluteName());

	FunctionSymbol* const index  = str->AddSymbol(Name::Index, new FunctionSymbol(FileInfo()));
	FunctionSymbol* const index1 = index->AddOverload(new FunctionSymbol(FileInfo()));
	index1->symbolNode = new IndexGetNode();
	index1->arguments.Add(NameList::String);
	index1->arguments.Add(NameList::UInt);
	index1->returnValues.Add(NameList::Char);

	FunctionSymbol* const len  = str->AddSymbol(Name::Len, new FunctionSymbol(FileInfo()));
	FunctionSymbol* const len1 = len->AddOverload(new FunctionSymbol(FileInfo()));
	len1->symbolNode = new LengthNode();
	len1->arguments.Add(NameList::String);
	len1->returnValues.Add(NameList::UInt);

	FunctionSymbol* const concat  = str->AddSymbol(Name::Add, new FunctionSymbol(FileInfo()));
	FunctionSymbol* const concat1 = concat->AddOverload(new FunctionSymbol(FileInfo()));
	concat1->symbolNode = new StringConcatNode();
	concat1->arguments.Add(NameList::String);
	concat1->arguments.Add(NameList::String);
	concat1->returnValues.Add(NameList::String);

	SymbolTable::String = str;
}

void SymbolTable::SetupOptional() {
	StructSymbol* const optionalSym = symbols->AddSymbol(Name::Optional, new StructSymbol(FileInfo()));
	List<NameList> args;
	args.Add(NameList().Add(Name("")).Add(Name("T")));
	Name scope = Name("");
	scope.types = args;

	StructSymbol* const optional = new StructSymbol(FileInfo());
	optional->templateArguments.Add(args[0]);
	optionalSym->AddTemplateVariant(optional);

	TemplateSymbol* const templateSym = optional->AddSymbol(Name("T"), new TemplateSymbol(FileInfo()));
	templateSym->type = templateSym->AbsoluteName();

	VariableSymbol* const hasValue = optional->AddSymbol(Name::HasValue, new VariableSymbol(FileInfo()));
	hasValue->type = NameList::Bool;
	optional->members.Add(Name::HasValue);

	VariableSymbol* const value = optional->AddSymbol(Name::Value, new VariableSymbol(FileInfo()));
	value->type = templateSym->AbsoluteName();
	optional->members.Add(Name::Value);

	FunctionSymbol* const assign  = optional->AddSymbol(Name::Assign, new FunctionSymbol(FileInfo()));

	FunctionSymbol* const assign1 = assign->AddOverload(new FunctionSymbol(FileInfo()));
	assign1->symbolNode = new OptionalAssignNode();
	assign1->arguments.Add(optional->AbsoluteName());

	FunctionSymbol* const assign2 = assign->AddOverload(new FunctionSymbol(FileInfo()));
	assign2->symbolNode = new OptionalAssignValueNode();
	assign2->arguments.Add(templateSym->AbsoluteName());

	FunctionSymbol* const assign3 = assign->AddOverload(new FunctionSymbol(FileInfo()));
	assign3->symbolNode = new OptionalAssignValueNode();
	assign3->arguments.Add(NameList::Nil);

	FunctionSymbol* const unwrap  = optional->AddSymbol(Name::Unwrap, new FunctionSymbol(FileInfo()));
	FunctionSymbol* const unwrap1 = unwrap->AddOverload(new FunctionSymbol(FileInfo()));
	unwrap1->symbolNode = new OptionalUnwrapNode();
	unwrap1->arguments.Add(optional->AbsoluteName());
	unwrap1->returnValues.Add(templateSym->AbsoluteName());

	FunctionSymbol* const toBool  = optional->AddSymbol(Name::As, new FunctionSymbol(FileInfo()));
	FunctionSymbol* const toBool1 = toBool->AddOverload(new FunctionSymbol(FileInfo()));
	toBool1->symbolNode = new OptionalToBooleanNode();
	toBool1->arguments.Add(optional->AbsoluteName());
	toBool1->returnValues.Add(NameList::Bool);
	toBool1->isExplicit = true;

	FunctionSymbol* const optionalNot  = optional->AddSymbol(Name::Not, new FunctionSymbol(FileInfo()));
	FunctionSymbol* const optionalNot1 = optionalNot->AddOverload(new FunctionSymbol(FileInfo()));
	optionalNot1->symbolNode = new OptionalNotNode();
	optionalNot1->arguments.Add(optional->AbsoluteName());
	optionalNot1->returnValues.Add(NameList::Bool);
}

void SymbolTable::SetupPointer() {
	PtrSymbol* const pointerSym = symbols->AddSymbol(Name::Pointer, new PtrSymbol(FileInfo()));
	List<NameList> args;
	args.Add(NameList().Add(Name("")).Add(Name("T")));
	Name scope = Name("");
	scope.types = args;

	PtrSymbol* const pointer = new PtrSymbol(FileInfo());
	pointer->templateArguments.Add(args[0]);
	pointer->type = args[0];
	pointerSym->AddTemplateVariant(pointer);

	TemplateSymbol* const templateSym = pointer->AddSymbol(Name("T"), new TemplateSymbol(FileInfo()));
	templateSym->type = templateSym->AbsoluteName();

	FunctionSymbol* const assign  = pointer->AddSymbol(Name::Assign, new FunctionSymbol(FileInfo()));
	FunctionSymbol* const assign1 = assign->AddOverload(new FunctionSymbol(FileInfo()));
	assign1->symbolNode = new PtrAssignNode();
	assign1->arguments.Add(pointer->AbsoluteName());

	FunctionSymbol* const unwrap  = pointer->AddSymbol(Name::Unwrap, new FunctionSymbol(FileInfo()));
	FunctionSymbol* const unwrap1 = unwrap->AddOverload(new FunctionSymbol(FileInfo()));
	unwrap1->symbolNode = new PtrUnwrapNode();
	unwrap1->arguments.Add(pointer->AbsoluteName());
	unwrap1->returnValues.Add(templateSym->AbsoluteName());
}

void SymbolTable::SetupArray() {
	ClassSymbol* const arraySym = symbols->AddSymbol(Name::Array, new ClassSymbol(FileInfo()));
	List<NameList> args;
	args.Add(NameList().Add(Name("")).Add(Name("T")));
	Name scope = Name("");
	scope.types = args;

	ClassSymbol* const arr = new ClassSymbol(FileInfo());
	arr->templateArguments.Add(args[0]);
	arraySym->AddTemplateVariant(arr);

	TemplateSymbol* const templateSym = arr->AddSymbol(Name("T"), new TemplateSymbol(FileInfo()));
	templateSym->type = templateSym->AbsoluteName();

	VariableSymbol* const length = arr->AddSymbol(Name::Length, new VariableSymbol(FileInfo()));
	length->type = NameList::UInt;
	arr->members.Add(Name::Length);

	VariableSymbol* const items = arr->AddSymbol(Name::Items, new VariableSymbol(FileInfo()));
	items->type = templateSym->AbsoluteName();
	arr->members.Add(Name::Items);

	FunctionSymbol* const assign  = arr->AddSymbol(Name::Assign, new FunctionSymbol(FileInfo()));
	FunctionSymbol* const assign1 = assign->AddOverload(new FunctionSymbol(FileInfo()));
	assign1->symbolNode = new PtrAssignNode();
	assign1->arguments.Add(arr->AbsoluteName());

	FunctionSymbol* const index  = arr->AddSymbol(Name::Index, new FunctionSymbol(FileInfo()));
	FunctionSymbol* const index1 = index->AddOverload(new FunctionSymbol(FileInfo()));
	index1->symbolNode = new IndexGetNode();
	index1->arguments.Add(arr->AbsoluteName());
	index1->arguments.Add(NameList::UInt);
	index1->returnValues.Add(args[0]);

	FunctionSymbol* const index2 = index->AddOverload(new FunctionSymbol(FileInfo()));
	index2->symbolNode = new IndexSetNode();
	index2->arguments.Add(arr->AbsoluteName());
	index2->arguments.Add(NameList::UInt);
	index2->returnValues.Add(args[0]);
	index2->attributes = FunctionAttributes::Set;

	FunctionSymbol* const len  = arr->AddSymbol(Name::Len, new FunctionSymbol(FileInfo()));
	FunctionSymbol* const len1 = len->AddOverload(new FunctionSymbol(FileInfo()));
	len1->symbolNode = new LengthNode();
	len1->arguments.Add(arr->AbsoluteName());
	len1->returnValues.Add(NameList::UInt);
}

void SymbolTable::SetupList() {
	ClassSymbol* const listSym = symbols->AddSymbol(Name::List, new ClassSymbol(FileInfo()));
	List<NameList> args;
	args.Add(NameList().Add(Name("")).Add(Name("T")));
	Name scope = Name("");
	scope.types = args;

	ClassSymbol* const list = new ClassSymbol(FileInfo());
	list->templateArguments.Add(args[0]);
	listSym->AddTemplateVariant(list);

	TemplateSymbol* const templateSym = list->AddSymbol(Name("T"), new TemplateSymbol(FileInfo()));
	templateSym->type = templateSym->AbsoluteName();

	PtrSymbol* const ptr = list->AddSymbol(Name::Array, new PtrSymbol(FileInfo()));
	ptr->type = templateSym->type;

	VariableSymbol* const length = list->AddSymbol(Name::Length, new VariableSymbol(FileInfo()));
	length->type = NameList::UInt;
	list->members.Add(Name::Length);

	VariableSymbol* const capacity = list->AddSymbol(Name::Capacity, new VariableSymbol(FileInfo()));
	capacity->type = NameList::UInt;
	list->members.Add(Name::Capacity);

	VariableSymbol* const items = list->AddSymbol(Name::Items, new VariableSymbol(FileInfo()));
	items->type = ptr->AbsoluteName();
	list->members.Add(Name::Items);

	FunctionSymbol* const assign  = list->AddSymbol(Name::Assign, new FunctionSymbol(FileInfo()));
	FunctionSymbol* const assign1 = assign->AddOverload(new FunctionSymbol(FileInfo()));
	assign1->symbolNode = new PtrAssignNode();
	assign1->arguments.Add(list->AbsoluteName());

	FunctionSymbol* const index  = list->AddSymbol(Name::Index, new FunctionSymbol(FileInfo()));
	FunctionSymbol* const index1 = index->AddOverload(new FunctionSymbol(FileInfo()));
	index1->symbolNode = new IndexGetNode();
	index1->arguments.Add(list->AbsoluteName());
	index1->arguments.Add(NameList::UInt);
	index1->returnValues.Add(args[0]);

	FunctionSymbol* const index2 = index->AddOverload(new FunctionSymbol(FileInfo()));
	index2->symbolNode = new IndexSetNode();
	index2->arguments.Add(list->AbsoluteName());
	index2->arguments.Add(NameList::UInt);
	index2->returnValues.Add(args[0]);
	index2->attributes = FunctionAttributes::Set;

	FunctionSymbol* const indexAdd = index->AddOverload(new FunctionSymbol(FileInfo()));
	indexAdd->symbolNode = new ListAddNode();
	indexAdd->arguments.Add(list->AbsoluteName());
	indexAdd->returnValues.Add(args[0]);
	indexAdd->attributes = FunctionAttributes::Set;

	FunctionSymbol* const len  = list->AddSymbol(Name::Len, new FunctionSymbol(FileInfo()));
	FunctionSymbol* const len1 = len->AddOverload(new FunctionSymbol(FileInfo()));
	len1->symbolNode = new LengthNode();
	len1->arguments.Add(list->AbsoluteName());
	len1->returnValues.Add(NameList::UInt);
}
