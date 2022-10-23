#include "TypeConversion.h"

#include "CallExpression.h"
#include "TypeExpression.h"
#include "Expression.h"

#include "Melon/Symbols/TemplateSymbol.h"

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

TypeSymbol* TypeConversion::Type() const {
	TypeSymbol* const s = SymbolTable::Find<TypeSymbol>(type, scope ? scope->AbsoluteName() : NameList(true), file, SymbolTable::SearchOptions::ReplaceTemplates);

	if (s == nullptr) return nullptr;

	if (s->Is<TemplateSymbol>()) {
		return s->Type();
	}
	
	return s;
}

CompiledNode TypeConversion::Compile(OldCompileInfo& info) {
	TypeSymbol* const convertType = Type();

	// Check if the node needs conversion
	if (expression->Type() == convertType) {
		return expression->Compile(info);
	}

	// Find conversion operator
	FunctionSymbol* const convert = SymbolTable::FindExplicitConversion(expression->Type(), convertType, file);
	if (!convert) return CompiledNode();


	// Compile symbol node
	if (convert->symbolNode) {
		return convert->symbolNode->Compile(expression, info);
	}
	// Compile call to operator function
	else {
		List<Ptr<Expression>> args;
		args.Add(expression);

		Ptr<CallExpression> cn = new CallExpression(scope, file);
		cn->arguments  = args;
		cn->expression = new TypeExpression(convert->ParentType()->AbsoluteName());

		CompiledNode c = cn->Compile(info);
		expression = args[0];
		return c;
	}
}

void TypeConversion::IncludeScan(ParsingInfo& info) {
	expression->IncludeScan(info);
}

ScanResult TypeConversion::Scan(ScanInfoStack& info) {
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
		sb += " as ";
		sb += Type()->AbsoluteName().ToString();
	}

	return sb;
}