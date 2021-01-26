#include "RepeatNode.h"

#include "BreakNode.h"
#include "ContinueNode.h"
#include "DoNode.h"
#include "EmptyNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

RepeatNode::RepeatNode(Symbol* const scope, const FileInfo& file) : Node(scope, file) {

}

RepeatNode::~RepeatNode() {

}

UInt RepeatNode::GetSize() const {
	return content->GetSize();
}

bool RepeatNode::IsScope() const {
	return true;
}

CompiledNode RepeatNode::Compile(CompileInfo& info) {
	CompiledNode compiled;

	const UInt label = info.label++;
	compiled.instructions.Add(Instruction::Label(label));

	List<UInt> endJumps;
	List<UInt> condJumps;

	for (const OptimizerInstruction& in : content->Compile(info).instructions) {
		if (in.instruction.type != InstructionType::Custom) {
			compiled.instructions.Add(in);
			continue;
		}

		const String type = in.instruction.instructionName;

		if (
			type != BreakNode::abortInstName &&
			type != BreakNode::scopeBreakInstName &&
			type != BreakNode::breakTrueInstName &&
			type != BreakNode::breakFalseInstName &&
			type != ContinueNode::continueInstName
		) {
			compiled.instructions.Add(in);
			continue;
		}

		if (in.instruction.sizes[0] > 1) {
			OptimizerInstruction inst = in;
			inst.instruction.sizes[0]--;
			compiled.instructions.Add(inst);
		}
		else if (
			type == BreakNode::abortInstName || 
			type == BreakNode::breakTrueInstName || 
			type == BreakNode::breakFalseInstName || 
			type == BreakNode::scopeBreakInstName
		) {
			Instruction jmp = Instruction(InstructionType::Jmp);
			endJumps.Add(compiled.instructions.Size());
			compiled.instructions.Add(jmp);
		}
		else if (type == ContinueNode::continueInstName) {
			Instruction jmp = Instruction(InstructionType::Jmp);
			condJumps.Add(compiled.instructions.Size());
			compiled.instructions.Add(jmp);
		}
	}

	if (!condJumps.IsEmpty()) {
		const UInt condLbl = info.label++;
		compiled.instructions.Add(Instruction::Label(condLbl));

		for (const UInt inst : condJumps) {
			compiled.instructions[inst].instruction.arguments.Add(Argument(ArgumentType::Label, condLbl));
		}
	}

	const UInt frame = info.stack.frame;
	CompiledNode cond = condition->Compile(info);
	info.stack.PopExpr(frame, cond);
	compiled.AddInstructions(cond.instructions);

	Instruction comp = Instruction(InstructionType::Eq, 1);
	comp.arguments.Add(cond.argument);
	comp.arguments.Add(Argument(0));
	comp.arguments.Add(Argument(ArgumentType::Label, label));
	compiled.instructions.Add(comp);

	if (!endJumps.IsEmpty()) {
		const UInt endLbl = info.label++;
		compiled.instructions.Add(Instruction::Label(endLbl));

		for (const UInt inst : endJumps) {
			compiled.instructions[inst].instruction.arguments.Add(Argument(ArgumentType::Label, endLbl));
		}
	}

	return compiled;
}

void RepeatNode::IncludeScan(ParsingInfo& info) {
	content->IncludeScan(info);
	condition->IncludeScan(info);
}

ScanResult RepeatNode::Scan(ScanInfoStack& info) {
	ScopeInfo scopeInfo = info.ScopeInfo().CopyBranch();
	info.ScopeInfo().EnterScope(ScopeInfo::ScopeType::Loop);

	ScanResult result1 = content->Scan(info);
	result1.SelfUseCheck(info, content->file);

	ScanResult result2 = condition->Scan(info);
	result2.SelfUseCheck(info, condition->file);

	info.ScopeInfo().ExitScope();
	info.ScopeInfo(ScopeInfo::WeakBranchUnion(scopeInfo, info.ScopeInfo()));

	return result1 | result2;
}

ScopeList RepeatNode::FindSideEffectScope(const bool assign) {
	return CombineSideEffects(content->GetSideEffectScope(assign), condition->GetSideEffectScope(assign));
}

NodePtr RepeatNode::Optimize(OptimizeInfo& info) {
	if (NodePtr node = content->Optimize(info)) content = node;
	if (NodePtr node = condition->Optimize(info)) condition = node;

	// TODO: Remove break, continue, abort and similar
	if (condition->IsImmediate() && condition->GetImmediate() != 0) {
		if (IsEmpty(content)) {
			info.optimized = true;
			return new EmptyNode();
		}

		Pointer<DoNode> dn = new DoNode(content->scope, content->file);
		dn->nodes = content;
		info.optimized = true;
		return dn;
	}

	return nullptr;
}

StringBuilder RepeatNode::ToMelon(const UInt indent) const {
	StringBuilder sb = "repeat\n";
	sb += String('\t').Repeat(indent + 1);
	sb += content->ToMelon(indent + 1);
	sb += "\n";
	sb += String('\t').Repeat(indent);
	sb += "until ";
	sb += condition->ToMelon(indent);
	return sb;
}