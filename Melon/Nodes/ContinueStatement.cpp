#include "ContinueStatement.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Symbols;

ContinueStatement::ContinueStatement(Symbols::Symbol* const scope, const FileInfo& file) : Statement(scope, file) {
	
}

ContinueStatement::~ContinueStatement() {

}

Ptr<Kiwi::Value> ContinueStatement::Compile(CompileInfo& info) {
	info.currentBlock->AddInstruction(new Kiwi::GotoInstruction(info.PeekLoop().continueLabel));
	return nullptr;
}

ScanResult ContinueStatement::Scan(ScanInfoStack& info) {
	if (info->scopeInfo.CanContinue()) {
		info->scopeInfo.loopBreakCount = Math::Max(info->scopeInfo.loopBreakCount, loops);
	}

	info->scopeInfo.maxLoopBreakCount = Math::Max(info->scopeInfo.maxLoopBreakCount, loops);

	return ScanResult();
};

StringBuilder ContinueStatement::ToMelon(const UInt indent) const {
	StringBuilder sb = String("continue ");
	sb += String::ToString((Int)loops);
	return sb;
}
