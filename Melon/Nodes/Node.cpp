#include "Node.h"

#include "Melon/Parsing/Parser.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

void CompiledNode::AddInstructions(const List<OptimizerInstruction>& instructions) {
	for (const OptimizerInstruction& inst : instructions) {
		this->instructions.Add(inst);
	}
}

Node::Node(const ScopeList& scope, const FileInfo& file) {
	this->scope = scope;
	this->file = file;
}

Node::~Node() {

}

ScopeList Node::Type() const {
	return ScopeList::undefined;
};

List<ScopeList> Node::Types() const {
	List<ScopeList> types;
	types.Add(Type());
	return types;
};

Set<ScanType> Node::Scan(ScanInfoStack& info) {
	return Set<ScanType>();
}

void Node::IncludeScan(ParsingInfo& info) {
	
}

Symbol Node::GetSymbol() const {
	return Symbol();
}

bool Node::IsImmediate() const {
	return false;
}