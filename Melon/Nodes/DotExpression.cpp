#include "DotExpression.h"

#include "NameExpression.h"
#include "AnyExpression.h"

#include "Melon/Parsing/Parser.h"
#include "Melon/Parsing/IncludeParser.h"

#include "Melon/Symbols/ValueSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"
#include "Melon/Symbols/TemplateSymbol.h"
#include "Melon/Symbols/StructSymbol.h"
#include "Melon/Symbols/ClassSymbol.h"
#include "Melon/Symbols/NamespaceSymbol.h"
#include "Melon/Symbols/FunctionSymbol.h"
#include "Melon/Symbols/EnumSymbol.h"
#include "Melon/Symbols/IntegerSymbol.h"

#include "Kiwi/Old/Kiwi.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

DotExpression::DotExpression(Symbols::Symbol* const scope, const FileInfo& file) : Expression(scope, file) {

}

DotExpression::~DotExpression() {

}

TypeSymbol* DotExpression::Type(TypeSymbol* expected) const {
	Symbols::Symbol* const s = Symbol(expected);

	if (s == nullptr) return nullptr;

	if (s->Is<ValueSymbol>() || s->Is<VariableSymbol>() || s->Is<TemplateSymbol>()) {
		return s->Type();
	}

	return nullptr;
}

Symbol* DotExpression::Symbol(TypeSymbol* expected) const {
	if (expected != nullptr && expected->Is<StructSymbol>() && expected->Name() == name) {
		if (EnumSymbol* e = expected->Parent<EnumSymbol>()) {
			expected = e;
		}
	}
	
	Symbols::Symbol* nodeSym = expression->Symbol(expected);

	if (nodeSym == nullptr) {
		return nullptr;
	}

	// Get variable symbol
	if (nodeSym->Is<VariableSymbol>()) {
		TypeSymbol* const type = nodeSym->Type();

		if (type->Is<StructSymbol>() || type->Is<ClassSymbol>() || type->Is<EnumSymbol>()) {
			return type->Find(name, file);
		}
	}
	// Get other symbols
	else {
		if (name.types) {
			if (Symbols::Symbol* const s = nodeSym->Contains(Name(name.name))) {
				if (s->Is<FunctionSymbol>()) {
					return s;
				}
			}

			if (!nodeSym->Contains(name)) {
				Root()->AddTemplateSpecialization(nodeSym->AbsoluteName().Add(name), scope->AbsoluteName(), file);
			}
		}

		return nodeSym->Find(name, file);
	}

	return nullptr;
}

Ptr<Kiwi::Value> DotExpression::Compile(CompileInfo& info) {
	TypeSymbol* const typeSym = expression->Type(info.PeekExpectedType());

	Symbols::Symbol* const symbol = Symbol(info.PeekExpectedType());
	 
	Ptr<Kiwi::Variable> value = expression->Compile(info).AsPtr<Kiwi::Variable>();

	// Compile enum value
	if (EnumSymbol* enumSym = expression->Symbol<EnumSymbol>(info.PeekExpectedType())) {
		ValueSymbol* valueSym = enumSym->Find<ValueSymbol>(name, file);

		if (valueSym) {
			Ptr<Kiwi::Variable> var = new Kiwi::Variable(info.NewRegister());
			info.AddInstruction(new Kiwi::AssignInstruction(enumSym->KiwiType(), var->Copy(), nullptr));
			info.AddInstruction(new Kiwi::AssignInstruction(new Kiwi::SubVariable(var->Copy(), Name::Value.name), new Kiwi::Integer(SymbolTable::Byte->KiwiType(), valueSym->value)));
			//info.AddInstruction(new Kiwi::AssignInstruction(new Kiwi::SubVariable(var->Copy(), Name::Items.name), new Kiwi::Integer(valueSym->KiwiType(), 0)));

			return var;
		}
	}
	// Compile class value
	else if (typeSym->Is<ClassSymbol>()) {
		if (expression.Is<DotExpression>() || !info.assign) {
			Ptr<Kiwi::Variable> var = new Kiwi::Variable(info.NewRegister());

			info.AddInstruction(new Kiwi::AssignInstruction(Type()->KiwiType(), var->Copy(), new Kiwi::SubVariable(new Kiwi::DerefVariable(value->name), symbol->KiwiName())));

			return var;
		}
		else {
			return new Kiwi::SubVariable(new Kiwi::DerefVariable(value->name), symbol->KiwiName());
		}
	}
	// Compile struct value
	else if (value) {
		TypeSymbol* const t = Type(info.PeekExpectedType());

		if (t->Is<ClassSymbol>() && !info.assign) {
			Ptr<Kiwi::Variable> var = new Kiwi::Variable(info.NewRegister());

			info.AddInstruction(new Kiwi::AssignInstruction(t->KiwiType(), var->Copy(), new Kiwi::SubVariable(new Kiwi::DerefVariable(value->name), symbol->KiwiName())));

			return var;
		}

		return new Kiwi::SubVariable(value, symbol->KiwiName());
	}

	return nullptr;
}

void DotExpression::IncludeScan(ParsingInfo& info) {
	expression->IncludeScan(info);
	
	Symbols::Symbol* const s = expression->Symbol();

	if (!name.types) {
		if (s->Is<NamespaceSymbol>() && !s->Contains(name)) {
			IncludeParser::Include(s->AbsoluteName().Add(name), info, false);
		}
	}
}

ScanResult DotExpression::Scan(ScanInfoStack& info) {
	const bool assign = info->assign;
	info->assign = false;

	// Scan node
	ScanResult result = expression->Scan(info);
	info->assign = assign;

	// Scan enum value
	if (ValueSymbol* val = Expression::Symbol<ValueSymbol>()) {
		if (val->type) {
			// TODO: Error
			//ErrorLog::Error(LogMessage("enum value is missing value argument"), File());
		}
	}

	TypeSymbol* const type = expression->Type(info.PeekExpectedType());
	if (type == nullptr) return result;

	Symbols::Symbol* sym = type->Find(Name(name.name), file);
	if (sym == nullptr) return result;

	if (!sym->Is<FunctionSymbol>()) {
		sym = type->Find(name, file);
	}

	// Scan variable
	if (VariableSymbol* const var = sym->Cast<VariableSymbol>()) {
		if ((var->modifiers & VariableModifiers::Static) != VariableModifiers::None) {
			info.usedVariables.Add(var);
		}
	}

	// Scan assignment
	if (info->assign) {
		if (Weak<NameExpression> nn = expression.As<NameExpression>()) {
			if (nn->name == Name::Self) {
				if (VariableSymbol* const var = sym->Cast<VariableSymbol>()) {
					if (info->scopeInfo.WillContinue()) {
						var->isAssigned = true;
					}

					if (info->type && info->type->IsInitialized()) {
						info->init = false;
					}
				}

				result.selfUsed = false;
			}
		}
	}
	// Scan init
	else if (info->init) {
		if (Weak<NameExpression> nn = expression.As<NameExpression>()) {
			if (nn->name == Name::Self) {
				if (VariableSymbol* const var = sym->Cast<VariableSymbol>()) {
					if (!var->isAssigned) {
						ErrorLog::Error(LogMessage("error.scan.use.member", name.ToSimpleString()), file);
					}
				}

				result.selfUsed = false;
			}
		}
	}
	else {
		result.selfUsed = false;
	}

	return result;
}

NameList DotExpression::FindSideEffectScope(const bool assign) {
	if (assign) {
		return expression->GetSideEffectScope(assign);
	}

	return scope->AbsoluteName();
}

Ptr<Expression> DotExpression::Optimize(OptimizeInfo& info) {
	Node::Optimize(expression, info);
	return nullptr;
}

StringBuilder DotExpression::ToMelon(const UInt indent) const {
	StringBuilder sb = expression->ToMelon(indent);
	sb += ".";
	sb += name.ToSimpleString();
	return sb;
}