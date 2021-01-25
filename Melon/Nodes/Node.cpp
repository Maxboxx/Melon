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

RootNode* Node::root = nullptr;

Node::Node(Symbol* const scope, const FileInfo& file) {
	this->scope = scope;
	this->file = file;
}

Node::~Node() {

}

TypeSymbol* Node::Type() const {
	return nullptr;
}

List<TypeSymbol*> Node::Types() const {
	List<TypeSymbol*> types;
	types.Add(Type());
	return types;
}

ScanResult Node::Scan(ScanInfoStack& info) {
	return ScanResult();
}

bool Node::HasSideEffects() {
	return HasSideEffects(scope->AbsoluteName());
}

bool Node::HasSideEffects(const ScopeList& scope) {
	ScopeList list = GetSideEffectScope(false);

	if (list.Size() == 1 && list[0] == Scope::Global) return true;

	UInt len = Math::Min(list.Size(), scope.Size());

	for (UInt i = 0; i < len; i++) {
		if (list[i] != scope[i]) {
			return true;
		}
	}

	return list.Size() < scope.Size();
}

ScopeList Node::GetSideEffectScope(const bool assign) {
	if (!sideEffectScope) {
		sideEffectScope = FindSideEffectScope(assign);
	}

	return sideEffectScope.Get();
}

ScopeList Node::FindSideEffectScope(const bool assign) {
	return scope->AbsoluteName();
}

ScopeList Node::CombineSideEffects(const ScopeList& scope1, const ScopeList& scope2) {
	if (scope1.Size() == 1 && scope1[0] == Scope::Global) return scope1;
	if (scope2.Size() == 1 && scope2[0] == Scope::Global) return scope2;

	UInt len = Math::Min(scope1.Size(), scope2.Size());

	for (UInt i = 0; i < len; i++) {
		if (scope1[i] != scope2[i]) {
			return ScopeList(true);
		}
	}

	if (scope1.Size() < scope2.Size()) {
		return scope1;
	}
	else {
		return scope2;
	}
}

NodePtr Node::Optimize(OptimizeInfo& info) {
	return nullptr;
}

void Node::IncludeScan(ParsingInfo& info) {
	
}

Symbol* Node::GetSymbol() const {
	return nullptr;
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

ScanResult Node::ScanAssignment(NodePtr var, NodePtr value, ScanInfoStack& info, const FileInfo& file) {
	List<Symbol*> args;
	args.Add(value->Type());

	/* TODO: node
	Symbols::FindFunction(var->Type().Add(Scope::Assign), args, file);
	*/

	return ScanResult();
}

CompiledNode Node::CompileAssignment(NodePtr var, NodePtr value, CompileInfo& info, const FileInfo& file) {
	List<Symbol*> args;
	args.Add(value->Type());

	/* TODO: node
	Symbols assign = Symbols::FindFunction(var->Type().Add(Scope::Assign), args, file);

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
	*/

	return CompiledNode();
}

bool Node::IsEmpty(const NodePtr& node) {
	if (Pointer<EmptyNode> empty = node.Cast<EmptyNode>()) {
		return !empty->node;
	}

	return false;
}

