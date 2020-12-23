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

Set<ScanType> BreakNode::Scan(ScanInfoStack& info) {
	if (!scopeWise) {
		if (info.Get().scopeInfo.CanContinue()) {
			if (!isBreak) {
				info.Get().scopeInfo.loopAbortCount = Math::Max(info.Get().scopeInfo.loopAbortCount, loops);
			}

			info.Get().scopeInfo.loopBreakCount = Math::Max(info.Get().scopeInfo.loopBreakCount, loops);
		}

		info.Get().scopeInfo.maxLoopBreakCount = Math::Max(info.Get().scopeInfo.maxLoopBreakCount, loops);
	}
	else {
		if (info.Get().scopeInfo.CanContinue()) {
			info.Get().scopeInfo.scopeBreakCount = Math::Max(info.Get().scopeInfo.scopeBreakCount, loops);
		}

		info.Get().scopeInfo.maxScopeBreakCount = Math::Max(info.Get().scopeInfo.maxScopeBreakCount, loops);
	}

	return Set<ScanType>();
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