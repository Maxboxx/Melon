#include "ConvertNode.h"

#include "CallNode.h"
#include "TypeNode.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

ConvertNode::ConvertNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

ConvertNode::~ConvertNode() {

}

TypeSymbol* ConvertNode::Type() const {
	/* TODO: node
	Symbols s = Symbols::FindNearestInNamespace(scope, type, file);

	if (s.type == SymbolType::Template) {
		return s.varType;
	}
	else if (s.type != SymbolType::None) {
		return s.scope;
	}
	else {
		return ScopeList::undefined;
	}
	*/

	return nullptr;
}

CompiledNode ConvertNode::Compile(CompileInfo& info) {
	Symbol* convertType = Type();

	if (node->Type() == convertType) return node->Compile(info);

	/* TODO: node
	Symbols convert = Symbols::FindExplicitConversion(node->Type(), convertType, file);

	if (convert.type == SymbolType::None) return CompiledNode();

	List<NodePtr> nodes;
	nodes.Add(node);

	if (convert.symbolNode) {
		return convert.symbolNode->Compile(nodes, info);
	}
	else {
		Pointer<CallNode> cn = new CallNode(scope, file);
		cn->args = nodes;
		cn->isMethod = false;
		Scope sc = convert.scope.Last();
		sc.variant = nullptr;
		Pointer<TypeNode> tn = new TypeNode(convert.scope.Pop().Add(sc));
		cn->node = tn;
		cn->op = true;
		return cn->Compile(info);
	}
	*/
}

void ConvertNode::IncludeScan(ParsingInfo& info) {
	node->IncludeScan(info);
}

Set<ScanType> ConvertNode::Scan(ScanInfoStack& info) {
	Symbol* convertType = Type();

	/* TODO: node
	if (node->Type() == convertType) return node->Scan(info);
	Symbols::FindExplicitConversion(node->Type(), convertType, file);
	*/

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