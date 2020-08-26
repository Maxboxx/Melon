#include "LoopNode.h"

#include "Boxx/Math.h"

#include "ForConditionNode.h"
#include "BreakNode.h"
#include "ContinueNode.h"
#include "AssignNode.h"
#include "CallNode.h"
#include "BinaryOperatorNode.h"

#include "Melon/Parsing/Parser.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

#define guard(x) if (x) {} 

LoopNode::LoopNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

LoopNode::~LoopNode() {

}

CompiledNode LoopNode::Compile(CompileInfo& info) {
	CompiledNode c;

	SegmentInfo segmentInfo;
	segmentInfo.segmentLabels = Array<UInt>(segments.Size() + 1);
	segmentInfo.stackTop = info.stack.top;

	for (UInt i = 0; i < segments.Size(); i++) {
		segmentInfo.index = i;

		switch (segments[i].type) {
			case LoopType::If:    CompileIfSegment(c, info, segmentInfo);    break;
			case LoopType::While: CompileWhileSegment(c, info, segmentInfo); break;
			case LoopType::For:   CompileForSegment(c, info, segmentInfo);   break;
			case LoopType::None:  CompileNoneSegment(c, info, segmentInfo);  break;
		}

		info.stack.top = segmentInfo.stackTop;
	}

	Instruction endLbl = Instruction::Label(info.label);
	segmentInfo.segmentLabels[segmentInfo.segmentLabels.Size() - 1] = info.label++;
	c.instructions.Add(endLbl);

	for (const Tuple<UInt, UInt>& jump : segmentInfo.jumps) {
		c.instructions[jump.value1].instruction.arguments.Add(Argument(ArgumentType::Label, segmentInfo.segmentLabels[jump.value2]));
	}

	return c;
}

void LoopNode::GetNextSegments(const UInt segment, UInt& nextTrue, UInt& nextFalse) const {
	nextTrue = segments.Size();
	nextFalse = segment + 1;

	if (IsSegmentLast(segment)) {
		if (segments[segment].also != segments[nextFalse].also) {
			nextFalse = segments.Size();
		}

		if (segment == 0) {
			nextTrue = segments.Size();

			for (UInt u = 1; u < segments.Size(); u++) {
				if (segments[u].also) {
					nextTrue = u;
					break;
				}
			}

			nextFalse = segments.Size();

			for (UInt u = 1; u < segments.Size(); u++) {
				if (!segments[u].also) {
					nextFalse = u;
					break;
				}
			}
		}
	}
}

bool LoopNode::IsSegmentLast(const UInt segment) const {
	if (segments.Size() == 1 && segment == 0) return true;
	if (segment == segments.Size() - 1) return true;

	for (UInt i = 1; i < segments.Size(); i++) {
		if (segments[i - 1].also && !segments[i].also) {
			if (segment == i - 1) return true;
			break;
		} 
	}

	return false;
}

void LoopNode::AddLabelIfNeeded(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo) const {
	bool needsLabel = false;

	for (const Tuple<UInt, UInt>& next : segmentInfo.jumps) {
		if (next.value2 == segmentInfo.index) {
			needsLabel = true;
			break;
		}
	}

	if (needsLabel) {
		Instruction lbl = Instruction::Label(info.label);
		segmentInfo.segmentLabels[segmentInfo.index] = info.label++;
		compiled.instructions.Add(lbl);
	}
}

void LoopNode::CompileIfSegment(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo) const {
	UInt nextTrue, nextFalse;
	GetNextSegments(segmentInfo.index, nextTrue, nextFalse);
	AddLabelIfNeeded(compiled, info, segmentInfo);

	LoopSegment segment = segments[segmentInfo.index];

	CompiledNode cond = segment.condition->Compile(info);
	compiled.AddInstructions(cond.instructions);

	Instruction eq = Instruction(InstructionType::Eq, 1);
	eq.arguments.Add(cond.argument);
	eq.arguments.Add(Argument(0));
	segmentInfo.jumps.Add(Tuple<UInt, UInt>(compiled.instructions.Size(), nextFalse));
	compiled.instructions.Add(eq);

	for (const OptimizerInstruction& in : segments[segmentInfo.index].statements->Compile(info).instructions) {
		if (in.instruction.type != InstructionType::Custom) {
			compiled.instructions.Add(in);
			continue;
		}

		const String type = in.instruction.instructionName;

		if (type != BreakNode::scopeBreakInstName) {
			compiled.instructions.Add(in);
			continue;
		}

		if (in.instruction.sizes[0] > 1) {
			OptimizerInstruction inst = in;
			inst.instruction.sizes[0]--;
			compiled.instructions.Add(inst);
		}
		else {
			Instruction jmp = Instruction(InstructionType::Jmp);
			segmentInfo.jumps.Add(Tuple<UInt, UInt>(compiled.instructions.Size(), IsSegmentLast(segmentInfo.index) ? segments.Size() : nextTrue));
			compiled.instructions.Add(jmp);
		}
	}
	
	if (!IsSegmentLast(segmentInfo.index)) {
		if (nextTrue != segmentInfo.index + 1 && (segment.type == LoopType::If || segment.type == LoopType::None)) {
			Instruction jmp = Instruction(InstructionType::Jmp);
			segmentInfo.jumps.Add(Tuple<UInt, UInt>(compiled.instructions.Size(), nextTrue));
			compiled.instructions.Add(jmp);
		}
	}
}

void LoopNode::CompileWhileSegment(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo) const {
	UInt nextTrue, nextFalse;
	GetNextSegments(segmentInfo.index, nextTrue, nextFalse);
	AddLabelIfNeeded(compiled, info, segmentInfo);

	LoopSegment segment = segments[segmentInfo.index];
	bool isLast = IsSegmentLast(segmentInfo.index);

	LoopInfo loopInfo;
	CompileWhileStart(compiled, info, segmentInfo, loopInfo);
	CompileLoopBody(compiled, info, segmentInfo, loopInfo);
	CompileWhileEnd(compiled, info, segmentInfo, loopInfo);
}

void LoopNode::CompileWhileStart(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo, LoopInfo& loopInfo) const {
	bool isLast = IsSegmentLast(segmentInfo.index);

	if (!isLast) {
		info.stack.Push(1);
		loopInfo.stack = info.stack.top;

		Instruction mov = Instruction(InstructionType::Mov, 1);
		mov.arguments.Add(Argument(MemoryLocation(info.stack.Offset(loopInfo.stack))));
		mov.arguments.Add(Argument(0));
		compiled.instructions.Add(mov);
	}

	Instruction lbl = Instruction::Label(info.label);
	loopInfo.loopLbl = info.label++;
	compiled.instructions.Add(lbl);

	CompiledNode cond = segments[segmentInfo.index].condition->Compile(info);
	compiled.AddInstructions(cond.instructions);

	Instruction eq = Instruction(InstructionType::Eq, 1);
	eq.arguments.Add(cond.argument);
	eq.arguments.Add(Argument(0));

	if (!isLast)
		loopInfo.loopEndJmp = compiled.instructions.Size();
	else
		segmentInfo.jumps.Add(Tuple<UInt, UInt>(compiled.instructions.Size(), segments.Size()));

	compiled.instructions.Add(eq);

	if (!isLast) {
		Instruction mov = Instruction(InstructionType::Mov, 1);
		mov.arguments.Add(Argument(MemoryLocation(info.stack.Offset(loopInfo.stack))));
		mov.arguments.Add(Argument(1));
		compiled.instructions.Add(mov);
	}
}

void LoopNode::CompileWhileEnd(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo, LoopInfo& loopInfo) const {
	UInt nextTrue, nextFalse;
	GetNextSegments(segmentInfo.index, nextTrue, nextFalse);

	Instruction jmp = Instruction(InstructionType::Jmp);
	jmp.arguments.Add(Argument(ArgumentType::Label, loopInfo.loopLbl));
	compiled.instructions.Add(jmp);

	info.stack.Pop(1);

	if (!IsSegmentLast(segmentInfo.index)) {
		Instruction lbl = Instruction::Label(info.label);
		loopInfo.loopEndLbl = info.label++;
		compiled.instructions[loopInfo.loopEndJmp].instruction.arguments.Add(Argument(ArgumentType::Label, loopInfo.loopEndLbl));
		compiled.instructions.Add(lbl);

		if (nextTrue == segmentInfo.index + 1) {
			Instruction eq = Instruction(InstructionType::Eq, 1);
			eq.arguments.Add(Argument(MemoryLocation(info.stack.Offset(loopInfo.stack))));
			eq.arguments.Add(Argument(0));
			segmentInfo.jumps.Add(Tuple<UInt, UInt>(compiled.instructions.Size(), nextFalse));
			compiled.instructions.Add(eq);
		}
		else if (nextFalse == segmentInfo.index + 1) {
			Instruction ne = Instruction(InstructionType::Ne, 1);
			ne.arguments.Add(Argument(MemoryLocation(info.stack.Offset(loopInfo.stack))));
			ne.arguments.Add(Argument(0));
			segmentInfo.jumps.Add(Tuple<UInt, UInt>(compiled.instructions.Size(), nextTrue));
			compiled.instructions.Add(ne);
		}
		else {
			Instruction ne = Instruction(InstructionType::Ne, 1);
			ne.arguments.Add(Argument(MemoryLocation(info.stack.Offset(loopInfo.stack))));
			ne.arguments.Add(Argument(0));
			segmentInfo.jumps.Add(Tuple<UInt, UInt>(compiled.instructions.Size(), nextTrue));
			compiled.instructions.Add(ne);

			Instruction jmp = Instruction(InstructionType::Jmp);
			segmentInfo.jumps.Add(Tuple<UInt, UInt>(compiled.instructions.Size(), nextFalse));
			compiled.instructions.Add(jmp);
		}
	}
}

void LoopNode::CompileForSegment(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo) const {
	UInt nextTrue, nextFalse;
	GetNextSegments(segmentInfo.index, nextTrue, nextFalse);
	AddLabelIfNeeded(compiled, info, segmentInfo);

	LoopSegment segment = segments[segmentInfo.index];
	bool isLast = IsSegmentLast(segmentInfo.index);

	LoopInfo loopInfo;
	CompileForStart(compiled, info, segmentInfo, loopInfo);
	CompileLoopBody(compiled, info, segmentInfo, loopInfo);
	CompileForEnd(compiled, info, segmentInfo, loopInfo);
}

void LoopNode::CompileForStart(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo, LoopInfo& loopInfo) const {
	bool isLast = IsSegmentLast(segmentInfo.index);

	Pointer<ForConditionNode> cond = segments[segmentInfo.index].condition.Cast<ForConditionNode>();
	compiled.AddInstructions(cond->loopInit->Compile(info).instructions);

	Instruction jmp = Instruction(InstructionType::Jmp);
	jmp.arguments.Add(Argument(ArgumentType::Label, info.label + 1));
	compiled.instructions.Add(jmp);

	Instruction lbl1 = Instruction::Label(info.label);
	loopInfo.loopLbl = info.label++;
	compiled.instructions.Add(lbl1);

	if (cond->loopStep.Cast<AssignNode>() != nullptr) {
		compiled.AddInstructions(cond->loopStep->Compile(info).instructions);
	}
	else {
		bool createAssign = true;

		if (Pointer<CallNode> call = cond->loopStep.Cast<CallNode>()) {
			createAssign = !call->GetFunc().ret.IsEmpty();
		}

		if (createAssign) {
			Pointer<AssignNode> assign = new AssignNode(cond->loopStep->scope, cond->loopStep->file);
			assign->vars.Add(cond->loopInit.Cast<AssignNode>()->vars[0]);
			
			Pointer<BinaryOperatorNode> add = new BinaryOperatorNode(cond->loopStep->scope, Scope::Add, cond->loopStep->file);
			add->node1 = assign->vars[0];
			add->node2 = cond->loopStep;

			assign->values.Add(add);
			compiled.AddInstructions(assign->Compile(info).instructions);
		}
		else {
			compiled.AddInstructions(cond->loopStep->Compile(info).instructions);
		}
	}

	Instruction lbl2 = Instruction::Label(info.label++);
	compiled.instructions.Add(lbl2);

	CompiledNode compiledCond;

	if (cond->loopCondition.Cast<AssignNode>() != nullptr) {
		compiledCond = cond->loopCondition->Compile(info);
	}
	else {
		if (cond->loopCondition->Type() != ScopeList().Add(Scope::Bool)) {
			Pointer<BinaryOperatorNode> comp = new BinaryOperatorNode(cond->loopCondition->scope, Scope::Less, cond->loopCondition->file);
			comp->node1 = cond->loopInit.Cast<AssignNode>()->vars[0];
			comp->node2 = cond->loopCondition;
			compiledCond = comp->Compile(info);
		}
		else {
			compiledCond = cond->loopCondition->Compile(info);
		}
	}

	compiled.AddInstructions(compiledCond.instructions);

	Instruction eq = Instruction(InstructionType::Eq, 1);
	eq.arguments.Add(compiledCond.argument);
	eq.arguments.Add(Argument(0));

	if (!isLast)
		loopInfo.loopEndJmp = compiled.instructions.Size();
	else
		segmentInfo.jumps.Add(Tuple<UInt, UInt>(compiled.instructions.Size(), segments.Size()));

	compiled.instructions.Add(eq);
}

void LoopNode::CompileForEnd(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo, LoopInfo& loopInfo) const {
	UInt nextTrue, nextFalse;
	GetNextSegments(segmentInfo.index, nextTrue, nextFalse);

	Instruction jmp = Instruction(InstructionType::Jmp);
	jmp.arguments.Add(Argument(ArgumentType::Label, loopInfo.loopLbl));
	compiled.instructions.Add(jmp);

	info.stack.Pop(1);

	if (!IsSegmentLast(segmentInfo.index)) {
		Instruction lbl = Instruction::Label(info.label);
		loopInfo.loopEndLbl = info.label++;
		compiled.instructions[loopInfo.loopEndJmp].instruction.arguments.Add(Argument(ArgumentType::Label, loopInfo.loopEndLbl));
		compiled.instructions.Add(lbl);

		if (nextTrue != segmentInfo.index + 1) {
			Instruction jmp = Instruction(InstructionType::Jmp);
			segmentInfo.jumps.Add(Tuple<UInt, UInt>(compiled.instructions.Size(), nextTrue));
			compiled.instructions.Add(jmp);
		}
	}
}

void LoopNode::CompileLoopBody(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo, LoopInfo& loopInfo) const {
	bool isLast = IsSegmentLast(segmentInfo.index);

	UInt nextTrue, nextFalse;
	GetNextSegments(segmentInfo.index, nextTrue, nextFalse);

	for (const OptimizerInstruction& in : segments[segmentInfo.index].statements->Compile(info).instructions) {
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
		else if (type == BreakNode::abortInstName) {
			Instruction jmp = Instruction(InstructionType::Jmp);
			segmentInfo.jumps.Add(Tuple<UInt, UInt>(compiled.instructions.Size(), segments.Size()));
			compiled.instructions.Add(jmp);
		}
		else if (type == BreakNode::breakTrueInstName) {
			Instruction jmp = Instruction(InstructionType::Jmp);
			segmentInfo.jumps.Add(Tuple<UInt, UInt>(compiled.instructions.Size(), isLast ? segments.Size() : nextTrue));
			compiled.instructions.Add(jmp);
		}
		else if (type == BreakNode::breakFalseInstName || type == BreakNode::scopeBreakInstName) {
			Instruction jmp = Instruction(InstructionType::Jmp);
			segmentInfo.jumps.Add(Tuple<UInt, UInt>(compiled.instructions.Size(), isLast ? segments.Size() : nextFalse));
			compiled.instructions.Add(jmp);
		}
		else if (type == ContinueNode::continueInstName) {
			Instruction jmp = Instruction(InstructionType::Jmp);
			jmp.arguments.Add(Argument(ArgumentType::Label, loopInfo.loopLbl));
			compiled.instructions.Add(jmp);
		}
	}
}

void LoopNode::CompileNoneSegment(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo) const {
	UInt nextTrue, nextFalse;
	GetNextSegments(segmentInfo.index, nextTrue, nextFalse);
	AddLabelIfNeeded(compiled, info, segmentInfo);

	LoopSegment segment = segments[segmentInfo.index];

	for (const OptimizerInstruction& in : segments[segmentInfo.index].statements->Compile(info).instructions) {
		if (in.instruction.type != InstructionType::Custom) {
			compiled.instructions.Add(in);
			continue;
		}

		const String type = in.instruction.instructionName;

		if (type != BreakNode::scopeBreakInstName) {
			compiled.instructions.Add(in);
			continue;
		}

		if (in.instruction.sizes[0] > 1) {
			OptimizerInstruction inst = in;
			inst.instruction.sizes[0]--;
			compiled.instructions.Add(inst);
		}
		else {
			Instruction jmp = Instruction(InstructionType::Jmp);
			segmentInfo.jumps.Add(Tuple<UInt, UInt>(compiled.instructions.Size(), IsSegmentLast(segmentInfo.index) ? segments.Size() : nextTrue));
			compiled.instructions.Add(jmp);
		}
	}

	if (!IsSegmentLast(segmentInfo.index)) {
		if (nextTrue != segmentInfo.index + 1 && (segment.type == LoopType::If || segment.type == LoopType::None)) {
			Instruction jmp = Instruction(InstructionType::Jmp);
			segmentInfo.jumps.Add(Tuple<UInt, UInt>(compiled.instructions.Size(), nextTrue));
			compiled.instructions.Add(jmp);
		}
	}
}

void LoopNode::IncludeScan(ParsingInfo& info) {
	if (includeScanned) return;

	for (LoopSegment& segment : segments) {
		if (segment.type != LoopType::None) {
			segment.condition->IncludeScan(info);
		}

		segment.statements->IncludeScan(info);
	}

	includeScanned = true;
}

bool LoopNode::WillASegmentRun() const {
	bool hasPlainAlso = false;
	bool hasPlainElse = false;

	for (UInt i = 0; i < segments.Size(); i++) {
		if (segments[i].type == LoopType::None) {
			if (segments[i].also) {
				hasPlainAlso = true;
			}
			else {
				hasPlainElse = true;
			}
		}
		else if (i > 0 && (segments[i].type == LoopType::While || segments[i].type == LoopType::For)) {
			return false;
		}
	}

	return hasPlainElse;
}

LoopNode::LoopScanInfo LoopNode::ScanSetup(ScanInfo& info) const {
	LoopScanInfo loopInfo;
	loopInfo.init = info.init;

	loopInfo.willASegmentRun = WillASegmentRun();

	loopInfo.hasReturned   = segments.Size() > 1 && loopInfo.willASegmentRun;
	loopInfo.willNotReturn = info.scopeInfo.willNotReturn;

	loopInfo.loopBreakCount  = info.scopeInfo.loopBreakCount;
	loopInfo.scopeBreakCount = info.scopeInfo.scopeBreakCount;

	if (loopInfo.init) {
		loopInfo.unassignedVarsStart = info.symbol.GetUnassignedVars();

		if (!loopInfo.willASegmentRun) {
			for (const Scope& var : loopInfo.unassignedVarsStart) {
				loopInfo.unassignedVars.Add(var);
			}
		}
	}

	return loopInfo;
}

void LoopNode::ScanFirstPostContents(LoopScanInfo& loopInfo, ScanInfo& info) const {
	bool hasPlainAlso = false;
	bool hasAlso = false;

	for (UInt i = 1; i < segments.Size(); i++) {
		if (segments[i].also) {
			hasAlso = true;

			if (segments[i].type == LoopType::None) {
				hasPlainAlso = true;
				break;
			}
		}
	}

	loopInfo.loopBreakCounts.Add(loopInfo.loopBreakCount);
	loopInfo.scopeBreakCounts.Add(loopInfo.scopeBreakCount);

	if (segments[0].type == LoopType::For || segments[0].type == LoopType::While) {
		if (loopInfo.willASegmentRun) {
			if (loopInfo.init) {
				for (const Scope& var : loopInfo.unassignedVarsStart) {
					loopInfo.unassignedVarsAlso.Add(var);
				}
			
				if (!hasPlainAlso || info.scopeInfo.WillNotContinue()) {
					for (const Scope& var : info.symbol.GetUnassignedVars()) {
						loopInfo.unassignedVars.Add(var);
					}
				}
				else {
					loopInfo.checkAlsoAssign = true;
				}
			}

			info.scopeInfo.hasReturned = hasPlainAlso;

			if (info.scopeInfo.CanAbort()) {
				loopInfo.willASegmentRun = false;
				info.scopeInfo.hasReturned = false;
			}
		}
	}
	else {
		if (loopInfo.willASegmentRun) {
			if (loopInfo.init) {
				for (const Scope& var : info.symbol.GetUnassignedVars()) {
					loopInfo.unassignedVarsAlso.Add(var);

					if (!hasPlainAlso || info.scopeInfo.WillNotContinue()) {
						loopInfo.unassignedVars.Add(var);
					}
					else {
						loopInfo.checkAlsoAssign = true;
					}
				}
			}

			if (info.scopeInfo.hasReturned || !hasAlso || hasPlainAlso) {
				if (info.scopeInfo.hasReturned) {
					loopInfo.checkAlsoRet = false;
				}
				else {
					loopInfo.checkAlsoRet = true;
					info.scopeInfo.hasReturned = hasPlainAlso;
				}
			}

			loopInfo.loopBreakCounts[0]  = info.scopeInfo.loopBreakCount;
			loopInfo.scopeBreakCounts[0] = info.scopeInfo.scopeBreakCount;
		}
	}

	if (info.scopeInfo.hasReturned && !loopInfo.unassignedVars.IsEmpty()) {
		for (const Scope& var : loopInfo.unassignedVars) {
			ErrorLog::Error(CompileError(CompileError::VarNotInitStart + var.ToString() + CompileError::VarNotInitEnd, info.file));
		}
	}

	if (!info.scopeInfo.hasReturned && hasAlso && !hasPlainAlso) {
		loopInfo.willASegmentRun = false;
	}

	if (!info.scopeInfo.WillBreak() && hasAlso && !hasPlainAlso) {
		loopInfo.willASegmentRun = false;
	}

	if (!loopInfo.willASegmentRun) {
		loopInfo.loopBreakCounts[0]  = loopInfo.loopBreakCount;
		loopInfo.scopeBreakCounts[0] = loopInfo.scopeBreakCount;
	}

	if (!info.scopeInfo.hasReturned) {
		loopInfo.hasReturned = false;
	}
}

void LoopNode::ScanPreContents(LoopScanInfo& loopInfo, ScanInfo& info, const LoopSegment& segment) const {
	if (loopInfo.init && loopInfo.willASegmentRun && (!segment.also || loopInfo.checkAlsoAssign)) {
		info.init = true;

		for (const Scope& var : loopInfo.unassignedVarsStart) {
			info.symbol.Get(var, FileInfo()).sign = false;
		}
	}

	info.scopeInfo.hasReturned     = false;
	info.scopeInfo.willNotReturn   = loopInfo.willNotReturn;
	info.scopeInfo.loopBreakCount  = loopInfo.loopBreakCount;
	info.scopeInfo.scopeBreakCount = loopInfo.scopeBreakCount;
}

void LoopNode::ScanPostContents(LoopScanInfo& loopInfo, ScanInfo& info, const LoopSegment& segment) const {
	if (loopInfo.init && loopInfo.willASegmentRun && (!segment.also || loopInfo.checkAlsoAssign)) {
		if (segment.also) {
			for (const Scope& var : info.symbol.GetUnassignedVars()) {
				if (loopInfo.unassignedVarsAlso.Contains(var)) {
					loopInfo.unassignedVars.Add(var);
				}
			}
		}
		else {
			for (const Scope& var : info.symbol.GetUnassignedVars()) {
				loopInfo.unassignedVars.Add(var);
			}
		}
	}

	if (!info.scopeInfo.willNotReturn) {
		loopInfo.hasAReturn = true;
	}

	if (loopInfo.willASegmentRun && (!segment.also || loopInfo.checkAlsoRet)) {
		if (!info.scopeInfo.hasReturned) {
			loopInfo.hasReturned = false;
		}
	}

	if (loopInfo.willASegmentRun) {
		loopInfo.loopBreakCounts.Add(info.scopeInfo.loopBreakCount);
		loopInfo.scopeBreakCounts.Add(info.scopeInfo.scopeBreakCount);
	}
	else {
		loopInfo.loopBreakCounts.Add(loopInfo.loopBreakCount);
		loopInfo.scopeBreakCounts.Add(loopInfo.scopeBreakCount);
	}
}

void LoopNode::ScanCleanup(LoopScanInfo& loopInfo, ScanInfo& info) const {
	if (loopInfo.init) {
		for (const Scope& var : loopInfo.unassignedVarsStart) {
			info.symbol.Get(var, FileInfo()).sign = loopInfo.willASegmentRun;
		}

		for (const Scope& var : loopInfo.unassignedVars) {
			info.symbol.Get(var, FileInfo()).sign = false;
		}

		if (!loopInfo.unassignedVars.IsEmpty()) {
			info.init = true;
		}
	}

	UInt mainLoopBreaks  = loopInfo.loopBreakCounts[0];
	UInt mainScopeBreaks = loopInfo.scopeBreakCounts[0];
	UInt alsoLoopBreaks  = segments.Size() > 1 ? Math::UIntMax() : 0;
	UInt alsoScopeBreaks = segments.Size() > 1 ? Math::UIntMax() : 0;
	UInt elseLoopBreaks  = segments.Size() > 1 ? Math::UIntMax() : 0;
	UInt elseScopeBreaks = segments.Size() > 1 ? Math::UIntMax() : 0;

	for (UInt i = 1; i < segments.Size(); i++) {
		if (segments[i].also) {
			if (loopInfo.loopBreakCounts[i] < alsoLoopBreaks) {
				alsoLoopBreaks = loopInfo.loopBreakCounts[i];
			}

			if (loopInfo.scopeBreakCounts[i] < alsoScopeBreaks) {
				alsoScopeBreaks = loopInfo.scopeBreakCounts[i];
			}
		}
		else {
			if (loopInfo.loopBreakCounts[i] < elseLoopBreaks) {
				elseLoopBreaks = loopInfo.loopBreakCounts[i];
			}

			if (loopInfo.scopeBreakCounts[i] < elseScopeBreaks) {
				elseScopeBreaks = loopInfo.scopeBreakCounts[i];
			}
		}
	}

	info.scopeInfo.loopBreakCount  = Math::Min(Math::Max( mainLoopBreaks,  alsoLoopBreaks),  elseLoopBreaks);
	info.scopeInfo.scopeBreakCount = Math::Min(Math::Max(mainScopeBreaks, alsoScopeBreaks), elseScopeBreaks);

	info.scopeInfo.hasReturned = loopInfo.hasReturned || !loopInfo.willNotReturn;
	info.scopeInfo.willNotReturn = loopInfo.willNotReturn && !info.scopeInfo.hasReturned && !loopInfo.hasAReturn;
}

Set<ScanType> LoopNode::Scan(ScanInfoStack& info) {
	Set<ScanType> scanSet = Set<ScanType>();

	info.Get().scopeInfo.EnterScope();
	LoopScanInfo loopInfo = ScanSetup(info.Get());
	info.Get().scopeInfo.ExitScope();

	for (UInt i = 0; i < segments.Size(); i++) {
		if (segments[i].type != LoopType::None) {
			for (const ScanType type : segments[i].condition->Scan(info)) {
				scanSet.Add(type);

				if (info.Get().init && type == ScanType::Self && !info.Get().symbol.IsAssigned()) {
					ErrorLog::Error(CompileError(CompileError::SelfInit, segments[i].condition->file));
				}
			}
		}

		info.Get().scopeInfo.EnterScope(segments[i].type == LoopType::For || segments[i].type == LoopType::While);

		ScanPreContents(loopInfo, info.Get(), segments[i]);

		for (const ScanType type : segments[i].statements->Scan(info)) {
			scanSet.Add(type);

			if (info.Get().init && type == ScanType::Self && !info.Get().symbol.IsAssigned()) {
				ErrorLog::Error(CompileError(CompileError::SelfInit, segments[i].statements->file));
			}
		}

		if (segments[i].type == LoopType::For || segments[i].type == LoopType::While) {
			info.Get().scopeInfo.loopBreakCount  = loopInfo.loopBreakCount;
			info.Get().scopeInfo.scopeBreakCount = loopInfo.scopeBreakCount;
		}

		if (i == 0) {
			ScanFirstPostContents(loopInfo, info.Get());
		}
		else {
			ScanPostContents(loopInfo, info.Get(), segments[i]);
		}

		info.Get().scopeInfo.ExitScope(segments[i].type == LoopType::For || segments[i].type == LoopType::While);
	}

	info.Get().scopeInfo.EnterScope();
	ScanCleanup(loopInfo, info.Get());
	info.Get().scopeInfo.ExitScope();

	return scanSet;
}

Mango LoopNode::ToMango() const {
	Mango mango = Mango("loop", MangoType::List);

	for (UInt i = 0; i < segments.Size(); i++) {
		String name = "";

		if (segments[i].type == LoopType::If)
			name = "if";
		else if (segments[i].type == LoopType::While)
			name = "while";
		else if (segments[i].type == LoopType::For)
			name = "for";

		if (i > 0) {
			if (segments[i].also)
				name = "also" + name;
			else
				name = "else" + name;
		}

		Mango s = Mango(name, MangoType::Map);

		if (segments[i].type == LoopType::If || segments[i].type == LoopType::While || segments[i].type == LoopType::For) {
			s.Add("condition", segments[i].condition->ToMango());
		}

		s.Add("content", segments[i].statements->ToMango());

		mango.Add(s);
	}

	return mango;
}