#include "UnaryOperatorNode.h"

#include "CallNode.h"
#include "TypeNode.h"
#include "BooleanNode.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

UnaryOperatorNode::UnaryOperatorNode(const ScopeList& scope, const Scope& op, const FileInfo& file) : Node(scope, file) {
	this->op = op;
}

UnaryOperatorNode::~UnaryOperatorNode() {

}

ScopeList UnaryOperatorNode::Type() const {
	List<ScopeList> args;
	args.Add(node->Type());

	const ScopeList type = node->Type();
	const Symbol s = Symbol::FindFunction(type.Add(op), args, file);

	if (s.type != SymbolType::None && !s.returnValues.IsEmpty()) {
		const Symbol s2 = Symbol::FindNearest(s.scope.Pop(), s.returnValues[0], file);

		if (s2.type == SymbolType::Template) {
			return s2.varType;
		}
		else if (s2.type != SymbolType::None) {
			return s2.scope;
		}
	}

	return ScopeList::undefined;
}

Scope UnaryOperatorNode::GetOperator() const {
	return op;
}

CompiledNode UnaryOperatorNode::Compile(CompileInfo& info) {
	List<NodePtr> nodes;
	nodes.Add(node);

	List<ScopeList> args;
	args.Add(node->Type());

	const ScopeList type = node->Type();
	const Symbol s = Symbol::FindFunction(type.Add(op), args, file);

	if (s.symbolNode) {
		return s.symbolNode->Compile(nodes, info);
	}
	else {
		Pointer<CallNode> cn = new CallNode(scope, file);
		cn->args = nodes;
		cn->isMethod = false;
		Scope sc = s.scope.Last();
		sc.variant = nullptr;
		Pointer<TypeNode> tn = new TypeNode(s.scope.Pop().Add(sc));
		cn->node = tn;
		cn->op = true;
		return cn->Compile(info);
	}
}

void UnaryOperatorNode::IncludeScan(ParsingInfo& info) {
	node->IncludeScan(info);
}

Set<ScanType> UnaryOperatorNode::Scan(ScanInfoStack& info) {
	Set<ScanType> scanSet = node->Scan(info);

	if (info.Get().init && scanSet.Contains(ScanType::Self) && !info.Get().symbol.IsAssigned()) {
		ErrorLog::Error(CompileError(CompileError::SelfInit, node->file));
	}

	List<ScopeList> args;
	args.Add(node->Type());

	const ScopeList type = node->Type();
	Symbol::FindFunction(type.Add(op), args, file);

	return scanSet;
}

NodePtr UnaryOperatorNode::Optimize(OptimizeInfo& info) {
	if (NodePtr n = node->Optimize(info)) node = n;

	// TODO: Add more operators
	if (node->IsImmediate()) {
		if (op == Scope::Not) {
			Pointer<BooleanNode> bn = new BooleanNode(node->file);
			bn->boolean = node->GetImmediate() == 0;
			info.optimized = true;
			return bn;
		}
	}

	return nullptr;
}

Mango UnaryOperatorNode::ToMango() const {
	List<ScopeList> args;
	args.Add(node->Type());

	const ScopeList type = node->Type();
	const Symbol s = Symbol::FindFunction(type.Add(op), args, file);

	Mango mango = Mango(s.scope.ToString(), MangoType::List);
	mango.Add(node->ToMango());
	return mango;
}

StringBuilder UnaryOperatorNode::ToMelon(const UInt indent) const {
	if (op == Scope::Unwrap) {
		StringBuilder sb = node->ToMelon(indent);
		sb += op.ToString();
		return sb;
	}
	else {
		StringBuilder sb = op.ToString();

		if (op == Scope::Not) sb += " ";

		sb += node->ToMelon(indent);
		return sb;
	}
}