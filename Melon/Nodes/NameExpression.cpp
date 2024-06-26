#include "NameExpression.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/TypeSymbol.h"
#include "Melon/Symbols/TemplateSymbol.h"
#include "Melon/Symbols/FunctionSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"
#include "Melon/Symbols/ClassSymbol.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;

NameExpression::NameExpression(Symbols::Symbol* const scope, const FileInfo& file) : Expression(scope, file) {

}

NameExpression::~NameExpression() {

}

TypeSymbol* NameExpression::Type(TypeSymbol* expected) const {
	Symbols::Symbol* const s = Symbol();

	if (s == nullptr) return nullptr;
	
	return s->Type();
}

Symbol* NameExpression::Symbol(TypeSymbol* expected) const {
	Name s = name.Copy();

	// Get template types
	if (s.types) {
		for (NameList& type : *s.types) {
			TypeSymbol* const sym = SymbolTable::Find<TypeSymbol>(type, scope->AbsoluteName(), file, SymbolTable::SearchOptions::ReplaceTemplates);
			if (!sym) continue;

			if (sym->Is<TemplateSymbol>()) {
				type = sym->Type()->AbsoluteName();
			}
			else {
				type = sym->AbsoluteName();
			}
		}
	}

	// Find symbol without template arguments
	Name noTemplateScope = s.Copy();
	noTemplateScope.types = nullptr;

	Symbols::Symbol* const sym = SymbolTable::Find(noTemplateScope, scope->AbsoluteName(), file, SymbolTable::SearchOptions::ReplaceTemplates);

	// Check if name is template type
	if (sym->Is<TemplateSymbol>()) {
		NameList type = sym->Type()->AbsoluteName();
		type[type.Size() - 1].types = s.types;
		return SymbolTable::FindAbsolute(type, file);
	}

	// Check if name if function
	if (sym->Is<FunctionSymbol>()) {
		return sym;
	}
	// Find template overload
	else if (s.types) {
		Name scope = s.Copy();
		scope.name  = "";
		return sym->Find(scope, file);
	}
	// Return symbol
	else {
		return sym;
	}
}

Ptr<Kiwi::Value> NameExpression::Compile(CompileInfo& info) {
	VariableSymbol* const sym = Symbol()->Cast<VariableSymbol>();

	if (!sym) return nullptr;

	if (!ignoreRef && sym->HasAttribute(VariableModifiers::Ref)) {
		return new Kiwi::DerefVariable(sym->KiwiName());
	}
	else {
		return new Kiwi::Variable(sym->KiwiName());
	}
}

ScanResult NameExpression::Scan(ScanInfoStack& info) {
	ScanResult result;
	Symbols::Symbol* const s = Symbol();

	// Check if name is self
	if (name == Name::Self) {
		result.selfUsed = true;
	}
	// Check if var is used
	else if (!info->assign) {
		if (VariableSymbol* const var = s->Cast<VariableSymbol>()) {
			info.usedVariables.Add(var);
		}
	}

	return result;
}

NameList NameExpression::FindSideEffectScope(const bool assign) {
	if (assign) {
		Symbols::Symbol* const s = Symbol();

		if (s->Is<VariableSymbol>() && (s->Cast<VariableSymbol>()->modifiers & VariableModifiers::Ref) != VariableModifiers::None) {
			return s->Parent()->Parent()->AbsoluteName();
		}
		else {
			return s->Parent()->AbsoluteName();
		}
	}

	return scope->AbsoluteName();
}

StringBuilder NameExpression::ToMelon(const UInt indent) const {
	return name.ToSimpleString();
}