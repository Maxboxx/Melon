#include "UnaryOperatorNode.h"

#include "CallNode.h"
#include "TypeNode.h"

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

	if (s.type != SymbolType::None && !s.ret.IsEmpty()) {
		const Symbol s2 = Symbol::FindNearest(s.scope.Pop(), s.ret[0], file);

		if (s2.type != SymbolType::None) {
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

	if (s.node) {
		return s.node->Compile(nodes, info);
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
	if (includeScanned) return;

	node->IncludeScan(info);

	includeScanned = true;
}

Set<ScanType> UnaryOperatorNode::Scan(ScanInfo& info) const {
	Set<ScanType> scanSet = node->Scan(info);

	if (info.init && scanSet.Contains(ScanType::Self) && !info.symbol.IsAssigned()) {
		ErrorLog::Error(CompileError(CompileError::SelfInit, node->file));
	}

	List<ScopeList> args;
	args.Add(node->Type());

	const ScopeList type = node->Type();
	Symbol::FindFunction(type.Add(op), args, file);

	return scanSet;
}

Mango UnaryOperatorNode::ToMango() const {
	Mango mango = Mango(Symbol::FindMethod(node->Type().Add(op), List<ScopeList>(), file).scope.ToString(), MangoType::List);
	mango.Add(node->ToMango());
	return mango;
}