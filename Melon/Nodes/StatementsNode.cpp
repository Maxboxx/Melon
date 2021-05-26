#include "StatementsNode.h"

#include "LoopNode.h";
#include "SwitchNode.h";
#include "RepeatNode.h";
#include "GuardNode.h";
#include "FunctionNode.h";
#include "StructNode.h";
#include "EnumNode.h";
#include "DoNode.h";
#include "EmptyNode.h";

#include "Melon/Parsing/Parser.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

StatementsNode::StatementsNode(Symbol* const scope, const FileInfo& file) : StatementNode(scope, file) {

}

StatementsNode::~StatementsNode() {
	for (StatementNode* const statement : statements) {
		delete statement;
	}
}

UInt StatementsNode::GetSize() const {
	UInt size = 0;

	for (UInt i = statements.Size(); i > 0;) {
		i--;

		if (statements[i]->IsScope()) {
			size = Math::Max(size, statements[i]->GetSize());
		}
		else {
			size += statements[i]->GetSize();
		}
	}

	return size;
}

void StatementsNode::IncludeScan(ParsingInfo& info) {
	for (StatementNode* const statement : statements) {
		statement->IncludeScan(info);
	}
}

ScanResult StatementsNode::Scan(ScanInfoStack& info) {
	ScanResult result;

	for (StatementNode* const statement : statements) {
		ScanResult r = statement->Scan(info);
		r.SelfUseCheck(info, statement->File());
		result |= r;
	}

	return result;
}

StatementNode* StatementsNode::Optimize(OptimizeInfo& info) {
	if (!HasSideEffects()) {
		info.optimized = true;
		return new EmptyNode();
	}

	for (StatementNode*& statement : statements) {
		Node::Optimize(statement, info);
	}

	for (UInt i = 0; i < statements.Size(); i++) {
		if (IsEmpty(statements[i])) {
			statements.RemoveAt(i);
			i--;
			info.optimized = true;
		}
	}

	if (statements.IsEmpty()) {
		info.optimized = true;
		return new EmptyNode();
	}

	return nullptr;
}

CompiledNode StatementsNode::Compile(CompileInfo& info) {
	CompiledNode c;

	for (StatementNode* const node : statements) {
		info.index = 0;
		c.AddInstructions(node->Compile(info).instructions);
	}

	info.index = 0;

	return c;
}

NameList StatementsNode::FindSideEffectScope(const bool assign) {
	if (statements.IsEmpty()) return scope->AbsoluteName();

	NameList list = statements[0]->GetSideEffectScope(assign);

	for (UInt i = 1; i < statements.Size(); i++) {
		list = CombineSideEffects(list, statements[i]->GetSideEffectScope(assign));
	}

	return list;
}

StringBuilder StatementsNode::ToMelon(const UInt indent) const {
	StringBuilder sb;
	String tabs = String('\t').Repeat(indent);

	bool prevSpace = false;

	for (UInt i = 0; i < statements.Size(); i++) {
		bool hasSpace = StatementsNode::HasSpaceAround(statements[i]);

		if (i > 0) {
			if (hasSpace || prevSpace) sb += "\n";
			sb += "\n";
			sb += tabs;
		}

		sb += statements[i]->ToMelon(indent);
		prevSpace = hasSpace;
	}

	return sb;
}

bool StatementsNode::HasSpaceAround(StatementNode* const node) {
	if (node->Is<LoopNode>())     return true;
	if (node->Is<SwitchNode>())   return true;
	if (node->Is<DoNode>())       return true;
	if (node->Is<RepeatNode>())   return true;
	if (node->Is<FunctionNode>()) return true;
	if (node->Is<StructNode>())   return true;
	if (node->Is<EnumNode>())     return true;

	if (GuardNode* const guardNode = node->Cast<GuardNode>()) {
		return guardNode->else_ != nullptr;
	}

	if (EmptyNode* const empty = node->Cast<EmptyNode>()) {
		return HasSpaceAround(empty->node);
	}

	return false;
}
