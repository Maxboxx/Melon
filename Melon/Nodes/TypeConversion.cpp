#include "TypeConversion.h"

#include "CallExpression.h"
#include "TypeExpression.h"
#include "Expression.h"

#include "Melon/Symbols/TemplateSymbol.h"
#include "Melon/Symbols/ValueSymbol.h"
#include "Melon/Symbols/EnumSymbol.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

TypeConversion::TypeConversion(Symbols::Symbol* const scope, const FileInfo& file) : Expression(scope, file) {

}

TypeConversion::~TypeConversion() {

}

TypeSymbol* TypeConversion::Type(TypeSymbol* expected) const {
	if (type == NameList::Any) {
		return expected;
	}

	Symbols::Symbol* s = SymbolTable::Find(type, scope ? scope->AbsoluteName() : NameList(true), file, SymbolTable::SearchOptions::ReplaceTemplates);

	if (ValueSymbol* value = s->Cast<ValueSymbol>()) {
		s = value->ValueType();

		if (s == nullptr) {
			ErrorLog::Error(LogMessage("enum value conversion error"), file);
			return nullptr;
		}
	}

	if (s && isOptional) {
		Name opt = Name::Optional;
		opt.types = List<NameList>();
		opt.types->Add(s->AbsoluteName());
		s = SymbolTable::Find<TypeSymbol>(NameList(opt), scope ? scope->AbsoluteName() : NameList(true), file, SymbolTable::SearchOptions::ReplaceTemplates);
	}

	if (s == nullptr) return nullptr;
	TypeSymbol* t = s->Cast<TypeSymbol>();
	if (t == nullptr) return nullptr;

	if (t->Is<TemplateSymbol>()) {
		return t->Type();
	}
	
	return t;
}

ValueSymbol* TypeConversion::GetValueSymbol() const {
	ErrorLog::AddMarker();
	ValueSymbol* valueSym = SymbolTable::Find<ValueSymbol>(type, scope ? scope->AbsoluteName() : NameList(true), file, SymbolTable::SearchOptions::ReplaceTemplates);
	ErrorLog::RevertToMarker();

	if (!valueSym) return nullptr;

	if (valueSym->ParentType() != expression->Type() || !valueSym->type || !isOptional) {
		ErrorLog::Error(LogMessage("enum value conversion error"), file);
		return nullptr;
	}

	return valueSym;
}

Ptr<Kiwi::Value> TypeConversion::Compile(CompileInfo& info) {
	if (ValueSymbol* valueSym = GetValueSymbol()) {
		EnumSymbol* enumSym = valueSym->ParentType()->Cast<EnumSymbol>();

		Ptr<Kiwi::Variable> var = expression->Compile(info).AsPtr<Kiwi::Variable>();
		Ptr<Kiwi::Variable> opt = new Kiwi::Variable(info.NewRegister());
		
		const String endLbl = info.NewLabel();

		info.AddInstruction(new Kiwi::AssignInstruction(Type()->KiwiType(), opt->Copy(), nullptr));

		info.AddInstruction(new Kiwi::AssignInstruction(
			new Kiwi::SubVariable(opt->Copy(), Name::HasValue.name),
			new Kiwi::EqualExpression(
				new Kiwi::SubVariable(var->Copy(), Name::Value.name),
				new Kiwi::Integer(enumSym->IdentifierType()->KiwiType(), valueSym->value)
			)
		));

		info.AddInstruction(new Kiwi::IfInstruction(new Kiwi::SubVariable(opt->Copy(), Name::HasValue.name), nullptr, endLbl));
		info.AddInstruction(new Kiwi::AssignInstruction(new Kiwi::SubVariable(opt->Copy(), Name::Value.name), new Kiwi::SubVariable(var, Name::Items.name)));
		info.NewInstructionBlock(endLbl);

		return opt;
	}

	TypeSymbol* const convertType = Type(info.PeekExpectedType());
	type = convertType->AbsoluteName();

	// Check if the node needs conversion
	if (expression->Type() == convertType) {
		return expression->Compile(info);
	}

	// Find conversion operator
	FunctionSymbol* const convert = SymbolTable::FindExplicitConversion(expression->Type(), convertType, file);
	if (!convert) return nullptr;

	// Compile symbol node
	if (convert->symbolNode) {
		return convert->symbolNode->Compile(expression, info, false);
	}
	// Compile call to operator function
	else {
		List<Ptr<Expression>> args;
		args.Add(expression);

		Ptr<CallExpression> cn = new CallExpression(scope, file);
		cn->arguments  = args;
		cn->expression = new TypeExpression(convert->ParentType()->AbsoluteName());

		Ptr<Kiwi::Value> v = cn->Compile(info);
		expression = args[0];
		return v;
	}
}

void TypeConversion::IncludeScan(ParsingInfo& info) {
	expression->IncludeScan(info);
}

ScanResult TypeConversion::Scan(ScanInfoStack& info) {
	if (GetValueSymbol()) {
		return expression->Scan(info);
	}

	TypeSymbol* const convertType = Type();

	if (expression->Type() == convertType) return expression->Scan(info);

	SymbolTable::FindExplicitConversion(expression->Type(), convertType, file);

	return expression->Scan(info);
}

NameList TypeConversion::FindSideEffectScope(const bool assign) {
	// TODO: Check operator function
	return expression->GetSideEffectScope(assign);
}

Ptr<Expression> TypeConversion::Optimize(OptimizeInfo& info) {
	Node::Optimize(expression, info);
	return nullptr;
}

StringBuilder TypeConversion::ToMelon(const UInt indent) const {
	StringBuilder sb = expression->ToMelon(indent);

	if (isExplicit) {
		sb += isOptional ? " as? " : " as ";

		if (ValueSymbol* v = GetValueSymbol()) {
			sb += v->AbsoluteName().ToSimpleString();
		}
		else {
			sb += Type()->AbsoluteName().ToSimpleString();
		}
	}

	return sb;
}