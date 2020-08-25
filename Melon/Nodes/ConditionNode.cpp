#include "ConditionNode.h"

#include "ConvertNode.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

ConditionNode::ConditionNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

ConditionNode::~ConditionNode() {

}

ScopeList ConditionNode::Type() const {
	return ScopeList().Add(Scope::Bool);
}

CompiledNode ConditionNode::Compile(CompileInfo& info) {
	Pointer<ConvertNode> convert = new ConvertNode(scope, file);
	convert->isExplicit = true;
	convert->node = cond;
	convert->type = ScopeList().Add(Scope::Bool);
	return convert->Compile(info);
}

void ConditionNode::IncludeScan(ParsingInfo& info) {
	if (includeScanned) return;

	cond->IncludeScan(info);

	includeScanned = true;
}

Set<ScanType> ConditionNode::Scan(ScanInfoStack& info) const {
	Symbol::FindExplicitConversion(cond->Type(), ScopeList().Add(Scope::Bool), file);
	return cond->Scan(info);
}

Mango ConditionNode::ToMango() const {
	return cond->ToMango();
}
