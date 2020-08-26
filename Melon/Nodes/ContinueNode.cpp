#include "ContinueNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;

String ContinueNode::continueInstName = "continue";

ContinueNode::ContinueNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

ContinueNode::~ContinueNode() {

}

CompiledNode ContinueNode::Compile(CompileInfo& info) {
	CompiledNode c;

	Instruction in = Instruction(InstructionType::Custom, loops);
	in.instructionName = continueInstName;
	c.instructions.Add(in);

	return c;
}

Set<ScanType> ContinueNode::Scan(ScanInfoStack& info) {
	if (info.Get().scopeInfo.CanContinue()) {
		info.Get().scopeInfo.loopBreakCount = Math::Max(info.Get().scopeInfo.loopBreakCount, loops);
	}

	info.Get().scopeInfo.maxLoopBreakCount = Math::Max(info.Get().scopeInfo.maxLoopBreakCount, loops);

	return Set<ScanType>();
};

Mango ContinueNode::ToMango() const {
	return Mango("continue", (Int)loops);
}
