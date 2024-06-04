#pragma once

#include "BaseCallNode.h"

#include "TypeExpression.h"
#include "ObjectInitExpression.h"
#include "TypeConversion.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/StructSymbol.h"
#include "Melon/Symbols/EnumSymbol.h"
#include "Melon/Symbols/ClassSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"
#include "Melon/Symbols/ValueSymbol.h"
#include "Melon/Symbols/IntegerSymbol.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

#include "AnyExpression.h"
#include "NameExpression.h"
#include "DotExpression.h"

#include "Boxx/Optional.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

// Note: Delete CallExpresion.obj for this file to build correctly.
// Include this file in all cpp files that references call nodes.

template <BaseCallType T>
inline BaseCallNode<T>::BaseCallNode(Symbols::Symbol* const scope, const FileInfo& file) : T(scope, file) {

}


template <BaseCallType T>
inline BaseCallNode<T>::~BaseCallNode() {
	
}

template <BaseCallType T>
inline List<TypeSymbol*> BaseCallNode<T>::GetReturnTypes(TypeSymbol* expected) const {
	List<TypeSymbol*> types;

	if (ValueSymbol* value = EnumValue(expected)) {
		types.Add(value->ParentType());
		return types;
	}

	FunctionSymbol* const f = GetFunc(expected);

	if (f == nullptr) {
		types.Add(nullptr);
		return types;
	}

	if (IsInit(expected)) {
		types.Add(expression->Symbol(expected)->Cast<TypeSymbol>());
	}
	else for (UInt i = 0; i < f->returnValues.Count(); i++) {
		types.Add(f->ReturnType(i));
	}

	if (types.IsEmpty()) {
		types.Add(nullptr);
	}

	return types;
}

template <BaseCallType T>
inline Name BaseCallNode<T>::GetFuncName() const {
	if (Weak<NameExpression> nn = expression.As<NameExpression>()) {
		return nn->name;
	}
	else if (Weak<DotExpression> nn = expression.As<DotExpression>()) {
		return nn->name;
	}
	else {
		return Name();
	}
}

template <BaseCallType T>
inline Optional<List<TypeSymbol*>> BaseCallNode<T>::GetTemplateTypes(const Optional<List<NameList>>& types) const {
	if (!types) return nullptr;

	List<TypeSymbol*> args;

	for (const NameList& s : *types) {
		TypeSymbol* const type = SymbolTable::Find<TypeSymbol>(s, this->scope->AbsoluteName(), this->File(), SymbolTable::SearchOptions::ReplaceTemplates);

		if (!type) return nullptr;

		args.Add(type);
	}

	return args;
}

template <BaseCallType T>
inline Optional<List<TypeSymbol*>> BaseCallNode<T>::GetArgumentTypes() const {
	List<TypeSymbol*> argTypes;

	for (Weak<Expression> arg : arguments) {
		if (TypeSymbol* const type = arg->Type()) {
			argTypes.Add(type);
		}
		else {
			// TODO: error
			ErrorLog::Error(LogMessage("argument types not found"), this->File());
			return nullptr;
		}
	}

	return argTypes;
}

template <BaseCallType T>
inline FunctionSymbol* BaseCallNode<T>::GetInitFunction(const Optional<List<TypeSymbol*>>& templateTypes, const List<TypeSymbol*>& argTypes, TypeSymbol* expected) const {
	if (expected) {
		expression->Type(expected);
	}
	
	if (TypeSymbol* const t = expression->Symbol<TypeSymbol>()) {
		if (FunctionSymbol* const f = t->Find<FunctionSymbol>(Name::Init, expression->File())) {
			return f->FindMethodOverload(argTypes, expression->File());
		}
	}

	return nullptr;
}

template <BaseCallType T>
inline FunctionSymbol* BaseCallNode<T>::GetStaticOrPlainFunction(const Optional<List<TypeSymbol*>>& templateTypes, const List<TypeSymbol*>& argTypes) const {
	if (FunctionSymbol* const f = expression->Symbol<FunctionSymbol>()) {
		if (f->Parent()->Is<TypeSymbol>()) {
			if (templateTypes) {
				return f->FindStaticOverload(*templateTypes, argTypes, expression->File());
			}
			else {
				return f->FindStaticOverload(argTypes, expression->File());
			}
		}
		else {
			if (templateTypes) {
				return f->FindOverload(*templateTypes, argTypes, expression->File());
			}
			else {
				return f->FindOverload(argTypes, expression->File());
			}
		}
	}

	return nullptr;
}

template <BaseCallType T>
inline FunctionSymbol* BaseCallNode<T>::GetMethod(const Optional<List<TypeSymbol*>>& templateTypes, const List<TypeSymbol*>& argTypes) const {
	Weak<DotExpression> dot = expression.As<DotExpression>();
	if (!dot) return nullptr;

	if (TypeSymbol* const t = dot->expression->Type()) {
		if (FunctionSymbol* const f = t->Find<FunctionSymbol>(dot->name, expression->File())) {
			if (templateTypes) {
				return f->FindMethodOverload(*templateTypes, argTypes, expression->File());
			}
			else {
				return f->FindMethodOverload(argTypes, expression->File());
			}
		}
	}

	return nullptr;
}

template <BaseCallType T>
inline FunctionSymbol* BaseCallNode<T>::GetFunctionSymbol(const Optional<List<TypeSymbol*>>& templateTypes, const List<TypeSymbol*>& argTypes, TypeSymbol* expected) const {
	if (IsInit(expected)) {
		return GetInitFunction(templateTypes, argTypes, expected);
	}
	else if (!IsMethod()) {
		return GetStaticOrPlainFunction(templateTypes, argTypes);
	}
	else {
		return GetMethod(templateTypes, argTypes);
	}
}

template <BaseCallType T>
inline bool BaseCallNode<T>::IsMethod() const {
	if (Weak<DotExpression> dot = expression.As<DotExpression>()) {
		Symbols::Symbol* sym = dot->Symbol();

		if (sym->Is<StructSymbol>()) return false;

		if (dot->expression->Type()) {
			return true;
		}
	}
	
	return false;
}

template <BaseCallType T>
inline FunctionSymbol* BaseCallNode<T>::GetFunc(TypeSymbol* expected) const {
	if (operatorFunction) return operatorFunction;

	List<TypeSymbol*> argTypes;
	Optional<List<TypeSymbol*>> templateTypes = nullptr;

	// Get function name
	Name name = GetFuncName();

	// Get template types
	templateTypes = GetTemplateTypes(name.types);

	if (name.types && !templateTypes) return nullptr;

	// Get argument types
	if (Optional<List<TypeSymbol*>> types = GetArgumentTypes()) {
		argTypes = *types;
	}
	else {
		return nullptr;
	}

	// Get function symbol
	if (FunctionSymbol* const f = GetFunctionSymbol(templateTypes, argTypes, expected)) {
		return f;
	}

	// Handle error
	List<String> argStr;

	if (FunctionSymbol* const f = expression->Symbol<FunctionSymbol>()) {
		for (TypeSymbol* const type : argTypes) {
			argStr.Add(type->AbsoluteName().ToString());
		}

		ErrorLog::Error(LogMessage("error.symbol.function.not_found_args", f->ToString(), argStr), this->File());
	}

	return nullptr;
}

template <BaseCallType T>
inline bool BaseCallNode<T>::IsSelfPassing() const {
	return IsMethod();
}

template <BaseCallType T>
inline bool BaseCallNode<T>::IsInit(TypeSymbol* expected) const {
	if (expression.Is<AnyExpression>()) {
		return true;
	}

	Symbols::Symbol* const s = expression->Symbol(expected);

	if (s->Is<StructSymbol>() || s->Is<ClassSymbol>()) {
		if (s->Parent<EnumSymbol>()) return true;

		return !IsMethod();
	}

	return false;
}

template <BaseCallType T>
inline ValueSymbol* BaseCallNode<T>::EnumValue(TypeSymbol* expected) const {
	Symbols::Symbol* const s = expression->Symbol(expected);
	return s->Cast<ValueSymbol>();
}

template <BaseCallType T>
inline BaseCallNode<T>::CompileResult BaseCallNode<T>::CompileWithResult(CompileInfo& info) { // TODO: more accurate arg error lines
	if (ValueSymbol* value = EnumValue(info.PeekExpectedType())) {
		Ptr<Kiwi::Variable> var = new Kiwi::Variable(info.NewRegister());
		info.AddInstruction(new Kiwi::AssignInstruction(value->ParentType()->KiwiType(), var->Copy(), nullptr));
		info.AddInstruction(new Kiwi::AssignInstruction(new Kiwi::SubVariable(var->Copy(), Name::Value.name), new Kiwi::Integer(SymbolTable::Byte->KiwiType(), value->value)));
		info.AddInstruction(new Kiwi::AssignInstruction(new Kiwi::SubVariable(var->Copy(), Name::Items.name), arguments[0]->Compile(info)));

		CompileResult result;
		result.func = nullptr;
		result.instance = var;
		return result;
	}

	FunctionSymbol* func = GetFunc(info.PeekExpectedType());

	Ptr<Kiwi::CallExpression> call = new Kiwi::CallExpression(func->KiwiName());

	Ptr<Kiwi::Variable> instance = nullptr;

	UInt argOffset = 0;

	if (!operatorFunction && (IsInit(info.PeekExpectedType()) || IsSelfPassing())) {
		argOffset++;

		TypeSymbol* typeSym = func->Argument(0)->Type();
		Kiwi::Type type = typeSym->KiwiType();

		Ptr<Kiwi::Variable> self = nullptr;
		
		if (IsInit(info.PeekExpectedType())) {
			self = new Kiwi::Variable(info.NewRegister());

			Ptr<Kiwi::Expression> expr = nullptr;

			if (typeSym->Is<ClassSymbol>()) {
				Kiwi::Type t = type;
				t.pointers--;
				expr = new Kiwi::AllocExpression(t);
			}

			info.AddInstruction(new Kiwi::AssignInstruction(type, self->Copy(), expr));

			if (typeSym->Is<ClassSymbol>()) {
				info.AddInstruction(new Kiwi::AssignInstruction(
					new Kiwi::SubVariable(new Kiwi::DerefVariable(self->name), Name::VTable.name),
					new Kiwi::Variable(typeSym->AbsoluteName().Add(Name::VData).ToString())
				));
			}
		}
		else if (Weak<DotExpression> dot = expression.As<DotExpression>()) {
			self = dot->expression->Compile(info).AsPtr<Kiwi::Variable>();
		}

		if (type.pointers > 0) {
			call->args.Add(self->Copy());
		}
		else {
			call->args.Add(new Kiwi::RefValue(self->Copy()));
		}

		if (((func->modifiers & FunctionModifiers::Abstract) | (func->modifiers & FunctionModifiers::Override)) != FunctionModifiers::None) {
			Ptr<Kiwi::Variable> funcPtr = new Kiwi::Variable(info.NewRegister());

			info.AddInstruction(new Kiwi::AssignInstruction(
				Kiwi::Type(1, typeSym->AbsoluteName().Add(Name::VTable).ToString()),
				funcPtr->Copy(),
				new Kiwi::SubVariable(new Kiwi::DerefVariable(self->name), Name::VTable.name)
			));

			call->funcPtr = new Kiwi::SubVariable(new Kiwi::DerefVariable(funcPtr->name), func->Parent()->Name().name);
		}

		instance = self;
	}

	for (UInt i = 0; i < arguments.Count(); i++) {
		VariableSymbol* arg = func->Argument(i + argOffset);

		if ((arg->modifiers & VariableModifiers::Ref) != VariableModifiers::None) {
			Kiwi::Type type = func->Argument(i + argOffset)->Type()->KiwiType();

			Ptr<Kiwi::Variable> value = arguments[i]->Compile(info).AsPtr<Kiwi::Variable>();

			if (i < modifiers.Count() && (modifiers[i] & CallArgAttributes::NoRef) != CallArgAttributes::None) {
				Ptr<Kiwi::Variable> tempVar = new Kiwi::Variable(info.NewRegister());
				info.AddInstruction(new Kiwi::AssignInstruction(type, tempVar->Copy(), value));
				value = tempVar;
			}

			call->args.Add(new Kiwi::RefValue(value));
		}
		else {
			Ptr<TypeConversion> conv = new TypeConversion(this->Node::scope, this->Node::file);
			conv->type = arg->Type()->AbsoluteName();
			conv->isExplicit = false;
			conv->expression = new WeakExpression(arguments[i]);

			call->args.Add(conv->Compile(info));
		}
	}
	
	CompileResult result;
	result.call = call;
	result.instance = instance;
	result.func = func;
	return result;
}

template <BaseCallType T>
inline Ptr<Kiwi::Value> BaseCallNode<T>::Compile(CompileInfo& info) {
	CompileResult result = CompileWithResult(info);

	if (!result.func) {
		return result.instance;
	}
	else if (result.func->returnValues.IsEmpty()) {
		info.AddInstruction(new Kiwi::CallInstruction(result.call));
		return result.instance;
	}
	else {
		Ptr<Kiwi::Variable> var = new Kiwi::Variable(info.NewRegister());
		info.AddInstruction(new Kiwi::AssignInstruction(result.func->ReturnType(0)->KiwiType(), var->name, result.call));
		return var;
	}
}

template <BaseCallType T>
inline Ptr<Kiwi::CallExpression> BaseCallNode<T>::CompileCallExpression(CompileInfo& info) {
	CompileResult result = CompileWithResult(info);
	return result.call;
}

template <BaseCallType T>
inline void BaseCallNode<T>::IncludeScan(ParsingInfo& info) {
	expression->IncludeScan(info);

	for (Weak<Expression> arg : arguments) {
		arg->IncludeScan(info);
	}

	GetFunc();
}

template <BaseCallType T>
inline ScanResult BaseCallNode<T>::Scan(ScanInfoStack& info) {
	// Scan called node
	ScanResult result;
	
	if (IsMethod()) {
		result = expression.As<DotExpression>()->expression->Scan(info);
	}
	else {
		result = expression->Scan(info);
	}

	result.SelfUseCheck(info, expression->File());

	// Scan function
	FunctionSymbol* const func = GetFunc();

	if (func == nullptr) {
		return ScanResult();
	}

	if (info->useFunction) {
		if (!info.usedFunctions.Contains(func)) {
			info.usedFunctions.Add(func);
			info.functions.Add(func->node);
		}
	}

	// Scan arguments
	for (UInt i = 0; i < arguments.Count(); i++) {
		VariableSymbol* const arg = func->Argument(i);

		if (arg && (arg->modifiers & VariableModifiers::Ref) == VariableModifiers::None) {
			if (modifiers[i] == CallArgAttributes::NoRef) {
				ErrorLog::Error(LogMessage("error.scan.use.noref"), arguments[i]->File());
			}
			else if (modifiers[i] == CallArgAttributes::Ref) {
				ErrorLog::Error(LogMessage("error.scan.use.ref"), arguments[i]->File());
			}
		}

		ScanResult r = arguments[i]->Scan(info);
		r.SelfUseCheck(info, arguments[i]->File());
		result |= r;
	}

	const bool init = IsInit(info.PeekExpectedType());

	// Scan argument assignment
	for (UInt u = 0; u < func->arguments.Count(); u++) {
		Int i = init ? u - 1 : u;

		TypeSymbol* const type = func->ArgumentType(u);
		
		if (i < 0) continue;

		Weak<Expression> arg;

		if (IsSelfPassing()) {
			if (i == 0) {
				if (IsMethod()) {
					arg = expression.As<DotExpression>()->expression;
				}
				else {
					arg = expression;
				}
			}
			else {
				arg = this->arguments[i - 1];
			}
		}
		else {
			arg = this->arguments[i];
		}

		if (type) {
			Ptr<TypeExpression> tn = new TypeExpression(type->AbsoluteName());
			this->ScanAssignment(tn, arg, info, expression->File());
		}
	}

	return result;
}

template <BaseCallType T>
inline Ptr<T> BaseCallNode<T>::Optimize(OptimizeInfo& info) {
	Node::Optimize(expression, info);

	for (Ptr<Expression>& arg : arguments) {
		Node::Optimize(arg, info);
	}

	return nullptr;
}

template <BaseCallType T>
inline StringBuilder BaseCallNode<T>::ToMelon(const UInt indent) const {
	StringBuilder sb = expression->ToMelon(indent);

	sb += "(";

	for (UInt i = 0; i < arguments.Count(); i++) {
		if (i > 0) sb += ", ";
		if (modifiers[i] == CallArgAttributes::Ref)   sb += "ref ";
		if (modifiers[i] == CallArgAttributes::NoRef) sb += "noref ";
		sb += arguments[i]->ToMelon(indent);
	}

	sb += ")";

	return sb;
}
