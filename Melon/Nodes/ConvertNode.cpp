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

ScopeList ConvertNode::Type() const {
	Symbol s = Symbol::FindNearestInNamespace(scope, type, file);

	if (s.type == SymbolType::Template) {
		return s.varType;
	}
	else if (s.type != SymbolType::None) {
		return s.scope;
	}
	else {
		return ScopeList::undefined;
	}
}

CompiledNode ConvertNode::Compile(CompileInfo& info) {
	ScopeList convertType = Type();

	if (node->Type() == convertType) return node->Compile(info);

	Symbol convert = Symbol::FindExplicitConversion(node->Type(), convertType, file);

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
}

void ConvertNode::IncludeScan(ParsingInfo& info) {
	node->IncludeScan(info);
}

Set<ScanType> ConvertNode::Scan(ScanInfoStack& info) {
	ScopeList convertType = Type();

	if (node->Type() == convertType) return Set<ScanType>();
	Symbol::FindExplicitConversion(node->Type(), convertType, file);
	return node->Scan(info);
}

Mango ConvertNode::ToMango() const {
	Mango mango = Mango("as", MangoType::Map);
	mango.Add("from", node->ToMango());
	mango.Add("to", Type().ToString());
	mango.Add("explicit", isExplicit);
	return mango;
}
