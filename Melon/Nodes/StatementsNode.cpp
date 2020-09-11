#include "StatementsNode.h"

#include "Melon/Parsing/Parser.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

StatementsNode::StatementsNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

StatementsNode::~StatementsNode() {

}

UInt StatementsNode::GetSize() const {
	UInt size = 0;

	for (UInt i = statements.Size(); i > 0;) {
		i--;

		if (statements[i]->IsScope()) {
			size = Math::Max(size, statements[i]->GetSize());
		}
		else {
			size += statements[i]->GetSize();
		}
	}

	return size;
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

void StatementsNode::IncludeScan(ParsingInfo& info) {
	for (NodePtr statement : statements) {
		statement->IncludeScan(info);
	}
}

Set<ScanType> StatementsNode::Scan(ScanInfoStack& info) {
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