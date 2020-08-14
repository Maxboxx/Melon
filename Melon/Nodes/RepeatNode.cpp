#include "RepeatNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

RepeatNode::RepeatNode(const Symbols::ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

RepeatNode::~RepeatNode() {

}

CompiledNode RepeatNode::Compile(CompileInfo& info) {
	return CompiledNode();
}

void RepeatNode::IncludeScan(ParsingInfo& info) {
	if (includeScanned) return;

	content->IncludeScan(info);
	condition->IncludeScan(info);

	includeScanned = true;
}

Set<ScanType> RepeatNode::Scan(ScanInfoStack& info) const {
	return Set<ScanType>();
}

Mango RepeatNode::ToMango() const {
	Mango mango = Mango("repeat", MangoType::Map);
	mango.Add("condition", condition->ToMango());
	mango.Add("content", content->ToMango());
	return mango;
}