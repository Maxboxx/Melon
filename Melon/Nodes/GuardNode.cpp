#include "GuardNode.h"

#include "Kiwi/Kiwi.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

GuardNode::GuardNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

GuardNode::~GuardNode() {

}

CompiledNode GuardNode::Compile(CompileInfo& info) {
	CompiledNode cn = cond->Compile(info);

	Instruction ne = Instruction(InstructionType::Ne, 1);
	ne.arguments.Add(cn.argument);
	ne.arguments.Add(Argument(0));

	UInt jumpIndex = cn.instructions.Size();
	cn.instructions.Add(ne);

	cn.AddInstructions(else_->Compile(info).instructions);

	cn.instructions.Add(Instruction::Label(info.label));
	cn.instructions[jumpIndex].instruction.arguments.Add(Argument(ArgumentType::Label, info.label++));

	return cn;
}

void GuardNode::IncludeScan(ParsingInfo& info) {
	if (includeScanned) return;

	cond->IncludeScan(info);
	else_->IncludeScan(info);

	includeScanned = true;
}

Set<ScanType> GuardNode::Scan(ScanInfoStack& info) const {
	Set<ScanType> scanSet = cond->Scan(info);

	for (const ScanType type : else_->Scan(info)) {
		scanSet.Add(type);
	}

	return scanSet;
}

Mango GuardNode::ToMango() const {
	Mango mango = Mango("guard", MangoType::Map);
	mango.Add("condition", cond->ToMango());
	mango.Add("else", else_->ToMango());
	return mango;
}
