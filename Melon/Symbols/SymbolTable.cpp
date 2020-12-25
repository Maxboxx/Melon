#include "SymbolTable.h"

#include "TemplateSymbol.h"
#include "TemplateTypeSymbol.h"
#include "FunctionSymbol.h"
#include "NamespaceSymbol.h"
#include "IntegerSymbol.h"
#include "VariableSymbol.h"

#include "Nodes/SymbolNode.h"
#include "Nodes/IntegerUnaryOperatorNode.h"
#include "Nodes/IntegerBinaryOperatorNode.h"
#include "Nodes/IntegerToBoolNode.h"
#include "Nodes/IntegerNotNode.h"
#include "Nodes/IntegerAssignNode.h"
#include "Nodes/IntegerConvertNode.h"

#include "Kiwi/Kiwi.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

Pointer<ScopeSymbol> SymbolTable::symbols = new ScopeSymbol(FileInfo());

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

Symbol* SymbolTable::Contains(const ScopeList& name, const ScopeList& scope, const FileInfo& file, const SearchOptions options) {
	ErrorLog::AddMarker();
	Symbol* const sym = Find(name, scope, file, options);
	ErrorLog::Revert();

	return sym;
}

Symbol* SymbolTable::Find(ScopeList name, ScopeList scope, const FileInfo& file, const SearchOptions options) {
	if ((options & SearchOptions::ReplaceTemplates) != SearchOptions::None) {
		scope = ReplaceTemplatesAbsolute(scope, file);
		name  = ReplaceTemplates(name, scope, file);
	}

	if (name.absolute || (name.Size() > 0 && name[0] == Scope::Global)) {
		return FindAbsolute(name, file);
	}

	const bool ignoreOrder = (options & SearchOptions::IgnoreOrder) != SearchOptions::None; 

	if (scope.Size() > 0) {
		ScopeList list = scope;

		while (list.Size() > 0) {
			Symbol* const s = FindInNamespaces(list, file);

			if (Symbol* const sym = s->Contains(name)) {
				if (ignoreOrder || sym->file.statement < file.statement) {
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
					if (t->type != t->AbsoluteName()) {
						list = ReplaceTemplatesAbsolute(t->type, file);
					}
					else {
						list = t->type;
					}

					i++;
					continue;
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
					args.Add(ReplaceTemplatesAbsolute(arg, file));
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

	return list;
}

ScopeList SymbolTable::ReplaceTemplates(const ScopeList& name, const ScopeList& scope, const FileInfo& file) {
	ScopeList typeScope = name;

	for (Boxx::UInt i = 0; i < typeScope.Size(); i++) {
		if (typeScope[i].types) {
			for (ScopeList& type : typeScope[i].types.Get()) {
				if (Symbol* const sym = Find(type, scope, file)) {
					type = ReplaceTemplatesAbsolute(sym->AbsoluteName(), file);
				}
			}
		}
	}

	return typeScope;
}

void SymbolTable::Setup() {
	SetupIntegers();
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
	// -------------- Integers --------------
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
					binOp->returnValues.Add(ScopeList::Bool);
				else {
					const ScopeList name = v1 > v2 ? integer.key : integer2.key;

					if (sign && name[0].name[0] == 'u')
						binOp->returnValues.Add(ScopeList(true).Add(Scope(name[0].name.Sub(1))));
					else
						binOp->returnValues.Add(name);
				}

				binOp->arguments.Add(integer.key);
				binOp->arguments.Add(integer2.key);
				binOp->symbolNode = new IntegerBinaryOperatorNode(
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

	/*
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

	Symbol nilAssign = Symbol(SymbolType::Function);
	nilAssign.arguments.Add(ScopeList::Nil);
	nilAssign.symbolNode = new EmptySymbolNode();
	nilSym.Add(Scope::Assign, nilAssign, FileInfo());

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
	*/