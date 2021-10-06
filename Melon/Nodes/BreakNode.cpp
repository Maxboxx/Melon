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

BreakNode::BreakNode(Symbol* const scope, const FileInfo& file) : StatementNode(scope, file) {
	
}

BreakNode::~BreakNode() {

}

CompiledNode BreakNode::Compile(CompileInfo& info) {
	CompiledNode c;

	Instruction in = Instruction(InstructionType::Custom, loops);

	if (!isBreak)
		in.instructionName = abortInstName;
	else if (scopewise)
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
	if (!scopewise) {
		if (info->scopeInfo.CanContinue()) {
			if (!isBreak) {
				info->scopeInfo.loopAbortCount = Math::Max(info->scopeInfo.loopAbortCount, loops);
			}

			info->scopeInfo.loopBreakCount = Math::Max(info->scopeInfo.loopBreakCount, loops);
		}

		info->scopeInfo.maxLoopBreakCount = Math::Max(info->scopeInfo.maxLoopBreakCount, loops);
	}
	// Scope wise break
	else {
		if (info->scopeInfo.CanContinue()) {
			info->scopeInfo.scopeBreakCount = Math::Max(info->scopeInfo.scopeBreakCount, loops);
		}

		info->scopeInfo.maxScopeBreakCount = Math::Max(info->scopeInfo.maxScopeBreakCount, loops);
	}

	return ScanResult();
};

StringBuilder BreakNode::ToMelon(const UInt indent) const {
	StringBuilder sb = String(scopewise ? "break!" : (isBreak ? "break" : "abort"));
	sb += " ";
	sb += String::ToString(scopewise ? (Int)loops - 1 : (Int)loops);

	if (isBreak && !scopewise) {
		sb += ", ";
		sb += String::ToString((bool)breakBool);
	}

	return sb;
}