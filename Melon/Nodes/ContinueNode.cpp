#include "ContinueNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;

String ContinueNode::continueInstName = "continue";

ContinueNode::ContinueNode(Symbol* const scope, const FileInfo& file) : Node(scope, file) {

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

ScanResult ContinueNode::Scan(ScanInfoStack& info) {
	if (info->scopeInfo.CanContinue()) {
		info->scopeInfo.loopBreakCount = Math::Max(info->scopeInfo.loopBreakCount, loops);
	}

	info->scopeInfo.maxLoopBreakCount = Math::Max(info->scopeInfo.maxLoopBreakCount, loops);

	return ScanResult();
};

StringBuilder ContinueNode::ToMelon(const UInt indent) const {
	StringBuilder sb = String("continue ");
	sb += String::ToString((Int)loops);
	return sb;
}
