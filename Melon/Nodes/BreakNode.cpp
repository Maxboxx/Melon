#include "BreakNode.h"

#include "Boxx/Math.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;

String BreakNode::abortInstName = "abort";
String BreakNode::scopeBreakInstName = "break!";
String BreakNode::breakTrueInstName = "breaktrue";
String BreakNode::breakFalseInstName = "breakfalse";

BreakNode::BreakNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {
	
}

BreakNode::~BreakNode() {

}

CompiledNode BreakNode::Compile(CompileInfo& info) {
	CompiledNode c;

	Instruction in = Instruction(InstructionType::Custom, loops);

	if (!isBreak)
		in.instructionName = abortInstName;
	else if (scopeWise)
		in.instructionName = scopeBreakInstName;
	else if (breakBool)
		in.instructionName = breakTrueInstName;
	else
		in.instructionName = breakFalseInstName;

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
			info.Get().scopeBreakCount = Math::Max(info.Get().scopeBreakCount, loops);
		}

		info.Get().maxScopeBreakCount = Math::Max(info.Get().maxScopeBreakCount, loops);
	}

	return Set<ScanType>();
};

Mango BreakNode::ToMango() const {
	Mango m = Mango(scopeWise ? "break!" : (isBreak ? "break" : "abort"), MangoType::List);
	m.Add(Mango(scopeWise ? (Int)loops - 1 : (Int)loops));

	if (isBreak && !scopeWise)
		m.Add(Mango((bool)breakBool));

	return m;
}