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

ConvertNode::ConvertNode(Symbol* const scope, const FileInfo& file) : Node(scope, file) {

}

ConvertNode::~ConvertNode() {

}

TypeSymbol* ConvertNode::Type() const {
	TypeSymbol* const s = SymbolTable::Find<TypeSymbol>(type, scope ? scope->AbsoluteName() : ScopeList(true), file, SymbolTable::SearchOptions::ReplaceTemplates);

	if (s == nullptr) return nullptr;

	if (s->Is<TemplateSymbol>()) {
		return s->Type();
	}
	
	return s;
}

CompiledNode ConvertNode::Compile(CompileInfo& info) {
	TypeSymbol* const convertType = Type();

	if (node->Type() == convertType) return node->Compile(info);

	FunctionSymbol* const convert = SymbolTable::FindExplicitConversion(node->Type(), convertType, file);

	if (!convert) return CompiledNode();

	List<NodePtr> nodes;
	nodes.Add(node);

	if (convert->symbolNode) {
		return convert->symbolNode->Compile(nodes, info);
	}
	else {
		Pointer<CallNode> cn = new CallNode(scope, file);
		cn->args = nodes;
		cn->isMethod = false;

		Pointer<TypeNode> tn = new TypeNode(convert->ParentType()->AbsoluteName());
		cn->node = tn;
		cn->op = true;

		return cn->Compile(info);
	}
}

void ConvertNode::IncludeScan(ParsingInfo& info) {
	node->IncludeScan(info);
}

ScanResult ConvertNode::Scan(ScanInfoStack& info) {
	TypeSymbol* const convertType = Type();

	if (node->Type() == convertType) return node->Scan(info);

	SymbolTable::FindExplicitConversion(node->Type(), convertType, file);

	return node->Scan(info);
}

ScopeList ConvertNode::FindSideEffectScope(const bool assign) {
	// TODO: Check operator function
	return node->GetSideEffectScope(assign);
}

NodePtr ConvertNode::Optimize(OptimizeInfo& info) {
	if (NodePtr n = node->Optimize(info)) node = n;

	return nullptr;
}

StringBuilder ConvertNode::ToMelon(const UInt indent) const {
	StringBuilder sb = node->ToMelon(indent);

	if (isExplicit) {
		sb += " as ";
		sb += Type()->AbsoluteName().ToString();
	}

	return sb;
}