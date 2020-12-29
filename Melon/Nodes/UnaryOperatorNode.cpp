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

UnaryOperatorNode::UnaryOperatorNode(Symbol* const scope, const Scope& op, const FileInfo& file) : Node(scope, file) {
	this->op = op;
}

UnaryOperatorNode::~UnaryOperatorNode() {

}

TypeSymbol* UnaryOperatorNode::Type() const {
	List<TypeSymbol*> args;
	args.Add(node->Type());

	TypeSymbol* const type = node->Type();

	/* TODO: node
	const Symbols s = Symbols::FindFunction(type.Add(op), args, file);

	if (s.type != SymbolType::None && !s.returnValues.IsEmpty()) {
		const Symbols s2 = Symbols::FindNearest(s.scope.Pop(), s.returnValues[0], file);

		if (s2.type == SymbolType::Template) {
			return s2.varType;
		}
		else if (s2.type != SymbolType::None) {
			return s2.scope;
		}
	}
	*/

	return nullptr;
}

Symbol* UnaryOperatorNode::GetSymbol() const {
	/* TODO: node
	if (op == Scope::Unwrap) {
		return Symbols::Find(node->Type(), file).Get(Scope::Value, file);
	}
	*/

	return nullptr;
}

Scope UnaryOperatorNode::GetOperator() const {
	return op;
}

CompiledNode UnaryOperatorNode::Compile(CompileInfo& info) {
	List<NodePtr> nodes;
	nodes.Add(node);

	List<Symbol*> args;
	args.Add(node->Type());

	Symbol* const type = node->Type();

	/* TODO: node
	const Symbols s = Symbols::FindFunction(type.Add(op), args, file);

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
	*/

	return CompiledNode();
}

void UnaryOperatorNode::IncludeScan(ParsingInfo& info) {
	node->IncludeScan(info);
}

Set<ScanType> UnaryOperatorNode::Scan(ScanInfoStack& info) {
	Set<ScanType> scanSet = node->Scan(info);

	if (op == Scope::Unwrap) {
		// TODO: fix
		ErrorLog::Warning(WarningError("unwrap operator does not work properly for nil values", file));
	}

	/* TODO: node
	if (info.Get().init && scanSet.Contains(ScanType::Self) && !info.Get().symbol.IsAssigned()) {
		ErrorLog::Error(CompileError(CompileError::SelfInit, node->file));
	}

	List<ScopeList> args;
	args.Add(node->Type());

	const ScopeList type = node->Type();

	Symbols::FindFunction(type.Add(op), args, file);
	*/

	return scanSet;
}

ScopeList UnaryOperatorNode::FindSideEffectScope(const bool assign) {
	// TODO: Check operator function
	return node->GetSideEffectScope(assign);
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