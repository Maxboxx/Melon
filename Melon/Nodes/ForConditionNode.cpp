#include "ForConditionNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

ForConditionNode::ForConditionNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

ForConditionNode::~ForConditionNode() {

}

CompiledNode ForConditionNode::Compile(CompileInfo& info) {
	return CompiledNode();
}

void ForConditionNode::IncludeScan(ParsingInfo& info) {

}

Set<ScanType> ForConditionNode::Scan(ScanInfoStack& info) const {
	Set<ScanType> scanSet = loopInit->Scan(info);

	for (const ScanType type : loopCondition->Scan(info)) {
		scanSet.Add(type);
	}

	for (const ScanType type : loopStep->Scan(info)) {
		scanSet.Add(type);
	}

	return scanSet;
}

Mango ForConditionNode::ToMango() const {
	Mango mango = Mango(MangoType::Map);
	mango.Add("init", loopInit->ToMango());
	mango.Add("condition", loopCondition->ToMango());
	mango.Add("step", loopStep->ToMango());
	return mango;
}