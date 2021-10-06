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

BinaryOperatorNode::BinaryOperatorNode(Symbols::Symbol* const scope, const Name& op, const FileInfo& file) : ExpressionNode(scope, file) {
	this->op = op;
}

BinaryOperatorNode::~BinaryOperatorNode() {
	
}

TypeSymbol* BinaryOperatorNode::Type() const {
	FunctionSymbol* const s = SymbolTable::FindOperator(GetOperator(), operand1->Type(), operand2->Type(), file);

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
	FunctionSymbol* const func = SymbolTable::FindOperator(GetOperator(), operand1->Type(), operand2->Type(), file);
	if (!func) return CompiledNode();

	List<Expression> nodes;
	nodes.Add(operand1);
	nodes.Add(operand2);

	// Compile symbol node
	if (func->symbolNode) {
		return func->symbolNode->Compile(nodes, info);
	}
	// Compile operator function
	else {
		Pointer<CallNode> cn = new CallNode(scope, file);
		cn->args = nodes;

		Pointer<TypeNode> tn = new TypeNode(func->Parent()->Parent()->AbsoluteName());
		cn->node = tn;

		return cn->Compile(info);
	}
}

void BinaryOperatorNode::IncludeScan(ParsingInfo& info) {
	operand1->IncludeScan(info);
	operand2->IncludeScan(info);
}

ScanResult BinaryOperatorNode::Scan(ScanInfoStack& info) {
	ScanResult result1 = operand1->Scan(info);
	result1.SelfUseCheck(info, operand1->File());

	ScanResult result2 = operand2->Scan(info);
	result2.SelfUseCheck(info, operand2->File());

	SymbolTable::FindOperator(GetOperator(), operand1->Type(), operand2->Type(), file);

	return result1 | result2;
}

NameList BinaryOperatorNode::FindSideEffectScope(const bool assign) {
	FunctionSymbol* const f = SymbolTable::FindOperator(GetOperator(), operand1->Type(), operand2->Type(), file);

	if (f->symbolNode) {
		return CombineSideEffects(operand1->GetSideEffectScope(assign), operand2->GetSideEffectScope(assign));
	}
	else {
		// TODO: Check operator function
		return CombineSideEffects(operand1->GetSideEffectScope(assign), operand2->GetSideEffectScope(assign));
	}

	return NameList();
}

Expression BinaryOperatorNode::Optimize(OptimizeInfo& info) {
	Node::Optimize(operand1, info);
	Node::Optimize(operand2, info);

	// TODO: Add more operators
	// Optimize immediate operands
	if (operand1->IsImmediate() && operand2->IsImmediate()) {
		// Bool operands
		if (operand1->Type()->AbsoluteName() == NameList::Bool && operand2->Type()->AbsoluteName() == NameList::Bool) {
			// Equal
			if (op == Name::Equal) {
				BooleanNode* const bn = new BooleanNode(operand1->File());
				bn->boolean = operand1->GetImmediate() == operand2->GetImmediate();
				info.optimized = true;
				return bn;
			}
			// Not Equal
			else if (op == Name::NotEqual) {
				BooleanNode* const bn = new BooleanNode(operand1->File());
				bn->boolean = operand1->GetImmediate() != operand2->GetImmediate();
				info.optimized = true;
				return bn;
			}
		}
	}

	return nullptr;
}

StringBuilder BinaryOperatorNode::ToMelon(const UInt indent) const {
	StringBuilder sb = operand1->ToMelon(indent);
	sb += " ";
	sb += op.ToString();
	sb += " ";
	sb += operand2->ToMelon(indent);
	return sb;
}
