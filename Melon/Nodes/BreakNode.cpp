#include "BreakNode.h"

#include "Boxx/Math.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;

BreakNode::BreakNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {
	
}

BreakNode::~BreakNode() {

}

CompiledNode BreakNode::Compile(CompileInfo& info) {
	CompiledNode c;

	Instruction in = Instruction(InstructionType::Custom, loops);

	if (!isBreak)
		in.instructionName = "abort";
	else if (breakBool)
		in.instructionName = "breaktrue";
	else
		in.instructionName = "breakfalse";

	c.instructions.Add(in);
	return c;
}

Set<ScanType> BreakNode::Scan(ScanInfoStack& info) const {
	if (!scopeWise) {
		if (info.Get().CanContinue()) {
			if (!isBreak) {
				info.Get().loopAbortCount = Math::Max(info.Get().loopAbortCount, loops);
			}

			info.Get().loopBreakCount = Math::Max(info.Get().loopBreakCount, loops);
		}

		info.Get().maxLoopBreakCount = Math::Max(info.Get().maxLoopBreakCount, loops);
	}
	else {
		if (info.Get().CanContinue()) {
			if (!isBreak) {
				info.Get().scopeAbortCount = Math::Max(info.Get().scopeAbortCount, loops + 1);
			}

			info.Get().scopeBreakCount = Math::Max(info.Get().scopeBreakCount, loops + 1);
		}

		info.Get().maxScopeBreakCount = Math::Max(info.Get().maxScopeBreakCount, loops + 1);
	}

	return Set<ScanType>();
};

Mango BreakNode::ToMango() const {
	Mango m = Mango(isBreak ? "break" : "abort", MangoType::List);
	m.Add(Mango((Int)loops));

	if (isBreak)
		m.Add(Mango((bool)breakBool));

	return m;
}