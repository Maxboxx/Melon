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
		if (loopCondition->Type() != ScopeList().Add(Scope::Bool)) {
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

Mango ForConditionNode::ToMango() const {
	Mango mango = Mango(MangoType::Map);
	mango.Add("init", loopInit->ToMango());
	mango.Add("condition", loopCondition->ToMango());
	mango.Add("step", loopStep->ToMango());
	return mango;
}

StringBuilder ForConditionNode::ToMelon(const UInt indent) const {
	StringBuilder sb = loopInit->ToMelon(indent);
	sb += ", ";
	sb += loopCondition->ToMelon(indent);
	sb += ", ";
	sb += loopStep->ToMelon(indent);
	return sb;
}