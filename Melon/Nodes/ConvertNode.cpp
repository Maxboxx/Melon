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
	return type;
}

CompiledNode ConvertNode::Compile(CompileInfo& info) {
	if (node->Type() == type) return node->Compile(info);

	Symbol convert = Symbol::FindExplicitConversion(node->Type(), type, file);

	List<NodePtr> nodes;
	nodes.Add(node);

	if (convert.node) {
		return convert.node->Compile(nodes, info);
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
	if (includeScanned) return;

	node->IncludeScan(info);

	includeScanned = true;
}

Set<ScanType> ConvertNode::Scan(ScanInfoStack& info) const {
	if (node->Type() == type) return Set<ScanType>();
	Symbol::FindExplicitConversion(node->Type(), type, file);
	return node->Scan(info);
}

Mango ConvertNode::ToMango() const {
	Mango mango = Mango("as", MangoType::Map);
	mango.Add("from", node->ToMango());
	mango.Add("to", type.ToString());
	mango.Add("explicit", isExplicit);
	return mango;
}
