#include "ContinueStatement.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Symbols;

String ContinueStatement::continueInstName = "continue";

ContinueStatement::ContinueStatement(Symbols::Symbol* const scope, const FileInfo& file) : Statement(scope, file) {
	
}

ContinueStatement::~ContinueStatement() {

}

CompiledNode ContinueStatement::Compile(OldCompileInfo& info) {
	CompiledNode c;

	Instruction in = Instruction(InstructionType::Custom, loops);
	in.instructionName = continueInstName;
	c.instructions.Add(in);

	return c;
}

Ptr<Kiwi::Value> ContinueStatement::Compile(CompileInfo& info) {
	info.currentBlock->AddInstruction(new Kiwi::GotoInstruction(info.loops.Peek().continueLabel));
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
