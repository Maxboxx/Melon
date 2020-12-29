#include "LoopNode.h"

#include "Boxx/Math.h"

#include "ForConditionNode.h"
#include "ConditionNode.h"
#include "BreakNode.h"
#include "ContinueNode.h"
#include "AssignNode.h"
#include "CallNode.h"
#include "BinaryOperatorNode.h"
#include "EmptyNode.h"
#include "DoNode.h"

#include "Melon/Parsing/Parser.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

LoopNode::LoopNode(Symbol* const scope, const FileInfo& file) : Node(scope, file) {

}

LoopNode::~LoopNode() {

}

UInt LoopNode::GetSize() const {
	UInt size = 0;
	
	for (UInt i = 0; i < segments.Size(); i++) {
		UInt segSize = segments[i].statements->GetSize();

		if (segments[i].type != LoopType::None) {
			segSize += segments[i].condition->GetSize();

			if (segments[i].type == LoopType::While) {
				if (!IsSegmentLast(i)) {
					segSize++;
				}
			}
		}

		if (size < segSize) {
			size = segSize;
		}
	}

	return size;
}

bool LoopNode::IsScope() const {
	return true;
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
	if (segment == segments.Size() - 1) return true;
	return segment != 0 && segments[segment].also != segments[segment + 1].also;
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

	const UInt frame = info.stack.frame;
	CompiledNode cond = segment.condition->Compile(info);
	info.stack.PopExpr(frame, cond);
	
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

	const UInt frame = info.stack.frame;
	CompiledNode cond = segments[segmentInfo.index].condition->Compile(info);
	info.stack.PopExpr(frame, cond);
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

	if (!IsSegmentLast(segmentInfo.index)) {
		info.stack.Pop(1);

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

	const UInt frame = info.stack.frame;

	Pointer<ForConditionNode> cond = segments[segmentInfo.index].condition.Cast<ForConditionNode>();
	compiled.AddInstructions(cond->loopInit->Compile(info).instructions);

	Instruction jmp = Instruction(InstructionType::Jmp);
	jmp.arguments.Add(Argument(ArgumentType::Label, (Long)info.label + 1));
	compiled.instructions.Add(jmp);

	Instruction lbl1 = Instruction::Label(info.label);
	loopInfo.loopLbl = info.label++;
	compiled.instructions.Add(lbl1);

	if (cond->stepOperator) {
		Pointer<AssignNode> assign = new AssignNode(cond->loopStep->scope, cond->loopStep->file);
		assign->vars.Add(cond->loopInit.Cast<AssignNode>()->vars[0]);

		Pointer<BinaryOperatorNode> add = new BinaryOperatorNode(cond->loopStep->scope, cond->stepOperator.Get(), cond->loopStep->file);
		add->node1 = assign->vars[0];
		add->node2 = cond->loopStep;

		assign->values.Add(add);
		compiled.AddInstructions(assign->Compile(info).instructions);
	}
	else {
		compiled.AddInstructions(cond->loopStep->Compile(info).instructions);
	}

	Instruction lbl2 = Instruction::Label(info.label++);
	compiled.instructions.Add(lbl2);

	CompiledNode compiledCond;

	if (cond->conditionOperator) {
		Pointer<BinaryOperatorNode> comp = new BinaryOperatorNode(cond->loopCondition->scope, Scope::Less, cond->loopCondition->file);
		comp->node1 = cond->loopInit.Cast<AssignNode>()->vars[0];
		comp->node2 = cond->loopCondition;
		compiledCond = comp->Compile(info);
	}
	else {
		compiledCond = cond->loopCondition->Compile(info);
	}

	info.stack.PopExpr(frame, compiledCond);
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
	for (LoopSegment& segment : segments) {
		if (segment.type != LoopType::None) {
			segment.condition->IncludeScan(info);
		}

		segment.statements->IncludeScan(info);
	}
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

	loopInfo.scope = info.scopeInfo.CopyBranch();

	/* TODO: node
	if (loopInfo.init) {
		loopInfo.scope.unassigned = info.symbol.GetUnassignedVarsSet();
	}
	*/

	return loopInfo;
}

void LoopNode::ScanPreContents(LoopScanInfo& loopInfo, ScanInfo& info, const LoopSegment& segment) const {
	/* TODO: node
	if (loopInfo.init) {
		if (!segment.also) {
			for (const Scope& var : loopInfo.scope.unassigned) {
				info.symbol.Get(var, FileInfo()).isAssigned = false;
			}
		}
		else {
			for (const Scope& var : loopInfo.mainSegment.unassigned) {
				info.symbol.Get(var, FileInfo()).isAssigned = false;
			}
		}
	}
	*/

	if (!segment.also) {
		info.scopeInfo = loopInfo.scope.CopyBranch();
	}
	else {
		info.scopeInfo = loopInfo.mainSegment.CopyBranch();
	}
}

void LoopNode::ScanFirstPostContents(LoopScanInfo& loopInfo, ScanInfo& info) const {
	/* TODO: node
	if (info.init) {
		info.scopeInfo.unassigned = info.symbol.GetUnassignedVarsSet();
	}
	*/

	if (segments[0].IsLoop()) {
		loopInfo.mainSegment = ScopeInfo::WeakBranchUnion(loopInfo.scope, info.scopeInfo);

		if (info.scopeInfo.CanAbort()) {
			loopInfo.willASegmentRun = false;
		}
	}
	else {
		loopInfo.mainSegment = info.scopeInfo;
	}
}

void LoopNode::ScanPostContents(LoopScanInfo& loopInfo, ScanInfo& info, const LoopSegment& segment) const {
	/* TODO: node
	if (info.init) {
		info.scopeInfo.unassigned = info.symbol.GetUnassignedVarsSet();
	}
	*/

	if (segment.also) {
		loopInfo.alsoSegments.Add(info.scopeInfo);
	}
	else {
		loopInfo.elseSegments.Add(info.scopeInfo);
	}
}

void LoopNode::ScanCleanup(LoopScanInfo& loopInfo, ScanInfo& info) const {
	if (!loopInfo.alsoSegments.IsEmpty()) {
		if (loopInfo.willASegmentRun) { 
			loopInfo.scope = loopInfo.alsoSegments[0];
		}
		else {
			loopInfo.scope = ScopeInfo::WeakBranchUnion(loopInfo.scope, loopInfo.alsoSegments[0]);
		}

		for (UInt i = 1; i < loopInfo.alsoSegments.Size(); i++) {
			if (loopInfo.willASegmentRun) {
				loopInfo.scope = ScopeInfo::BranchIntersection(loopInfo.scope, loopInfo.alsoSegments[i]);
			}
			else {
				loopInfo.scope = ScopeInfo::WeakBranchIntersection(loopInfo.scope, loopInfo.alsoSegments[i]);
			}
		}
	}
	else {
		loopInfo.scope = loopInfo.mainSegment;
	}

	for (UInt i = 0; i < loopInfo.elseSegments.Size(); i++) {
		if (loopInfo.willASegmentRun) {
			loopInfo.scope = ScopeInfo::BranchIntersection(loopInfo.scope, loopInfo.elseSegments[i]);
		}
		else {
			loopInfo.scope = ScopeInfo::WeakBranchIntersection(loopInfo.scope, loopInfo.elseSegments[i]);
		}
	}

	info.scopeInfo = loopInfo.scope;
}

Set<ScanType> LoopNode::Scan(ScanInfoStack& info) {
	Set<ScanType> scanSet = Set<ScanType>();

	info.Get().scopeInfo.EnterScope(ScopeInfo::ScopeType::Scope);
	LoopScanInfo loopInfo = ScanSetup(info.Get());
	info.Get().scopeInfo.ExitScope();

	for (UInt i = 0; i < segments.Size(); i++) {
		info.Get().scopeInfo.EnterScope(segments[i].IsLoop() ? ScopeInfo::ScopeType::Loop : ScopeInfo::ScopeType::Scope);
		ScanPreContents(loopInfo, info.Get(), segments[i]);

		/* TODO: node
		if (segments[i].type != LoopType::None) {
			for (const ScanType type : segments[i].condition->Scan(info)) {
				scanSet.Add(type);

				if (info.Get().init && type == ScanType::Self && !info.Get().symbol.IsAssigned()) {
					ErrorLog::Error(CompileError(CompileError::SelfInit, segments[i].condition->file));
				}
			}
		}

		for (const ScanType type : segments[i].statements->Scan(info)) {
			scanSet.Add(type);

			if (info.Get().init && type == ScanType::Self && !info.Get().symbol.IsAssigned()) {
				ErrorLog::Error(CompileError(CompileError::SelfInit, segments[i].statements->file));
			}
		}

		if (segments[i].IsLoop()) {
			info.Get().scopeInfo.loopBreakCount  = loopInfo.scope.loopBreakCount;
			info.Get().scopeInfo.scopeBreakCount = loopInfo.scope.scopeBreakCount;
		}

		if (i == 0) {
			ScanFirstPostContents(loopInfo, info.Get());
		}
		else {
			ScanPostContents(loopInfo, info.Get(), segments[i]);
		}

		info.Get().scopeInfo.ExitScope();
		*/
	}

	info.Get().scopeInfo.EnterScope(ScopeInfo::ScopeType::Scope);
	ScanCleanup(loopInfo, info.Get());
	info.Get().scopeInfo.ExitScope();

	return scanSet;
}

ScopeList LoopNode::FindSideEffectScope(const bool assign) {
	ScopeList list = segments[0].statements->GetSideEffectScope(assign);

	for (LoopSegment& segment : segments) {
		if (segment.IsLoop()) {
			return ScopeList(true).Add(Scope::Global);
		}

		if (segment.condition) {
			list = CombineSideEffects(list, segment.condition->GetSideEffectScope(assign));
		}

		list = CombineSideEffects(list, segment.statements->GetSideEffectScope(assign));
	}

	return list;
}

NodePtr LoopNode::Optimize(OptimizeInfo& info) {
	for (LoopSegment& segment : segments) {
		if (segment.condition) {
			if (NodePtr node = segment.condition->Optimize(info)) segment.condition = node;
		}

		if (NodePtr node = segment.statements->Optimize(info)) segment.statements = node;
	}

	// Removes also or else path
	if ((segments[0].type == LoopType::If || segments[0].type == LoopType::While) && segments[0].condition->IsImmediate()) {
		if (segments[0].condition->GetImmediate() == 0) {
			for (UInt i = 1; i < segments.Size(); i++) {
				if (!segments[i].also) {
					segments.RemoveAt(0, i);
					info.optimized = true;
					break;
				}
			}
		}
		else if (segments[0].type == LoopType::If) {
			info.optimized = true;

			for (UInt i = 1; i < segments.Size(); i++) {
				if (!segments[i].also) {
					segments.RemoveAt(i, segments.Size() - i);
					break;
				}
			}

			for (UInt i = 1; i < segments.Size(); i++) {
				segments[i].also = false;
			}
		}
	}

	// TODO: check for breaks

	// Remove unnecessary segments
	for (UInt i = 0; i < segments.Size(); i++) {
		if ((segments[i].type == LoopType::If || segments[i].type == LoopType::While) && segments[i].condition->IsImmediate()) {
			if (segments[i].condition->GetImmediate() == 0) {
				segments.RemoveAt(i);
				i--;
				info.optimized = true;
			}
			else if (segments[i].type == LoopType::If) {
				info.optimized = true;
				segments[i].type = LoopType::None;

				if (!segments[i].also) {
					segments.RemoveAt(i + 1, segments.Size() - i - 1);
					break;
				}
				else {
					UInt u = i + 1;

					for (; u < segments.Size(); u++) {
						if (!segments[u].also) {
							break;
						}
					}

					segments.RemoveAt(i + 1, u - i - 1);
				}
			}
		}
	}

	// Remove empty segments
	for (UInt i = segments.Size(); i > 0;) {
		i--;

		if (!segments[i].IsLoop() && IsSegmentLast(i) && IsEmpty(segments[i].statements)) {
			info.optimized = true;
			segments.RemoveAt(i);
		}
	}

	if (segments.Size() == 0) {
		info.optimized = true;
		return new EmptyNode();
	}
	else if (segments.Size() == 1 && (!segments[0].condition || segments[0].condition->IsImmediate())) {
		if (!segments[0].IsLoop()) {
			Pointer<DoNode> dn = new DoNode(segments[0].statements->scope, segments[0].statements->file);
			dn->nodes = segments[0].statements;
			info.optimized = true;
			return dn;
		}
	}

	return nullptr;
}

StringBuilder LoopNode::ToMelon(const UInt indent) const {
	StringBuilder sb;
	String tabs1 = String('\t').Repeat(indent);
	String tabs2 = String('\t').Repeat(indent + 1);

	for (UInt i = 0; i < segments.Size(); i++) {
		if (i > 0) {
			sb += segments[i].also ? "also" : "else";
		}

		switch (segments[i].type) {
			case LoopType::If:    sb += "if ";    break;
			case LoopType::While: sb += "while "; break;
			case LoopType::For:   sb += "for ";   break;
		}

		if (segments[i].type != LoopType::None) {
			sb += segments[i].condition->ToMelon(indent);
			
			if (segments[i].type == LoopType::If) {
				sb += " then";
			}
			else {
				sb += " do";
			}
		}

		sb += "\n";
		sb += tabs2;
		sb += segments[i].statements->ToMelon(indent + 1);
		sb += "\n";
		sb += tabs1;
	}

	sb += "end";

	return sb;
}