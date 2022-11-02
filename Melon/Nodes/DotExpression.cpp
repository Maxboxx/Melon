#include "DotExpression.h"

#include "NameExpression.h"

#include "Melon/Parsing/Parser.h"
#include "Melon/Parsing/IncludeParser.h"

#include "Melon/Symbols/ValueSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"
#include "Melon/Symbols/TemplateSymbol.h"
#include "Melon/Symbols/StructSymbol.h"
#include "Melon/Symbols/NamespaceSymbol.h"
#include "Melon/Symbols/FunctionSymbol.h"
#include "Melon/Symbols/EnumSymbol.h"

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

TypeSymbol* DotExpression::Type() const {
	Symbols::Symbol* const s = Symbol();

	if (s == nullptr) return nullptr;

	if (s->Is<ValueSymbol>() || s->Is<VariableSymbol>() || s->Is<TemplateSymbol>()) {
		return s->Type();
	}
	else if (TypeSymbol* const t = s->Cast<TypeSymbol>()) {
		return t;
	}

	return nullptr;
}

Symbol* DotExpression::Symbol() const {
	Symbols::Symbol* const nodeSym = expression->Symbol();

	if (nodeSym == nullptr) {
		return nullptr;
	}

	// Get variable symbol
	if (nodeSym->Is<VariableSymbol>()) {
		TypeSymbol* const type = nodeSym->Type();

		if (StructSymbol* const s = type->Cast<StructSymbol>()) {
			return s->Find(name, file);
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

CompiledNode DotExpression::Compile(OldCompileInfo& info) {
	TypeSymbol* const sym = expression->Type();

	CompiledNode c = expression->Compile(info);

	// Compile enum value
	if (EnumSymbol* enumSym = sym->Cast<EnumSymbol>()) {
		c.argument = Argument(sym->Find<ValueSymbol>(name, file)->value);
		c.size = enumSym->Size();
	}
	// Compile variable
	else if (VariableSymbol* const var = sym->Find<VariableSymbol>(name, file)) {
		if (var->HasAttribute(VariableModifiers::Static)) {
			c.argument = MemoryLocation(0);
			c.argument.mem.memptr = var->AbsoluteName().ToString();
		}

		c.argument.mem.offset += var->stackIndex;
		c.size = var->Type()->Size();
	}

	return c;
}

Ptr<Kiwi::Value> DotExpression::Compile(CompileInfo& info) {
	TypeSymbol* const sym = expression->Type();

	Ptr<Kiwi::Variable> value = expression->Compile(info).AsPtr<Kiwi::Variable>();

	// Compile enum value
	if (EnumSymbol* enumSym = sym->Cast<EnumSymbol>()) {
		return new Kiwi::Integer(sym->Find<ValueSymbol>(name, file)->value);
	}
	// Compile struct value
	else if (value) {
		return new Kiwi::SubVariable(value, Symbol()->KiwiName());
	}

	return nullptr;
}

void DotExpression::IncludeScan(ParsingInfo& info) {
	expression->IncludeScan(info);
	
	Symbols::Symbol* const s = expression->Symbol();

	if (!name.types) {
		if (s->Is<NamespaceSymbol>() && !s->Contains(name)) {
			IncludeParser::ParseInclude(s->AbsoluteName().Add(name), info);
		}
	}
}

ScanResult DotExpression::Scan(ScanInfoStack& info) {
	const bool assign = info->assign;
	info->assign = false;

	// Scan node
	ScanResult result = expression->Scan(info);
	info->assign = assign;

	TypeSymbol* const type = expression->Type();
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

					if (info->type->IsInitialized()) {
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