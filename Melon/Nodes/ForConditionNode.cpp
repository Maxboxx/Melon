#include "ForConditionNode.h"

#include "AssignNode.h"
#include "CallNode.h"
#include "BinaryOperatorNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

ForConditionNode::ForConditionNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

ForConditionNode::~ForConditionNode() {

}

UInt ForConditionNode::GetSize() const {
	return loopInit->GetSize() + loopCondition->GetSize() + loopStep->GetSize();
}

CompiledNode ForConditionNode::Compile(CompileInfo& info) {
	return CompiledNode();
}

void ForConditionNode::IncludeScan(ParsingInfo& info) {

}

Set<ScanType> ForConditionNode::Scan(ScanInfoStack& info) {
	Set<ScanType> scanSet = loopInit->Scan(info);

	for (const ScanType type : loopCondition->Scan(info)) {
		scanSet.Add(type);
	}

	if (loopCondition.Cast<AssignNode>() != nullptr) {
		for (const ScanType type : loopCondition->Scan(info)) {
			scanSet.Add(type);
		}
	}
	else {
		if (loopCondition->Type() != ScopeList::Bool) {
			Pointer<BinaryOperatorNode> comp = new BinaryOperatorNode(loopCondition->scope, Scope::Less, loopCondition->file);
			comp->node1 = loopInit.Cast<AssignNode>()->vars[0];
			comp->node2 = loopCondition;

			for (const ScanType type : comp->Scan(info)) {
				scanSet.Add(type);
			}
		}
		else {
			for (const ScanType type : loopCondition->Scan(info)) {
				scanSet.Add(type);
			}
		}
	}

	if (loopStep.Cast<AssignNode>() != nullptr) {
		for (const ScanType type : loopStep->Scan(info)) {
			scanSet.Add(type);
		}
	}
	else {
		bool createAssign = true;

		if (Pointer<CallNode> call = loopStep.Cast<CallNode>()) {
			createAssign = !call->GetFunc().returnValues.IsEmpty();
		}

		if (createAssign) {
			Pointer<AssignNode> assign = new AssignNode(loopStep->scope, loopStep->file);
			assign->vars.Add(loopInit.Cast<AssignNode>()->vars[0]);

			Pointer<BinaryOperatorNode> add = new BinaryOperatorNode(loopStep->scope, Scope::Add, loopStep->file);
			add->node1 = assign->vars[0];
			add->node2 = loopStep;

			assign->values.Add(add);

			for (const ScanType type : assign->Scan(info)) {
				scanSet.Add(type);
			}
		}
		else {
			for (const ScanType type : loopStep->Scan(info)) {
				scanSet.Add(type);
			}
		}
	}

	return scanSet;
}

ScopeList ForConditionNode::FindSideEffectScope(const bool assign) {
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
		sb += conditionOperator.Get().ToString();
		sb += " ";
	}

	sb += loopCondition->ToMelon(indent);
	sb += ", ";

	if (stepOperator) {
		sb += stepOperator.Get().ToString();
		sb += " ";
	}

	sb += loopStep->ToMelon(indent);
	return sb;
}