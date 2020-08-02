#include "StatementsNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

StatementsNode::StatementsNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

StatementsNode::~StatementsNode() {

}

CompiledNode StatementsNode::Compile(CompileInfo& info) {
	CompiledNode c;

	for (NodePtr node : statements) {
		info.index = 0;
		c.AddInstructions(node->Compile(info).instructions);
	}

	info.index = 0;

	return c;
}

Set<ScanType> StatementsNode::Scan(ScanInfo& info) const {
	Set<ScanType> scanSet = Set<ScanType>();

	for (const NodePtr& node : statements) {
		for (const ScanType& type : node->Scan(info)) {
			scanSet.Add(type);
		}
	}

	return scanSet;
}

Mango StatementsNode::ToMango() const {
	Mango m = Mango(MangoType::List);

	for (NodePtr node : statements)
		m.Add(node->ToMango());

	return m;
}