#include "BreakNode.h"

#include "Boxx/Math.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;

String BreakNode::abortInstName      = "abort";
String BreakNode::scopeBreakInstName = "break!";
String BreakNode::breakTrueInstName  = "breaktrue";
String BreakNode::breakFalseInstName = "breakfalse";

BreakNode::BreakNode(Symbol* const scope, const FileInfo& file) : Node(scope, file) {
	
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

ScanResult BreakNode::Scan(ScanInfoStack& info) {
	// Regular break or abort
	if (!scopeWise) {
		if (info.ScopeInfo().CanContinue()) {
			if (!isBreak) {
				info.ScopeInfo().loopAbortCount = Math::Max(info.ScopeInfo().loopAbortCount, loops);
			}

			info.ScopeInfo().loopBreakCount = Math::Max(info.ScopeInfo().loopBreakCount, loops);
		}

		info.ScopeInfo().maxLoopBreakCount = Math::Max(info.ScopeInfo().maxLoopBreakCount, loops);
	}
	// Scope wise break
	else {
		if (info.ScopeInfo().CanContinue()) {
			info.ScopeInfo().scopeBreakCount = Math::Max(info.ScopeInfo().scopeBreakCount, loops);
		}

		info.ScopeInfo().maxScopeBreakCount = Math::Max(info.ScopeInfo().maxScopeBreakCount, loops);
	}

	return ScanResult();
};

StringBuilder BreakNode::ToMelon(const UInt indent) const {
	StringBuilder sb = String(scopeWise ? "break!" : (isBreak ? "break" : "abort"));
	sb += " ";
	sb += String::ToString(scopeWise ? (Int)loops - 1 : (Int)loops);

	if (isBreak && !scopeWise) {
		sb += ", ";
		sb += String::ToString((bool)breakBool);
	}

	return sb;
}