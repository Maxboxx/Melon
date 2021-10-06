#include "ForConditionNode.h"

#include "Boxx/Error.h"

#include "AssignNode.h"
#include "CallNode.h"
#include "BinaryOperatorNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

ForConditionNode::ForConditionNode(Symbols::Symbol* const scope, const FileInfo& file) : ExpressionNode(scope, file) {

}

ForConditionNode::~ForConditionNode() {

}

UInt ForConditionNode::GetSize() const {
	return loopInit->GetSize() + loopCondition->GetSize() + loopStep->GetSize();
}

CompiledNode ForConditionNode::Compile(CompileInfo& info) {
	throw NotSupportedError("ForConditionNodes can not compile themselves");
}

void ForConditionNode::IncludeScan(ParsingInfo& info) {
	loopInit->IncludeScan(info);
	loopCondition->IncludeScan(info);

	if (loopStep) loopStep->IncludeScan(info);
}

ScanResult ForConditionNode::Scan(ScanInfoStack& info) {
	ScanResult result = loopInit->Scan(info);
	result.SelfUseCheck(info, loopInit->File());

	// Scan condition without condition operator
	if (!conditionOperator) {
		ScanResult r = loopCondition->Scan(info);
		r.SelfUseCheck(info, loopCondition->File());
		result |= r;
	}
	// Scan condition with condition operator
	else {
		Pointer<BinaryOperatorNode> op = new BinaryOperatorNode(loopCondition->scope, *conditionOperator, loopCondition->File());
		op->operand1 = loopInit.Cast<AssignNode>()->assignableValues[0];
		op->operand2 = loopCondition;

		ScanResult r = op->Scan(info);
		r.SelfUseCheck(info, loopCondition->File());
		result |= r;
	}

	// Scan step without step operator
	if (!stepOperator) {
		ScanResult r = loopStep->Scan(info);
		r.SelfUseCheck(info, loopStep->File());
		result |= r;
	}
	// Scan step with step operator
	else {
		Pointer<AssignNode> assign = new AssignNode(loopStep->scope, loopStep->File());
		assign->assignableValues.Add(loopInit.Cast<AssignNode>()->assignableValues[0]);

		Pointer<BinaryOperatorNode> op = new BinaryOperatorNode(loopStep->scope, *stepOperator, loopStep->File());
		op->operand1 = assign->assignableValues[0];
		op->operand2 = loopStep;

		assign->values.Add(op);

		ScanResult r = assign->Scan(info);
		r.SelfUseCheck(info, loopStep->File());
		result |= r;
	}

	return result;
}

NameList ForConditionNode::FindSideEffectScope(const bool assign) {
	return CombineSideEffects(loopInit->GetSideEffectScope(assign), CombineSideEffects(loopCondition->GetSideEffectScope(assign), loopStep->GetSideEffectScope(assign)));
}

Expression ForConditionNode::Optimize(OptimizeInfo& info) {
	Node::Optimize(loopInit, info);
	Node::Optimize(loopCondition, info);
	Node::Optimize(loopStep, info);
	return nullptr;
}

StringBuilder ForConditionNode::ToMelon(const UInt indent) const {
	StringBuilder sb = loopInit->ToMelon(indent);
	sb += ", ";

	if (conditionOperator) {
		sb += conditionOperator->ToSimpleString();
		sb += " ";
	}

	sb += loopCondition->ToMelon(indent);
	sb += ", ";

	if (stepOperator) {
		sb += stepOperator->ToSimpleString();
		sb += " ";
	}

	sb += loopStep->ToMelon(indent);
	return sb;
}
