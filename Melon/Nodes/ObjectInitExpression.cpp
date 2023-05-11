#include "ObjectInitExpression.h"

#include "KiwiVariable.h"
#include "TypeExpression.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/TypeSymbol.h"
#include "Melon/Symbols/StructSymbol.h"
#include "Melon/Symbols/classSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

ObjectInitExpression::ObjectInitExpression(Symbols::Symbol* const scope, const FileInfo& file) : Expression(scope, file) {

}

ObjectInitExpression::~ObjectInitExpression() {

}

TypeSymbol* ObjectInitExpression::Type() const {
	Symbols::Symbol* const sym = expression->Symbol();

	if (sym == nullptr) return nullptr;

	if (TypeSymbol* const t = sym->Cast<TypeSymbol>()) {
		return t;
	}

	return nullptr;
}

Ptr<Kiwi::Value> ObjectInitExpression::Compile(CompileInfo& info) {
	Ptr<Kiwi::Variable> value = expression->Compile(info).AsPtr<Kiwi::Variable>();

	TypeSymbol* const type = Type();

	if (!value) {
		value = new Kiwi::Variable(info.NewRegister());

		Ptr<Kiwi::Expression> expr;

		if (type->Is<ClassSymbol>()) {
			expr = new Kiwi::AllocExpression(type->KiwiName());
		}

		info.currentBlock->AddInstruction(new Kiwi::AssignInstruction(type->KiwiType(), value->Copy(), expr));
	}

	// Compile vars
	for (UInt i = 0; i < vars.Count(); i++) {
		VariableSymbol* const var = type->Find<VariableSymbol>(vars[i], file);

		if (type->Is<StructSymbol>()) {
			Ptr<KiwiVariable> kv = new KiwiVariable(new Kiwi::SubVariable(value->Copy(), var->KiwiName()), var->Type()->AbsoluteName());
			CompileAssignment(kv, expressions[i], info, expressions[i]->File());
		}
		else if (type->Is<ClassSymbol>()) {
			Ptr<KiwiVariable> kv = new KiwiVariable(new Kiwi::SubVariable(new Kiwi::DerefVariable(value->name), var->KiwiName()), var->Type()->AbsoluteName());
			CompileAssignment(kv, expressions[i], info, expressions[i]->File());
		}
	}

	return value;
}

void ObjectInitExpression::IncludeScan(ParsingInfo& info) {
	expression->IncludeScan(info);

	for (Weak<Expression> expression : expressions) {
		expression->IncludeScan(info);
	}
}

ScanResult ObjectInitExpression::Scan(ScanInfoStack& info) {
	ScanResult result = expression->Scan(info);
	result.SelfUseCheck(info, expression->File());
	
	TypeSymbol* const type = Type();

	if (type == nullptr) return result;

	// Check if object init is valid
	if (ClassStructBaseSymbol* const s = type->Cast<ClassStructBaseSymbol>()) {
		for (const Name& member : s->members) {
			bool found = false;

			for (const Name& var : vars) {
				if (var == member) {
					found = true;
					break;
				}
			}

			if (found) continue;

			ErrorLog::Error(LogMessage("error.scan.init.object_member", member.ToSimpleString()), file);
		}
	}
	else {
		ErrorLog::Error(LogMessage("error.scan.use.object"), file);
	}

	// Scan assignments
	for (UInt i = 0; i < vars.Count(); i++) {
		for (UInt u = i + 1; u < vars.Count(); u++) {
			if (vars[i] == vars[u]) {
				ErrorLog::Error(LogMessage("error.scan.init.multiple", vars[i].name), file);
			}
		}

		VariableSymbol* const v = type->Find<VariableSymbol>(vars[i], file);
		TypeSymbol* const varType = v->Type();

		Ptr<TypeExpression> tn = new TypeExpression(varType->AbsoluteName());
		ScanAssignment(tn, expressions[i], info, expressions[i]->File());
	}

	// Scan expressions
	for (Weak<Expression> node : expressions) {
		ScanResult r = node->Scan(info);
		r.SelfUseCheck(info, node->File());
		result |= r;
	}

	return result;
}

NameList ObjectInitExpression::FindSideEffectScope(const bool assign) {
	NameList list = expression->GetSideEffectScope(assign);

	for (Weak<Expression> expr : expressions) {
		list = CombineSideEffects(list, expr->GetSideEffectScope(assign));
	}

	return list;
}

Ptr<Expression> ObjectInitExpression::Optimize(OptimizeInfo& info) {
	Node::Optimize(expression, info);

	for (Ptr<Expression>& expr : expressions) {
		Node::Optimize(expr, info);
	}

	return nullptr;
}

StringBuilder ObjectInitExpression::ToMelon(const UInt indent) const {
	StringBuilder sb = expression->ToMelon(indent);

	if (vars.IsEmpty()) {
		sb += " {}";
		return sb;
	}

	sb += " {\n";

	String tabs = String('\t').Repeat(indent + 1);

	for (UInt i = 0; i < vars.Count(); i++) {
		if (i > 0) sb += ",\n";
		sb += tabs;
		sb += vars[i].ToString();
		sb += " = ";
		sb += expressions[i]->ToMelon(indent + 1);
	}

	sb += "\n";
	sb += String('\t').Repeat(indent);
	sb += "}";
	return sb;
}
