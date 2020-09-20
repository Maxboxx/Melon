#include "Node.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

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

UInt Node::GetSize() const {
	return 0;
}

bool Node::IsScope() const {
	return false;
}

bool Node::IsImmediate() const {
	return false;
}

void Node::ScanAssignment(NodePtr var, NodePtr value, ScanInfoStack& info, const FileInfo& file) {
	List<ScopeList> args;
	args.Add(value->Type());
	Symbol::FindFunction(var->Type().Add(Scope::Assign), args, file);
}

CompiledNode Node::CompileAssignment(NodePtr var, NodePtr value, CompileInfo& info, const FileInfo& file) {
	List<ScopeList> args;
	args.Add(value->Type());

	Symbol assign = Symbol::FindFunction(var->Type().Add(Scope::Assign), args, file);

	if (assign.type != SymbolType::None) {
		List<NodePtr> nodes;
		nodes.Add(var);
		nodes.Add(value);

		return assign.symbolNode->Compile(nodes, info);
	}
	else {
		return CompiledNode();
	}
}

