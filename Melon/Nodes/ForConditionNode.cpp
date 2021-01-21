#include "ForConditionNode.h"

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
	return CompiledNode();
}

void ForConditionNode::IncludeScan(ParsingInfo& info) {

}

void ForConditionNode::Scan(ScanInfoStack& info) {
	loopInit->Scan(info);
	loopCondition->Scan(info);

	if (loopCondition.Cast<AssignNode>() != nullptr) {
		loopCondition->Scan(info);
	}
	else {
		if (loopCondition->Type()->AbsoluteName() != ScopeList::Bool) {
			Pointer<BinaryOperatorNode> comp = new BinaryOperatorNode(loopCondition->scope, Scope::Less, loopCondition->file);
			comp->node1 = loopInit.Cast<AssignNode>()->vars[0];
			comp->node2 = loopCondition;
			comp->Scan(info);
		}
		else {
			loopCondition->Scan(info);
		}
	}

	if (loopStep.Cast<AssignNode>() != nullptr) {
		loopStep->Scan(info);
	}
	else {
		bool createAssign = true;

		if (Pointer<CallNode> call = loopStep.Cast<CallNode>()) {
			createAssign = !call->GetFunc()->returnValues.IsEmpty();
		}

		if (createAssign) {
			Pointer<AssignNode> assign = new AssignNode(loopStep->scope, loopStep->file);
			assign->vars.Add(loopInit.Cast<AssignNode>()->vars[0]);

			Pointer<BinaryOperatorNode> add = new BinaryOperatorNode(loopStep->scope, Scope::Add, loopStep->file);
			add->node1 = assign->vars[0];
			add->node2 = loopStep;

			assign->values.Add(add);
			assign->Scan(info);
		}
		else {
			loopStep->Scan(info);
		}
	}
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