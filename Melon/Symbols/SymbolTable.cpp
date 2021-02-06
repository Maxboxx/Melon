#include "SymbolTable.h"

#include "TemplateSymbol.h"
#include "TemplateTypeSymbol.h"
#include "FunctionSymbol.h"
#include "NamespaceSymbol.h"
#include "IntegerSymbol.h"
#include "VariableSymbol.h"
#include "StructSymbol.h"

#include "Nodes/SymbolNode.h"
#include "Nodes/IntegerUnaryOperatorNode.h"
#include "Nodes/IntegerBinaryOperatorNode.h"
#include "Nodes/IntegerToBoolNode.h"
#include "Nodes/IntegerNotNode.h"
#include "Nodes/IntegerAssignNode.h"
#include "Nodes/IntegerConvertNode.h"
#include "Nodes/BooleanAssignNode.h"
#include "Nodes/BooleanCompareNode.h"
#include "Nodes/BooleanNotNode.h"
#include "Nodes/BooleanToBooleanNode.h"
#include "Nodes/BooleanConstantNode.h"
#include "Nodes/EmptySymbolNode.h"
#include "Nodes/OptionalAssignNode.h"
#include "Nodes/OptionalAssignValueNode.h"
#include "Nodes/OptionalUnwrapNode.h"
#include "Nodes/OptionalToBooleanNode.h"
#include "Nodes/OptionalNotNode.h"

#include "Kiwi/Kiwi.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

Pointer<ScopeSymbol> SymbolTable::symbols = new ScopeSymbol(FileInfo());
List<SymbolTable::TemplateInfo> SymbolTable::templateSymbols;

Symbol* SymbolTable::FindAbsolute(const ScopeList& name, const FileInfo& file) {
	if (name.Size() > 0 && name[0] == Scope::Global) {
		ScopeList list;

		for (Boxx::UInt i = 1; i < name.Size(); i++) {
			list = list.Add(name[i]);
		}

		return symbols->Symbol::Find(list, file);
	}

	return symbols->Symbol::Find(name, file);
}

Symbol* SymbolTable::ContainsAbsolute(const ScopeList& name) {
	if (name.Size() > 0 && name[0] == Scope::Global) {
		ScopeList list;

		for (Boxx::UInt i = 1; i < name.Size(); i++) {
			list = list.Add(name[i]);
		}

		return symbols->Symbol::Contains(list);
	}

	return symbols->Symbol::Contains(name);
}

Symbol* SymbolTable::Contains(const Scope& name, const ScopeList& scope, const FileInfo& file, const SearchOptions options) {
	return Contains(ScopeList().Add(name), scope, file, options);
}

Symbol* SymbolTable::Contains(const ScopeList& name, const ScopeList& scope, const FileInfo& file, const SearchOptions options) {
	ErrorLog::AddMarker();
	Symbol* const sym = Find(name, scope, file, options);
	ErrorLog::Revert();

	return sym;
}

Symbol* SymbolTable::Find(const Scope& name, const ScopeList& scope, const FileInfo& file, const SearchOptions options) {
	return Find(ScopeList().Add(name), scope, file, options);
}

Symbol* SymbolTable::Find(ScopeList name, ScopeList scope, const FileInfo& file, const SearchOptions options) {
	if ((options & SearchOptions::ReplaceTemplates) != SearchOptions::None) {
		scope = ReplaceTemplatesAbsolute(scope, file);
		name  = ReplaceTemplates(name, scope, file);
	}

	if (name.absolute || (name.Size() > 0 && name[0] == Scope::Global)) {
		return FindAbsolute(name, file);
	}

	for (Boxx::UInt i = 0; i < name.Size(); i++) {
		if (name[i].types) {
			for (ScopeList& type : name[i].types.Get()) {
				if (Symbol* const s = Find(type, scope, file)) {
					type = s->AbsoluteName();
				}
			}
		}
	}

	const bool ignoreOrder = (options & SearchOptions::IgnoreOrder) != SearchOptions::None; 

	if (scope.Size() > 0) {
		ScopeList list = scope;

		while (list.Size() > 0) {
			Symbol* const s = FindInNamespaces(list, file);

			if (Symbol* const sym = s->Contains(name)) {
				if (ignoreOrder || sym->file.statement < file.statement || sym->Is<TemplateSymbol>()) {
					return sym;
				}
			}

			list = list.Pop();
		}
	}

	return FindInNamespaces(name, file);
}

Symbol* SymbolTable::FindInNamespaces(const ScopeList& name, const FileInfo& file) {
	if (name.absolute || (name.Size() > 0 && name[0] == Scope::Global)) {
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
				if (FindAbsolute<NamespaceSymbol>(ScopeList().Add(name[0]), file)) {
					foundSymbol = sym;
				}
			}
		}
	}

	for (const ScopeList& includedNamespace : file.includedNamespaces) {
		if (Symbol* const s = ContainsAbsolute(includedNamespace.Pop().Add(name))) {
			if (includedNamespace.Last() == name[0] || s->file.fileScope == name[0]) {
				Symbol* ns = s->Parent();

				for (Boxx::UInt i = 0; i < name.Size(); i++) {
					if (NamespaceSymbol* const sym = ns->Find<NamespaceSymbol>(name[i], file)) {
						ns = sym;
					}
				}

				if (s->file.currentNamespace == ns->AbsoluteName()) {
					if (foundSymbol && foundSymbol->AbsoluteName() != s->AbsoluteName()) {
						ErrorLog::Error(SymbolError(SymbolError::Ambiguous(name.ToString()), file));
					}

					foundSymbol = s;
				}
			}
		}
	}

	if (foundSymbol) return foundSymbol;

	ErrorLog::Error(SymbolError(SymbolError::NotFoundStart + name.ToString() + SymbolError::NotFoundEnd, file));
	return nullptr;
}

FunctionSymbol* SymbolTable::FindOperator(const Scope& op, TypeSymbol* const type1, TypeSymbol* const type2, const FileInfo& file) {
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

FunctionSymbol* SymbolTable::FindImplicitConversion(TypeSymbol* const from, TypeSymbol* const to, const FileInfo& file) {
	if (!from || !to) return nullptr;

	if (FunctionSymbol* const op = from->ImplicitConversionTo(to)) {
		return op;
	}

	if (FunctionSymbol* const op = to->ImplicitConversionFrom(from)) {
		return op;
	}

	// TODO: error
	return nullptr;
}

FunctionSymbol* SymbolTable::FindExplicitConversion(TypeSymbol* const from, TypeSymbol* const to, const FileInfo& file) {
	if (!from || !to) return nullptr;

	if (FunctionSymbol* const op = from->ExplicitConversionTo(to)) {
		return op;
	}

	if (FunctionSymbol* const op = to->ExplicitConversionFrom(from)) {
		return op;
	}

	// TODO: error
	return nullptr;
}

void SymbolTable::SpecializeTemplate(const ScopeList& name, Symbol* const scope, const FileInfo& file) {
	TemplateInfo info;
	info.name  = name;
	info.scope = scope;
	info.file  = file;
	templateSymbols.Add(info);
}

ScopeList SymbolTable::ReplaceTemplatesAbsolute(const ScopeList& name, const FileInfo& file) {
	ScopeList list;
	list.absolute = true;

	for (Boxx::UInt i = 0; i < name.Size(); i++) {
		Scope scope = name[i].Copy();

		if (scope.types) {
			Symbol* const s = FindAbsolute(list.Add(scope), FileInfo());
			List<ScopeList> args;

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

			Optional<List<ScopeList>> templateArgs = nullptr;

			if (TemplateTypeSymbol* const tts = s->Cast<TemplateTypeSymbol>()) {
				templateArgs = tts->templateArguments;
			}
			else if (FunctionSymbol* const fs = s->Cast<FunctionSymbol>()) {
				templateArgs = fs->templateArguments;
			}

			if (templateArgs) {
				for (const ScopeList& arg : templateArgs.Get()) {
					if (arg.Size() > 0 && arg[0] == Scope()) {
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

				Scope newScope = scope.Copy();
				newScope.types = args;

				if (Symbol* const sym = ContainsAbsolute(list.Add(newScope))) {
					scope = sym->Name();
				}
				else {
					scope = newScope;
				}
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

ScopeList SymbolTable::ReplaceTemplates(const ScopeList& name, const ScopeList& scope, const FileInfo& file) {
	ScopeList typeScope = name;

	for (Boxx::UInt i = 0; i < typeScope.Size(); i++) {
		if (typeScope[i].types) {
			for (ScopeList& type : typeScope[i].types.Get()) {
				ScopeList temp = type;

				if (temp.IsTemplate()) {
					temp = typeScope;

					while (temp.Size() > i + 1) {
						temp = temp.Pop();
					}

					temp = temp.Add(type[1]);
				}

				if (Symbol* const sym = Find(temp, scope, file)) {
					ScopeList absolute = ReplaceTemplatesAbsolute(sym->AbsoluteName(), file);
					
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
	}

	return typeScope;
}

void SymbolTable::Setup() {
	SetupIntegers();
	SetupBoolean();
	SetupNil();
	SetupOptional();
}

IntegerSymbol* SymbolTable::Byte   = nullptr;
IntegerSymbol* SymbolTable::UByte  = nullptr;
IntegerSymbol* SymbolTable::Short  = nullptr;
IntegerSymbol* SymbolTable::UShort = nullptr;
IntegerSymbol* SymbolTable::Int    = nullptr;
IntegerSymbol* SymbolTable::UInt   = nullptr;
IntegerSymbol* SymbolTable::Long   = nullptr;
IntegerSymbol* SymbolTable::ULong  = nullptr;

void SymbolTable::SetupIntegers() {
	Map<ScopeList, Boxx::Byte> integers;
	integers.Add(ScopeList::Byte, -1);
	integers.Add(ScopeList::UByte, 1);
	integers.Add(ScopeList::Short, -2);
	integers.Add(ScopeList::UShort, 2);
	integers.Add(ScopeList::Int, -4);
	integers.Add(ScopeList::UInt, 4);
	integers.Add(ScopeList::Long, -8);
	integers.Add(ScopeList::ULong, 8);

	Map<Scope, InstructionType> binOps;
	binOps.Add(Scope::Add,          InstructionType::Add);
	binOps.Add(Scope::Sub,          InstructionType::Sub);
	binOps.Add(Scope::Mul,          InstructionType::Mul);
	binOps.Add(Scope::IDiv,         InstructionType::Div);
	binOps.Add(Scope::Mod,          InstructionType::Mod);
	binOps.Add(Scope::BitOr,        InstructionType::Or);
	binOps.Add(Scope::BitAnd,       InstructionType::And);
	binOps.Add(Scope::BitXor,       InstructionType::Xor);
	binOps.Add(Scope::BitNor,       InstructionType::Nor);
	binOps.Add(Scope::BitNand,      InstructionType::Nand);
	binOps.Add(Scope::BitXnor,      InstructionType::Xnor);
	binOps.Add(Scope::ShiftLeft,    InstructionType::ShL);
	binOps.Add(Scope::ShiftRight,   InstructionType::ShR);
	binOps.Add(Scope::Equal,        InstructionType::Eq);
	binOps.Add(Scope::NotEqual,     InstructionType::Ne);
	binOps.Add(Scope::LessEqual,    InstructionType::Le);
	binOps.Add(Scope::GreaterEqual, InstructionType::Ge);
	binOps.Add(Scope::Less,         InstructionType::Lt);
	binOps.Add(Scope::Greater,      InstructionType::Gt);

	for (const Pair<ScopeList, Boxx::Byte> integer : integers) {
		// Type
		IntegerSymbol* const intSym = symbols->AddSymbol(integer.key[0], new IntegerSymbol(
			integer.value < 0 ? -integer.value : integer.value,
			integer.value < 0,
			FileInfo()
		));

		// Min max
		VariableSymbol* const min = intSym->AddSymbol(Scope("min"), new VariableSymbol(FileInfo()));
		min->attributes = VariableAttributes::Const | VariableAttributes::Static;
		min->type = intSym->AbsoluteName();

		VariableSymbol* const max = intSym->AddSymbol(Scope("max"), new VariableSymbol(FileInfo()));
		max->attributes = VariableAttributes::Const | VariableAttributes::Static;
		max->type = intSym->AbsoluteName();

		// Neg
		FunctionSymbol* const neg  = intSym->AddSymbol(Scope::Neg, new FunctionSymbol(FileInfo()));
		FunctionSymbol* const neg1 = neg->AddOverload(new FunctionSymbol(FileInfo()));
		neg1->symbolNode = new IntegerUnaryOperatorNode(Math::Abs(integer.value), InstructionType::Neg);

		if (integer.value > 0) {
			neg1->arguments.Add(ScopeList(true).Add(Scope(integer.key[0].name.Sub(1))));
			neg1->returnValues.Add(ScopeList(true).Add(Scope(integer.key[0].name.Sub(1))));
		}
		else {
			neg1->arguments.Add(integer.key);
			neg1->returnValues.Add(integer.key);
		}

		// Bit Not
		FunctionSymbol* const bnot  = intSym->AddSymbol(Scope::BitNot, new FunctionSymbol(FileInfo()));
		FunctionSymbol* const bnot1 = bnot->AddOverload(new FunctionSymbol(FileInfo()));
		bnot1->symbolNode = new IntegerUnaryOperatorNode(Math::Abs(integer.value), InstructionType::Not);
		bnot1->arguments.Add(integer.key);
		bnot1->returnValues.Add(integer.key);

		// To Bool
		FunctionSymbol* const toBool  = intSym->AddSymbol(Scope::As, new FunctionSymbol(FileInfo()));
		FunctionSymbol* const toBool1 = toBool->AddOverload(new FunctionSymbol(FileInfo()));
		toBool1->symbolNode = new IntegerToBoolNode();
		toBool1->arguments.Add(integer.key);
		toBool1->returnValues.Add(ScopeList::Bool);
		toBool1->isExplicit = true;

		// Not
		FunctionSymbol* const intNot  = intSym->AddSymbol(Scope::Not, new FunctionSymbol(FileInfo()));
		FunctionSymbol* const intNot1 = intNot->AddOverload(new FunctionSymbol(FileInfo()));
		intNot1->symbolNode = new IntegerNotNode();
		intNot1->arguments.Add(integer.key);
		intNot1->returnValues.Add(ScopeList::Bool);

		// Assign
		FunctionSymbol* const assign  = intSym->AddSymbol(Scope::Assign, new FunctionSymbol(FileInfo()));
		FunctionSymbol* const assign1 = assign->AddOverload(new FunctionSymbol(FileInfo()));
		assign1->symbolNode = new IntegerAssignNode(integer.value);
		assign1->arguments.Add(integer.key);

		// Binary operators
		for (const Pair<ScopeList, Boxx::Byte> integer2 : integers) {
			FunctionSymbol* const convert1 = intSym->Contains<FunctionSymbol>(Scope::As)->AddOverload(new FunctionSymbol(FileInfo()));
			
			IntegerConvertNode* const cn = new IntegerConvertNode();
			cn->size = Math::Abs(integer.value);
			cn->sign = integer.value < 0;
			cn->targetSize = Math::Abs(integer2.value);

			convert1->arguments.Add(integer.key);
			convert1->returnValues.Add(integer2.key);
			convert1->symbolNode = cn;
			convert1->isExplicit = false;

			for (const Pair<Scope, InstructionType>& op : binOps) {
				FunctionSymbol* binOp = intSym->Contains<FunctionSymbol>(op.key);
				if (!binOp) binOp = intSym->AddSymbol(op.key, new FunctionSymbol(FileInfo()));

				FunctionSymbol* const binOp1 = binOp->AddOverload(new FunctionSymbol(FileInfo()));

				const Boxx::UByte v1 = integer.value < 0 ? -integer.value : integer.value;
				const Boxx::UByte v2 = integer2.value < 0 ? -integer2.value : integer2.value;
				const bool sign = integer.value < 0 || integer2.value < 0;

				if (Instruction::IsComp(op.value))
					binOp1->returnValues.Add(ScopeList::Bool);
				else {
					const ScopeList name = v1 > v2 ? integer.key : integer2.key;

					if (sign && name[0].name[0] == 'u')
						binOp1->returnValues.Add(ScopeList(true).Add(Scope(name[0].name.Sub(1))));
					else
						binOp1->returnValues.Add(name);
				}

				binOp1->arguments.Add(integer.key);
				binOp1->arguments.Add(integer2.key);
				binOp1->symbolNode = new IntegerBinaryOperatorNode(
					v1 > v2 ? v1 : v2,
					sign,
					op.value
				);
			}
		}
	}

	SymbolTable::Byte   = FindAbsolute<IntegerSymbol>(ScopeList::Byte,   FileInfo());
	SymbolTable::UByte  = FindAbsolute<IntegerSymbol>(ScopeList::UByte,  FileInfo());
	SymbolTable::Short  = FindAbsolute<IntegerSymbol>(ScopeList::Short,  FileInfo());
	SymbolTable::UShort = FindAbsolute<IntegerSymbol>(ScopeList::UShort, FileInfo());
	SymbolTable::Int    = FindAbsolute<IntegerSymbol>(ScopeList::Int,    FileInfo());
	SymbolTable::UInt   = FindAbsolute<IntegerSymbol>(ScopeList::UInt,   FileInfo());
	SymbolTable::Long   = FindAbsolute<IntegerSymbol>(ScopeList::Long,   FileInfo());
	SymbolTable::ULong  = FindAbsolute<IntegerSymbol>(ScopeList::ULong,  FileInfo());
}

IntegerSymbol* SymbolTable::Bool = nullptr;

void SymbolTable::SetupBoolean() {
	IntegerSymbol* const boolSym = symbols->AddSymbol(ScopeList::Bool[0], new IntegerSymbol(1, false, FileInfo()));

	FunctionSymbol* const assign  = boolSym->AddSymbol(Scope::Assign, new FunctionSymbol(FileInfo()));
	FunctionSymbol* const assign1 = assign->AddOverload(new FunctionSymbol(FileInfo()));
	assign1->symbolNode = new BooleanAssignNode();
	assign1->arguments.Add(ScopeList::Bool);

	FunctionSymbol* const eq  = boolSym->AddSymbol(Scope::Equal, new FunctionSymbol(FileInfo()));
	FunctionSymbol* const eq1 = eq->AddOverload(new FunctionSymbol(FileInfo()));
	eq1->symbolNode = new BooleanCompareNode(InstructionType::Eq);
	eq1->arguments.Add(ScopeList::Bool);
	eq1->arguments.Add(ScopeList::Bool);
	eq1->returnValues.Add(ScopeList::Bool);

	FunctionSymbol* const ne  = boolSym->AddSymbol(Scope::NotEqual, new FunctionSymbol(FileInfo()));
	FunctionSymbol* const ne1 = ne->AddOverload(new FunctionSymbol(FileInfo()));
	ne1->symbolNode = new BooleanCompareNode(InstructionType::Ne);
	ne1->arguments.Add(ScopeList::Bool);
	ne1->arguments.Add(ScopeList::Bool);
	ne1->returnValues.Add(ScopeList::Bool);

	FunctionSymbol* const boolNot  = boolSym->AddSymbol(Scope::Not, new FunctionSymbol(FileInfo()));
	FunctionSymbol* const boolNot1 = boolNot->AddOverload(new FunctionSymbol(FileInfo()));
	boolNot1->symbolNode = new BooleanNotNode();
	boolNot1->arguments.Add(ScopeList::Bool);
	boolNot1->returnValues.Add(ScopeList::Bool);

	FunctionSymbol* const toBool  = boolSym->AddSymbol(Scope::As, new FunctionSymbol(FileInfo()));
	FunctionSymbol* const toBool1 = toBool->AddOverload(new FunctionSymbol(FileInfo()));
	toBool1->symbolNode = new BooleanToBooleanNode();
	toBool1->arguments.Add(ScopeList::Bool);
	toBool1->returnValues.Add(ScopeList::Bool);
	toBool1->isExplicit = false;

	SymbolTable::Bool = boolSym;
}

TypeSymbol* SymbolTable::Nil = nullptr;

void SymbolTable::SetupNil() {
	TypeSymbol* const nilSym = symbols->AddSymbol(ScopeList::Nil[0], new IntegerSymbol(0, false, FileInfo()));

	FunctionSymbol* const nilNot  = nilSym->AddSymbol(Scope::Not, new FunctionSymbol(FileInfo()));
	FunctionSymbol* const nilNot1 = nilNot->AddOverload(new FunctionSymbol(FileInfo()));
	nilNot1->symbolNode = new BooleanConstantNode(true);
	nilNot1->arguments.Add(ScopeList::Bool);
	nilNot1->returnValues.Add(ScopeList::Bool);

	FunctionSymbol* const toBool  = nilSym->AddSymbol(Scope::As, new FunctionSymbol(FileInfo()));
	FunctionSymbol* const toBool1 = toBool->AddOverload(new FunctionSymbol(FileInfo()));
	toBool1->symbolNode = new BooleanConstantNode(false);
	toBool1->arguments.Add(ScopeList::Nil);
	toBool1->returnValues.Add(ScopeList::Bool);
	toBool1->isExplicit = true;

	FunctionSymbol* const assign  = nilSym->AddSymbol(Scope::Assign, new FunctionSymbol(FileInfo()));
	FunctionSymbol* const assign1 = assign->AddOverload(new FunctionSymbol(FileInfo()));
	assign1->symbolNode = new EmptySymbolNode();
	assign1->arguments.Add(ScopeList::Nil);

	SymbolTable::Nil = nilSym;
}

void SymbolTable::SetupOptional() {
	StructSymbol* const optionalSym = symbols->AddSymbol(Scope::Optional, new StructSymbol(FileInfo()));
	List<ScopeList> args;
	args.Add(ScopeList().Add(Scope("")).Add(Scope("T")));
	Scope scope = Scope("");
	scope.types = args;

	StructSymbol* const optional = optionalSym->AddSymbol(scope, new StructSymbol(FileInfo()));
	optional->templateArguments.Add(args[0]);

	TemplateSymbol* const templateSym = optional->AddSymbol(Scope("T"), new TemplateSymbol(FileInfo()));
	templateSym->type = templateSym->AbsoluteName();

	VariableSymbol* const hasValue = optional->AddSymbol(Scope::HasValue, new VariableSymbol(FileInfo()));
	hasValue->type = ScopeList::Bool;

	VariableSymbol* const value = optional->AddSymbol(Scope::Value, new VariableSymbol(FileInfo()));
	value->type = templateSym->AbsoluteName();

	FunctionSymbol* const assign  = optional->AddSymbol(Scope::Assign, new FunctionSymbol(FileInfo()));

	FunctionSymbol* const assign1 = assign->AddOverload(new FunctionSymbol(FileInfo()));
	assign1->symbolNode = new OptionalAssignNode();
	assign1->arguments.Add(optional->AbsoluteName());

	FunctionSymbol* const assign2 = assign->AddOverload(new FunctionSymbol(FileInfo()));
	assign2->symbolNode = new OptionalAssignValueNode();
	assign2->arguments.Add(templateSym->AbsoluteName());

	FunctionSymbol* const assign3 = assign->AddOverload(new FunctionSymbol(FileInfo()));
	assign3->symbolNode = new OptionalAssignValueNode();
	assign3->arguments.Add(ScopeList::Nil);

	FunctionSymbol* const unwrap  = optional->AddSymbol(Scope::Unwrap, new FunctionSymbol(FileInfo()));
	FunctionSymbol* const unwrap1 = unwrap->AddOverload(new FunctionSymbol(FileInfo()));
	unwrap1->symbolNode = new OptionalUnwrapNode();
	unwrap1->arguments.Add(optional->AbsoluteName());
	unwrap1->returnValues.Add(templateSym->AbsoluteName());

	FunctionSymbol* const toBool  = optional->AddSymbol(Scope::As, new FunctionSymbol(FileInfo()));
	FunctionSymbol* const toBool1 = toBool->AddOverload(new FunctionSymbol(FileInfo()));
	toBool1->symbolNode = new OptionalToBooleanNode();
	toBool1->arguments.Add(optional->AbsoluteName());
	toBool1->returnValues.Add(ScopeList::Bool);
	toBool1->isExplicit = true;

	FunctionSymbol* const optionalNot  = optional->AddSymbol(Scope::Not, new FunctionSymbol(FileInfo()));
	FunctionSymbol* const optionalNot1 = optionalNot->AddOverload(new FunctionSymbol(FileInfo()));
	optionalNot1->symbolNode = new OptionalNotNode();
	optionalNot1->arguments.Add(optional->AbsoluteName());
	optionalNot1->returnValues.Add(ScopeList::Bool);
}
