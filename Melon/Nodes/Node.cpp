#include "Node.h"

#include "ConvertNode.h"
#include "EmptyNode.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

#include "Boxx/Math.h"

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
}

List<ScopeList> Node::Types() const {
	List<ScopeList> types;
	types.Add(Type());
	return types;
}

Set<ScanType> Node::Scan(ScanInfoStack& info) {
	return Set<ScanType>();
}

bool Node::HasSideEffects(const ScopeList& scope) {
	ScopeList list = GetSideEffectScope();

	if (list.Size() == 1 && list[0] == Scope::Global) return true;

	UInt len = Math::Min(list.Size(), scope.Size());

	for (UInt i = 0; i < len; i++) {
		if (list[i] != scope[i]) {
			return true;
		}
	}

	return list.Size() > scope.Size();
}

ScopeList Node::GetSideEffectScope() {
	if (!sideEffectScope) {
		sideEffectScope = FindSideEffectScope();
	}

	return sideEffectScope.Get();
}

ScopeList Node::FindSideEffectScope() {
	return scope;
}

ScopeList Node::CombineSideEffects(const ScopeList& scope1, const ScopeList& scope2) {
	if (scope1.Size() == 1 && scope1[0] == Scope::Global) return scope1;
	if (scope2.Size() == 1 && scope2[0] == Scope::Global) return scope2;

	UInt len = Math::Min(scope1.Size(), scope2.Size());

	for (UInt i = 0; i < len; i++) {
		if (scope1[i] != scope2[i]) {
			return true;
		}
	}

	return scope1.Size() > scope2.Size();
}

NodePtr Node::Optimize(OptimizeInfo& info) {
	return nullptr;
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

Long Node::GetImmediate() const {
	return 0;
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

		Pointer<ConvertNode> cn = new ConvertNode(value->scope, value->file);
		cn->isExplicit = false;
		cn->node = value;
		cn->type = assign.arguments[0];
		nodes.Add(cn);

		return assign.symbolNode->Compile(nodes, info);
	}
	else {
		return CompiledNode();
	}
}

bool Node::IsEmpty(const NodePtr& node) {
	if (Pointer<EmptyNode> empty = node.Cast<EmptyNode>()) {
		return !empty->node;
	}

	return false;
}

