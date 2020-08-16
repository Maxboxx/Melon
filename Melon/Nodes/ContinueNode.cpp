#include "ContinueNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;

ContinueNode::ContinueNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

ContinueNode::~ContinueNode() {

}

CompiledNode ContinueNode::Compile(CompileInfo& info) {
	CompiledNode c;

	Instruction in = Instruction(InstructionType::Custom, loops);
	in.instructionName = "continue";
	c.instructions.Add(in);

	return c;
}

Set<ScanType> ContinueNode::Scan(ScanInfoStack& info) const {
	if (!scopeWise) {
		if (info.Get().CanContinue()) {
			info.Get().loopBreakCount = Math::Max(info.Get().loopBreakCount, loops);
		}

		info.Get().maxLoopBreakCount = Math::Max(info.Get().maxLoopBreakCount, loops);
	}
	else {
		if (info.Get().CanContinue()) {
			info.Get().scopeBreakCount = Math::Max(info.Get().scopeBreakCount, loops + 1);
		}

		info.Get().maxScopeBreakCount = Math::Max(info.Get().maxScopeBreakCount, loops + 1);
	}

	return Set<ScanType>();
};

Mango ContinueNode::ToMango() const {
	return Mango("continue", (Int)loops);
}
