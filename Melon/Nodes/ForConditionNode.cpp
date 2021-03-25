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

ForConditionNode::ForConditionNode(Symbol* const scope, const FileInfo& file) : Node(scope, file) {

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
	result.SelfUseCheck(info, loopInit->file);

	// Scan condition without condition operator
	if (!conditionOperator) {
		ScanResult r = loopCondition->Scan(info);
		r.SelfUseCheck(info, loopCondition->file);
		result |= r;
	}
	// Scan condition with condition operator
	else {
		Pointer<BinaryOperatorNode> op = new BinaryOperatorNode(loopCondition->scope, *conditionOperator, loopCondition->file);
		op->node1 = loopInit.Cast<AssignNode>()->vars[0];
		op->node2 = loopCondition;

		ScanResult r = op->Scan(info);
		r.SelfUseCheck(info, loopCondition->file);
		result |= r;
	}

	// Scan step without step operator
	if (!stepOperator) {
		ScanResult r = loopStep->Scan(info);
		r.SelfUseCheck(info, loopStep->file);
		result |= r;
	}
	// Scan step with step operator
	else {
		Pointer<AssignNode> assign = new AssignNode(loopStep->scope, loopStep->file);
		assign->vars.Add(loopInit.Cast<AssignNode>()->vars[0]);

		Pointer<BinaryOperatorNode> op = new BinaryOperatorNode(loopStep->scope, *stepOperator, loopStep->file);
		op->node1 = assign->vars[0];
		op->node2 = loopStep;

		assign->values.Add(op);

		ScanResult r = assign->Scan(info);
		r.SelfUseCheck(info, loopStep->file);
		result |= r;
	}

	return result;
}

NameList ForConditionNode::FindSideEffectScope(const bool assign) {
	return CombineSideEffects(loopInit->GetSideEffectScope(assign), CombineSideEffects(loopCondition->GetSideEffectScope(assign), loopStep->GetSideEffectScope(assign)));
}

NodePtr ForConditionNode::Optimize(OptimizeInfo& info) {
	if (NodePtr node = loopInit->Optimize(info)) loopInit = node;
	if (NodePtr node = loopCondition->Optimize(info)) loopCondition = node;
	if (NodePtr node = loopStep->Optimize(info)) loopStep = node;

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
