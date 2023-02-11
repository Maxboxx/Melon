#include "BreakStatement.h"

#include "Boxx/Math.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Symbols;

BreakStatement::BreakStatement(Symbol* const scope, const FileInfo& file) : Statement(scope, file) {
	
}

BreakStatement::~BreakStatement() {

}

Ptr<Kiwi::Value> BreakStatement::Compile(CompileInfo& info) {
	LoopInfo loop = scopewise ? info.PeekScope(loops - 1) : info.PeekLoop(loops - 1);

	String label;

	if (isBreak) {
		label = breakBool ? loop.trueLabel : loop.falseLabel;
	}
	else {
		label = loop.endLabel;
	}

	info.currentBlock->AddInstruction(new Kiwi::GotoInstruction(label));
	return nullptr;
}

ScanResult BreakStatement::Scan(ScanInfoStack& info) {
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

StringBuilder BreakStatement::ToMelon(const UInt indent) const {
	StringBuilder sb = String(scopewise ? "break!" : (isBreak ? "break" : "abort"));
	sb += " ";
	sb += String::ToString(scopewise ? (Int)loops - 1 : (Int)loops);

	if (isBreak && !scopewise) {
		sb += ", ";
		sb += String::ToString((bool)breakBool);
	}

	return sb;
}