#include "BinaryOperatorNode.h"

#include "CallNode.h"
#include "TypeNode.h"
#include "BooleanNode.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/TemplateSymbol.h"
#include "Melon/Symbols/FunctionSymbol.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

BinaryOperatorNode::BinaryOperatorNode(Symbol* const scope, const Name& op, const FileInfo& file) : Node(scope, file) {
	this->op = op;
}

BinaryOperatorNode::~BinaryOperatorNode() {

}

TypeSymbol* BinaryOperatorNode::Type() const {
	FunctionSymbol* const s = SymbolTable::FindOperator(GetOperator(), node1->Type(), node2->Type(), file);

	if (s && !s->returnValues.IsEmpty()) {
		if (TypeSymbol* const s2 = s->ReturnType(0)) {
			if (TemplateSymbol* const t = s2->Cast<TemplateSymbol>()) {
				return t->Type();
			}
			
			return s2;
		}
	}

	return nullptr;
}

Name BinaryOperatorNode::GetOperator() const {
	return op;
}

CompiledNode BinaryOperatorNode::Compile(CompileInfo& info) {
	FunctionSymbol* const func = SymbolTable::FindOperator(GetOperator(), node1->Type(), node2->Type(), file);
	if (!func) return CompiledNode();

	List<NodePtr> nodes;
	nodes.Add(node1);
	nodes.Add(node2);

	// Compile symbol node
	if (func->symbolNode) {
		return func->symbolNode->Compile(nodes, info);
	}
	// Compile operator function
	else {
		Pointer<CallNode> cn = new CallNode(scope, file);
		cn->args = nodes;
		cn->isMethod = false;

		Pointer<TypeNode> tn = new TypeNode(func->Parent()->Parent()->AbsoluteName());
		cn->node = tn;
		cn->op = true;
		return cn->Compile(info);
	}
}

void BinaryOperatorNode::IncludeScan(ParsingInfo& info) {
	node1->IncludeScan(info);
	node2->IncludeScan(info);
}

ScanResult BinaryOperatorNode::Scan(ScanInfoStack& info) {
	ScanResult result1 = node1->Scan(info);
	result1.SelfUseCheck(info, node1->file);

	ScanResult result2 = node2->Scan(info);
	result2.SelfUseCheck(info, node2->file);

	SymbolTable::FindOperator(GetOperator(), node1->Type(), node2->Type(), file);

	return result1 | result2;
}

NameList BinaryOperatorNode::FindSideEffectScope(const bool assign) {
	FunctionSymbol* const f = SymbolTable::FindOperator(GetOperator(), node1->Type(), node2->Type(), file);

	if (f->symbolNode) {
		return CombineSideEffects(node1->GetSideEffectScope(assign), node2->GetSideEffectScope(assign));
	}
	else {
		// TODO: Check operator function
		return CombineSideEffects(node1->GetSideEffectScope(assign), node2->GetSideEffectScope(assign));
	}

	return NameList();
}

NodePtr BinaryOperatorNode::Optimize(OptimizeInfo& info) {
	if (NodePtr node = node1->Optimize(info)) node1 = node;
	if (NodePtr node = node2->Optimize(info)) node2 = node;

	// TODO: Add more operators
	// Optimize immediate operands
	if (node1->IsImmediate() && node2->IsImmediate()) {
		// Bool operands
		if (node1->Type()->AbsoluteName() == NameList::Bool && node2->Type()->AbsoluteName() == NameList::Bool) {
			// Equal
			if (op == Name::Equal) {
				Pointer<BooleanNode> bn = new BooleanNode(node1->file);
				bn->boolean = node1->GetImmediate() == node2->GetImmediate();
				info.optimized = true;
				return bn;
			}
			// Not Equal
			else if (op == Name::NotEqual) {
				Pointer<BooleanNode> bn = new BooleanNode(node1->file);
				bn->boolean = node1->GetImmediate() != node2->GetImmediate();
				info.optimized = true;
				return bn;
			}
		}
	}

	return nullptr;
}

StringBuilder BinaryOperatorNode::ToMelon(const UInt indent) const {
	StringBuilder sb = node1->ToMelon(indent);
	sb += " ";
	sb += op.ToString();
	sb += " ";
	sb += node2->ToMelon(indent);
	return sb;
}
