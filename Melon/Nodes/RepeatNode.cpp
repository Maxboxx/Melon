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

RepeatNode::RepeatNode(const Symbols::ScopeList& scope, const FileInfo& file) : Node(scope, file) {

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

Set<ScanType> RepeatNode::Scan(ScanInfoStack& info) {
	Set<ScanType> scanSet;

	ScopeInfo scopeInfo = info.Get().scopeInfo.CopyBranch();
	info.Get().scopeInfo.EnterScope(ScopeInfo::ScopeType::Loop);

	for (const ScanType type : content->Scan(info)) {
		scanSet.Add(type);

		if (info.Get().init && type == ScanType::Self && !info.Get().symbol.IsAssigned()) {
			ErrorLog::Error(CompileError(CompileError::SelfInit, content->file));
		}
	}

	for (const ScanType type : condition->Scan(info)) {
		scanSet.Add(type);

		if (info.Get().init && type == ScanType::Self && !info.Get().symbol.IsAssigned()) {
			ErrorLog::Error(CompileError(CompileError::SelfInit, condition->file));
		}
	}

	info.Get().scopeInfo.ExitScope();
	info.Get().scopeInfo = ScopeInfo::WeakBranchUnion(scopeInfo, info.Get().scopeInfo);
	return scanSet;
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

Mango RepeatNode::ToMango() const {
	Mango mango = Mango("repeat", MangoType::Map);
	mango.Add("condition", condition->ToMango());
	mango.Add("content", content->ToMango());
	return mango;
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