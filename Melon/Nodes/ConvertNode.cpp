#include "ConvertNode.h"

#include "CallNode.h"
#include "TypeNode.h"

#include "Melon/Symbols/TemplateSymbol.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

ConvertNode::ConvertNode(Symbols::Symbol* const scope, const FileInfo& file) : ExpressionNode(scope, file) {

}

ConvertNode::~ConvertNode() {

}

TypeSymbol* ConvertNode::Type() const {
	TypeSymbol* const s = SymbolTable::Find<TypeSymbol>(type, scope ? scope->AbsoluteName() : NameList(true), file, SymbolTable::SearchOptions::ReplaceTemplates);

	if (s == nullptr) return nullptr;

	if (s->Is<TemplateSymbol>()) {
		return s->Type();
	}
	
	return s;
}

CompiledNode ConvertNode::Compile(CompileInfo& info) {
	TypeSymbol* const convertType = Type();

	// Check if the node needs conversion
	if (expression->Type() == convertType) {
		return expression->Compile(info);
	}

	// Find conversion operator
	FunctionSymbol* const convert = SymbolTable::FindExplicitConversion(expression->Type(), convertType, file);
	if (!convert) return CompiledNode();

	List<Expression> nodes;
	nodes.Add(expression);

	// Compile symbol node
	if (convert->symbolNode) {
		return convert->symbolNode->Compile(nodes, info);
	}
	// Compile call to operator function
	else {
		Pointer<CallNode> cn = new CallNode(scope, file);
		cn->args = nodes;

		Pointer<TypeNode> tn = new TypeNode(convert->ParentType()->AbsoluteName());
		cn->node = tn;

		return cn->Compile(info);
	}
}

void ConvertNode::IncludeScan(ParsingInfo& info) {
	expression->IncludeScan(info);
}

ScanResult ConvertNode::Scan(ScanInfoStack& info) {
	TypeSymbol* const convertType = Type();

	if (expression->Type() == convertType) return expression->Scan(info);

	SymbolTable::FindExplicitConversion(expression->Type(), convertType, file);

	return expression->Scan(info);
}

NameList ConvertNode::FindSideEffectScope(const bool assign) {
	// TODO: Check operator function
	return expression->GetSideEffectScope(assign);
}

Expression ConvertNode::Optimize(OptimizeInfo& info) {
	Node::Optimize(expression, info);
	return nullptr;
}

StringBuilder ConvertNode::ToMelon(const UInt indent) const {
	StringBuilder sb = expression->ToMelon(indent);

	if (isExplicit) {
		sb += " as ";
		sb += Type()->AbsoluteName().ToString();
	}

	return sb;
}