#include "BinaryOperatorNode.h"

#include "CallNode.h"
#include "TypeNode.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

BinaryOperatorNode::BinaryOperatorNode(const ScopeList& scope, const Scope& op, const FileInfo& file) : Node(scope, file) {
	this->op = op;
}

BinaryOperatorNode::~BinaryOperatorNode() {

}

ScopeList BinaryOperatorNode::Type() const {
	Symbol s = Symbol::FindOperator(GetOperator(), node1->Type(), node2->Type(), file);

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

Scope BinaryOperatorNode::GetOperator() const {
	return op;
}

CompiledNode BinaryOperatorNode::Compile(CompileInfo& info) {
	List<NodePtr> nodes;
	nodes.Add(node1);
	nodes.Add(node2);
	Symbol s = Symbol::FindOperator(GetOperator(), node1->Type(), node2->Type(), file);

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

void BinaryOperatorNode::IncludeScan(ParsingInfo& info) {
	node1->IncludeScan(info);
	node2->IncludeScan(info);
}

Set<ScanType> BinaryOperatorNode::Scan(ScanInfoStack& info) {
	Set<ScanType> scanSet = node1->Scan(info);

	if (info.Get().init && scanSet.Contains(ScanType::Self) && !info.Get().symbol.IsAssigned()) {
		ErrorLog::Error(CompileError(CompileError::SelfInit, node1->file));
	}

	for (const ScanType type : node2->Scan(info)) {
		scanSet.Add(type);

		if (info.Get().init && type == ScanType::Self && !info.Get().symbol.IsAssigned()) {
			ErrorLog::Error(CompileError(CompileError::SelfInit, node2->file));
		}
	}

	Symbol::FindOperator(GetOperator(), node1->Type(), node2->Type(), file);

	return scanSet;
}

Mango BinaryOperatorNode::ToMango() const {
	const ScopeList op = Symbol::FindOperator(GetOperator(), node1->Type(), node2->Type(), file).scope;

	Mango mango = Mango(op.ToString(), MangoType::List);
	mango.Add(node1->ToMango());
	mango.Add(node2->ToMango());
	return mango;
}