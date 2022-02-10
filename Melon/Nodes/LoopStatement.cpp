#include "LoopStatement.h"

#include "Boxx/Math.h"

#include "Condition.h"
#include "BreakStatement.h"
#include "ContinueStatement.h"
#include "Assignment.h"
#include "CallStatement.h"
#include "BinaryOperatorExpression.h"
#include "EmptyStatement.h"
#include "DoStatement.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/VariableSymbol.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

LoopStatement::LoopStatement(Symbol* const scope, const FileInfo& file) : Statement(scope, file) {

}

LoopStatement::~LoopStatement() {

}

UInt LoopStatement::GetSize() const {
	UInt size = 0;
	
	for (UInt i = 0; i < segments.Size(); i++) {
		const LoopSegment& segment = segments[i];
		UInt segSize = segment.statements->GetSize();

		if (segment.type != LoopType::None) {
			segSize += segment.condition->GetSize();

			switch (segment.type) {
				case LoopType::While: {
					if (!IsSegmentLast(i)) {
						segSize++;
					}

					break;
				}

				case LoopType::For: {
					segSize += segment.init->GetSize() + segment.step->GetSize();
				}
			}
		}

		if (size < segSize) {
			size = segSize;
		}
	}

	return size;
}

bool LoopStatement::IsScope() const {
	return true;
}

CompiledNode LoopStatement::Compile(CompileInfo& info) {
	CompiledNode c;

	SegmentInfo segmentInfo;
	segmentInfo.segmentLabels = Array<UInt>(segments.Size() + 1);
	segmentInfo.stackTop = info.stack.top;

	// Compile segments
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

	// Create end label
	Instruction endLbl = Instruction::Label(info.label);
	segmentInfo.segmentLabels[segmentInfo.segmentLabels.Size() - 1] = info.label++;
	c.instructions.Add(endLbl);

	// Add jumps
	for (const Tuple<UInt, UInt>& jump : segmentInfo.jumps) {
		c.instructions[jump.value1].instruction.arguments.Add(Argument(ArgumentType::Label, segmentInfo.segmentLabels[jump.value2]));
	}

	return c;
}

void LoopStatement::GetNextSegments(const UInt segment, UInt& nextTrue, UInt& nextFalse) const {
	nextTrue = segments.Size();
	nextFalse = segment + 1;

	// Check if the current segment is the last segment
	if (IsSegmentLast(segment)) {
		// Set next false to end for also mismatch
		if (segments[segment].also != segments[nextFalse].also) {
			nextFalse = segments.Size();
		}

		// Get next for main segment
		if (segment == 0) {
			nextTrue = segments.Size();

			// Set next true to first also
			for (UInt u = 1; u < segments.Size(); u++) {
				if (segments[u].also) {
					nextTrue = u;
					break;
				}
			}

			nextFalse = segments.Size();

			// Set next false to first else
			for (UInt u = 1; u < segments.Size(); u++) {
				if (!segments[u].also) {
					nextFalse = u;
					break;
				}
			}
		}
	}
}

bool LoopStatement::IsSegmentLast(const UInt segment) const {
	// Check if the segmet is last
	if (segment == segments.Size() - 1) return true;

	// Check if the segent is the last also
	return segment != 0 && segments[segment].also != segments[segment + 1].also;
}

void LoopStatement::AddLabelIfNeeded(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo) const {
	bool needsLabel = false;

	// Check if a label is needed
	for (const Tuple<UInt, UInt>& next : segmentInfo.jumps) {
		if (next.value2 == segmentInfo.index) {
			needsLabel = true;
			break;
		}
	}

	// Add a label
	if (needsLabel) {
		Instruction lbl = Instruction::Label(info.label);
		segmentInfo.segmentLabels[segmentInfo.index] = info.label++;
		compiled.instructions.Add(lbl);
	}
}

void LoopStatement::CompileIfSegment(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo) const {
	// Setup segment
	UInt nextTrue, nextFalse;
	GetNextSegments(segmentInfo.index, nextTrue, nextFalse);
	AddLabelIfNeeded(compiled, info, segmentInfo);

	const LoopSegment* segment = &segments[segmentInfo.index];

	// Compile condition
	const UInt frame = info.stack.frame;
	CompiledNode cond = segment->condition->Compile(info);
	info.stack.PopExpr(frame, cond);
	
	compiled.AddInstructions(cond.instructions);

	// Chech if condition is true
	Instruction eq = Instruction(InstructionType::Eq, 1);
	eq.arguments.Add(cond.argument);
	eq.arguments.Add(Argument(0));
	segmentInfo.jumps.Add(Tuple<UInt, UInt>(compiled.instructions.Size(), nextFalse));
	compiled.instructions.Add(eq);

	// Compile statements
	for (const OptimizerInstruction& in : segments[segmentInfo.index].statements->Compile(info).instructions) {
		// Check for custom instructions
		if (in.instruction.type != InstructionType::Custom) {
			compiled.instructions.Add(in);
			continue;
		}

		const String type = in.instruction.instructionName;

		// Check for scopewise break
		if (type != BreakStatement::scopeBreakInstName) {
			compiled.instructions.Add(in);
			continue;
		}

		// Compile scopewise break
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
	
	// Compile jump to next
	if (!IsSegmentLast(segmentInfo.index)) {
		if (nextTrue != segmentInfo.index + 1 && (segment->type == LoopType::If || segment->type == LoopType::None)) {
			Instruction jmp = Instruction(InstructionType::Jmp);
			segmentInfo.jumps.Add(Tuple<UInt, UInt>(compiled.instructions.Size(), nextTrue));
			compiled.instructions.Add(jmp);
		}
	}
}

void LoopStatement::CompileWhileSegment(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo) const {
	// Setup segment
	AddLabelIfNeeded(compiled, info, segmentInfo);

	// Compile segment
	LoopInfo loopInfo;
	CompileWhileStart(compiled, info, segmentInfo, loopInfo);
	CompileLoopBody(compiled, info, segmentInfo, loopInfo);
	CompileWhileEnd(compiled, info, segmentInfo, loopInfo);
}

void LoopStatement::CompileWhileStart(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo, LoopInfo& loopInfo) const {
	bool isLast = IsSegmentLast(segmentInfo.index);

	// Add loop flag if loop is not last
	if (!isLast) {
		info.stack.Push(1);
		loopInfo.stack = info.stack.top;

		Instruction mov = Instruction(InstructionType::Mov, 1);
		mov.arguments.Add(Argument(MemoryLocation(info.stack.Offset(loopInfo.stack))));
		mov.arguments.Add(Argument(0));
		compiled.instructions.Add(mov);
	}

	// Add loop label
	Instruction lbl = Instruction::Label(info.label);
	loopInfo.loopLbl = info.label++;
	compiled.instructions.Add(lbl);

	// Compile condition
	const UInt frame = info.stack.frame;
	CompiledNode cond = segments[segmentInfo.index].condition->Compile(info);
	info.stack.PopExpr(frame, cond);
	compiled.AddInstructions(cond.instructions);

	// Check if condition is true
	Instruction eq = Instruction(InstructionType::Eq, 1);
	eq.arguments.Add(cond.argument);
	eq.arguments.Add(Argument(0));

	// Set jump info
	if (!isLast) {
		loopInfo.loopEndJmp = compiled.instructions.Size();
	}
	else {
		segmentInfo.jumps.Add(Tuple<UInt, UInt>(compiled.instructions.Size(), segments.Size()));
	}

	compiled.instructions.Add(eq);

	// Set loop flag to true
	if (!isLast) {
		Instruction mov = Instruction(InstructionType::Mov, 1);
		mov.arguments.Add(Argument(MemoryLocation(info.stack.Offset(loopInfo.stack))));
		mov.arguments.Add(Argument(1));
		compiled.instructions.Add(mov);
	}
}

void LoopStatement::CompileWhileEnd(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo, LoopInfo& loopInfo) const {
	UInt nextTrue, nextFalse;
	GetNextSegments(segmentInfo.index, nextTrue, nextFalse);

	// Add loop jump
	Instruction jmp = Instruction(InstructionType::Jmp);
	jmp.arguments.Add(Argument(ArgumentType::Label, loopInfo.loopLbl));
	compiled.instructions.Add(jmp);

	// Add jump to next segment
	if (!IsSegmentLast(segmentInfo.index)) {
		info.stack.Pop(1);

		// Add end label
		Instruction lbl = Instruction::Label(info.label);
		loopInfo.loopEndLbl = info.label++;
		compiled.instructions[loopInfo.loopEndJmp].instruction.arguments.Add(Argument(ArgumentType::Label, loopInfo.loopEndLbl));
		compiled.instructions.Add(lbl);

		// Add jump to next false
		if (nextTrue == segmentInfo.index + 1) {
			Instruction eq = Instruction(InstructionType::Eq, 1);
			eq.arguments.Add(Argument(MemoryLocation(info.stack.Offset(loopInfo.stack))));
			eq.arguments.Add(Argument(0));
			segmentInfo.jumps.Add(Tuple<UInt, UInt>(compiled.instructions.Size(), nextFalse));
			compiled.instructions.Add(eq);
		}
		// Add jump to next true
		else if (nextFalse == segmentInfo.index + 1) {
			Instruction ne = Instruction(InstructionType::Ne, 1);
			ne.arguments.Add(Argument(MemoryLocation(info.stack.Offset(loopInfo.stack))));
			ne.arguments.Add(Argument(0));
			segmentInfo.jumps.Add(Tuple<UInt, UInt>(compiled.instructions.Size(), nextTrue));
			compiled.instructions.Add(ne);
		}
		// Add jump to either next true or false
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

void LoopStatement::CompileForSegment(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo) const {
	// Setup segment
	AddLabelIfNeeded(compiled, info, segmentInfo);

	// Compile segment
	LoopInfo loopInfo;
	CompileForStart(compiled, info, segmentInfo, loopInfo);
	CompileLoopBody(compiled, info, segmentInfo, loopInfo);
	CompileForEnd(compiled, info, segmentInfo, loopInfo);
}

void LoopStatement::CompileForStart(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo, LoopInfo& loopInfo) const {
	const bool isLast = IsSegmentLast(segmentInfo.index);

	const UInt frame = info.stack.frame;

	const LoopSegment& segment = segments[segmentInfo.index];

	// Compile for init
	compiled.AddInstructions(segment.init->Compile(info).instructions);

	// Add jump to loop condition
	Instruction jmp = Instruction(InstructionType::Jmp);
	jmp.arguments.Add(Argument(ArgumentType::Label, (Long)info.label + 1));
	compiled.instructions.Add(jmp);

	// Add loop label
	Instruction lbl1 = Instruction::Label(info.label);
	loopInfo.loopLbl = info.label++;
	compiled.instructions.Add(lbl1);

	// Compile loop step
	if (segment.stepOperator) {
		Fixed<Assignment> assign = Assignment(segment.step->scope, segment.step->File());
		assign->assignableValues.Add(segment.init->assignableValues[0]);

		Ptr<BinaryOperatorExpression> add = new BinaryOperatorExpression(segment.step->scope, *segment.stepOperator, segment.step->File());
		add->operand1 = assign->assignableValues[0];
		add->operand2 = new WeakExpression(segment.step.As<Expression>());

		assign->values.Add(add);
		compiled.AddInstructions(assign->Compile(info).instructions);
	}
	else {
		compiled.AddInstructions(segment.step->Compile(info).instructions);
	}

	// Add condition label
	Instruction lbl2 = Instruction::Label(info.label++);
	compiled.instructions.Add(lbl2);

	CompiledNode compiledCond;

	// Compile condition
	if (segment.conditionOperator) {
		Pointer<BinaryOperatorExpression> comp = new BinaryOperatorExpression(segment.condition->scope, *segment.conditionOperator, segment.condition->File());
		comp->operand1 = segment.init->assignableValues[0];
		comp->operand2 = segment.condition->expression;
		compiledCond = comp->Compile(info);
	}
	else {
		compiledCond = segment.condition->Compile(info);
	}

	info.stack.PopExpr(frame, compiledCond);
	compiled.AddInstructions(compiledCond.instructions);

	// Check if loop ondition is true
	Instruction eq = Instruction(InstructionType::Eq, 1);
	eq.arguments.Add(compiledCond.argument);
	eq.arguments.Add(Argument(0));

	if (!isLast) {
		loopInfo.loopEndJmp = compiled.instructions.Size();
	}
	else {
		segmentInfo.jumps.Add(Tuple<UInt, UInt>(compiled.instructions.Size(), segments.Size()));
	}

	compiled.instructions.Add(eq);
}

void LoopStatement::CompileForEnd(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo, LoopInfo& loopInfo) const {
	UInt nextTrue, nextFalse;
	GetNextSegments(segmentInfo.index, nextTrue, nextFalse);

	// Add jump to loop label
	Instruction jmp = Instruction(InstructionType::Jmp);
	jmp.arguments.Add(Argument(ArgumentType::Label, loopInfo.loopLbl));
	compiled.instructions.Add(jmp);

	info.stack.Pop(1);

	// Add jump to next true
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

void LoopStatement::CompileLoopBody(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo, LoopInfo& loopInfo) const {
	bool isLast = IsSegmentLast(segmentInfo.index);

	UInt nextTrue, nextFalse;
	GetNextSegments(segmentInfo.index, nextTrue, nextFalse);

	// Compile loop body
	for (const OptimizerInstruction& in : segments[segmentInfo.index].statements->Compile(info).instructions) {
		// Check for custom instructions
		if (in.instruction.type != InstructionType::Custom) {
			compiled.instructions.Add(in);
			continue;
		}

		const String type = in.instruction.instructionName;

		// Check for break instruction
		if (
			type != BreakStatement::abortInstName &&
			type != BreakStatement::scopeBreakInstName &&
			type != BreakStatement::breakTrueInstName &&
			type != BreakStatement::breakFalseInstName &&
			type != ContinueStatement::continueInstName
		) {
			compiled.instructions.Add(in);
			continue;
		}

		// Compile multi break
		if (in.instruction.sizes[0] > 1) {
			OptimizerInstruction inst = in;
			inst.instruction.sizes[0]--;
			compiled.instructions.Add(inst);
		}
		// Compile abort
		else if (type == BreakStatement::abortInstName) {
			Instruction jmp = Instruction(InstructionType::Jmp);
			segmentInfo.jumps.Add(Tuple<UInt, UInt>(compiled.instructions.Size(), segments.Size()));
			compiled.instructions.Add(jmp);
		}
		// Compile break true
		else if (type == BreakStatement::breakTrueInstName) {
			Instruction jmp = Instruction(InstructionType::Jmp);
			segmentInfo.jumps.Add(Tuple<UInt, UInt>(compiled.instructions.Size(), isLast ? segments.Size() : nextTrue));
			compiled.instructions.Add(jmp);
		}
		// Compile break false and scopewise break
		else if (type == BreakStatement::breakFalseInstName || type == BreakStatement::scopeBreakInstName) {
			Instruction jmp = Instruction(InstructionType::Jmp);
			segmentInfo.jumps.Add(Tuple<UInt, UInt>(compiled.instructions.Size(), isLast ? segments.Size() : nextFalse));
			compiled.instructions.Add(jmp);
		}
		// Compile continue
		else if (type == ContinueStatement::continueInstName) {
			Instruction jmp = Instruction(InstructionType::Jmp);
			jmp.arguments.Add(Argument(ArgumentType::Label, loopInfo.loopLbl));
			compiled.instructions.Add(jmp);
		}
	}
}

void LoopStatement::CompileNoneSegment(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo) const {
	UInt nextTrue, nextFalse;
	GetNextSegments(segmentInfo.index, nextTrue, nextFalse);
	AddLabelIfNeeded(compiled, info, segmentInfo);

	const LoopSegment& segment = segments[segmentInfo.index];

	// Compile segment body
	for (const OptimizerInstruction& in : segments[segmentInfo.index].statements->Compile(info).instructions) {
		// Check for custom instructions
		if (in.instruction.type != InstructionType::Custom) {
			compiled.instructions.Add(in);
			continue;
		}

		const String type = in.instruction.instructionName;

		// Check for scope break instruction
		if (type != BreakStatement::scopeBreakInstName) {
			compiled.instructions.Add(in);
			continue;
		}

		// Compile scopewise break
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

	// Jump to next true
	if (!IsSegmentLast(segmentInfo.index)) {
		if (nextTrue != segmentInfo.index + 1 && (segment.type == LoopType::If || segment.type == LoopType::None)) {
			Instruction jmp = Instruction(InstructionType::Jmp);
			segmentInfo.jumps.Add(Tuple<UInt, UInt>(compiled.instructions.Size(), nextTrue));
			compiled.instructions.Add(jmp);
		}
	}
}

void LoopStatement::IncludeScan(ParsingInfo& info) {
	for (LoopSegment& segment : segments) {
		if (segment.type != LoopType::None) {
			segment.condition->IncludeScan(info);

			if (segment.type == LoopType::For) {
				segment.init->IncludeScan(info);
				segment.step->IncludeScan(info);
			}
		}

		segment.statements->IncludeScan(info);
	}
}

bool LoopStatement::WillASegmentRun() const {
	bool hasPlainAlso = false;
	bool hasPlainElse = false;

	for (UInt i = 0; i < segments.Size(); i++) {
		// Check if loop has plain also or else
		if (segments[i].type == LoopType::None) {
			if (segments[i].also) {
				hasPlainAlso = true;
			}
			else {
				hasPlainElse = true;
			}
		}
		// Check if loop has also or else loops
		else if (i > 0 && segments[i].IsLoop()) {
			return false;
		}
	}

	return hasPlainElse;
}

LoopStatement::LoopScanInfo LoopStatement::ScanSetup(ScanInfo& info) const {
	LoopScanInfo loopInfo;
	loopInfo.init = info.init;

	loopInfo.willASegmentRun = WillASegmentRun();

	loopInfo.scope = info.scopeInfo.CopyBranch();

	if (loopInfo.init) {
		loopInfo.scope.unassigned = info.type->UnassignedMembers();
	}

	return loopInfo;
}

void LoopStatement::ScanPreContents(LoopScanInfo& loopInfo, ScanInfo& info, const LoopSegment& segment) const {
	// Reset unassigned variables
	if (loopInfo.init) {
		if (!segment.also) {
			for (const Name& var : loopInfo.scope.unassigned) {
				if (VariableSymbol* const v = info.type->Find<VariableSymbol>(var, FileInfo())) {
					v->isAssigned = false;
				}
			}
		}
		else {
			for (const Name& var : loopInfo.mainSegment.unassigned) {
				if (VariableSymbol* const v = info.type->Find<VariableSymbol>(var, FileInfo())) {
					v->isAssigned = false;
				}
			}
		}
	}

	// Copy scope branch
	if (!segment.also) {
		info.scopeInfo = loopInfo.scope.CopyBranch();
	}
	else {
		info.scopeInfo = loopInfo.mainSegment.CopyBranch();
	}
}

void LoopStatement::ScanFirstPostContents(LoopScanInfo& loopInfo, ScanInfo& info) const {
	if (info.init) {
		info.scopeInfo.unassigned = info.type->UnassignedMembers();
	}

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

void LoopStatement::ScanPostContents(LoopScanInfo& loopInfo, ScanInfo& info, const LoopSegment& segment) const {
	if (info.init) {
		info.scopeInfo.unassigned = info.type->UnassignedMembers();
	}

	if (segment.also) {
		loopInfo.alsoSegments.Add(info.scopeInfo);
	}
	else {
		loopInfo.elseSegments.Add(info.scopeInfo);
	}
}

void LoopStatement::ScanCleanup(LoopScanInfo& loopInfo, ScanInfo& info) const {
	// Combine scan info for also segments
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

	// Combine scope info for else segments
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

ScanResult LoopStatement::ScanForCondition(const LoopSegment& segment, ScanInfoStack& info) {
	ScanResult result = segment.init->Scan(info);
	result.SelfUseCheck(info, segment.init->File());

	// Scan condition without condition operator
	if (!segment.conditionOperator) {
		ScanResult r = segment.condition->Scan(info);
		r.SelfUseCheck(info, segment.condition->File());
		result |= r;
	}
	// Scan condition with condition operator
	else {
		Pointer<BinaryOperatorExpression> op = new BinaryOperatorExpression(segment.condition->scope, *segment.conditionOperator, segment.condition->File());
		op->operand1 = segment.init->assignableValues[0];
		op->operand2 = segment.condition->expression;

		ScanResult r = op->Scan(info);
		r.SelfUseCheck(info, segment.condition->File());
		result |= r;
	}

	// Scan step without step operator
	if (!segment.stepOperator) {
		ScanResult r = segment.step->Scan(info);
		r.SelfUseCheck(info, segment.step->File());
		result |= r;
	}
	// Scan step with step operator
	else {
		Fixed<Assignment> assign = Assignment(segment.step->scope, segment.step->File());
		assign->assignableValues.Add(segment.init->assignableValues[0]);

		Ptr<BinaryOperatorExpression> op = new BinaryOperatorExpression(segment.step->scope, *segment.stepOperator, segment.step->File());
		op->operand1 = assign->assignableValues[0];
		op->operand2 = new WeakExpression(segment.step.As<Expression>());

		assign->values.Add(op);

		ScanResult r = assign->Scan(info);
		r.SelfUseCheck(info, segment.step->File());
		result |= r;
	}

	return result;
}

ScanResult LoopStatement::Scan(ScanInfoStack& info) {
	ScanResult result;

	// Setup scan
	info->scopeInfo.EnterScope(ScopeInfo::ScopeType::Scope);
	LoopScanInfo loopInfo = ScanSetup(info.Get());
	info->scopeInfo.ExitScope();

	// Setup segments
	for (UInt i = 0; i < segments.Size(); i++) {
		const LoopSegment& segment = segments[i];

		// Scan pre contents
		info->scopeInfo.EnterScope(segment.IsLoop() ? ScopeInfo::ScopeType::Loop : ScopeInfo::ScopeType::Scope);
		ScanPreContents(loopInfo, info.Get(), segment);

		// Scan condition
		if (segment.type != LoopType::None) {
			ScanResult r = segment.type != LoopType::For ? segment.condition->Scan(info) : ScanForCondition(segment, info);
			r.SelfUseCheck(info, segments[i].condition->File());
			result |= r;
		}

		// Scan body
		ScanResult r = segments[i].statements->Scan(info);
		r.SelfUseCheck(info, segments[i].statements->File());
		result |= r;

		// Update break counts for loops
		if (segments[i].IsLoop()) {
			info->scopeInfo.loopBreakCount  = loopInfo.scope.loopBreakCount;
			info->scopeInfo.scopeBreakCount = loopInfo.scope.scopeBreakCount;
		}

		// Scan post contents
		if (i == 0) {
			ScanFirstPostContents(loopInfo, info.Get());
		}
		else {
			ScanPostContents(loopInfo, info.Get(), segments[i]);
		}

		info->scopeInfo.ExitScope();
	}

	// Cleanup scan
	info->scopeInfo.EnterScope(ScopeInfo::ScopeType::Scope);
	ScanCleanup(loopInfo, info.Get());
	info->scopeInfo.ExitScope();

	return result;
}

NameList LoopStatement::FindSideEffectScope(const bool assign) {
	NameList list = segments[0].statements->GetSideEffectScope(assign);

	for (LoopSegment& segment : segments) {
		if (segment.IsLoop()) {
			return NameList(true, Name::Global);
		}

		if (segment.condition) {
			list = CombineSideEffects(list, segment.condition->GetSideEffectScope(assign));
		}

		list = CombineSideEffects(list, segment.statements->GetSideEffectScope(assign));
	}

	return list;
}

Ptr<Statement> LoopStatement::Optimize(OptimizeInfo& info) {
	for (LoopSegment& segment : segments) {
		if (segment.condition) {
			Node::Optimize(segment.condition, info);
		}

		if (segment.type == LoopType::For) {
			//Node::Optimize(segment.init, info);
			Node::Optimize(segment.step, info);
		}

		Node::Optimize(segment.statements, info);
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

	// Remove loop if all segments are removed
	if (segments.Size() == 0) {
		info.optimized = true;
		return new EmptyStatement();
	}
	// Convert plain segment to do node
	else if (segments.Size() == 1 && (!segments[0].condition || segments[0].condition->IsImmediate())) {
		if (!segments[0].IsLoop()) {
			Ptr<DoStatement> dn = new DoStatement(segments[0].statements->scope, segments[0].statements->File());
			dn->statements = segments[0].statements;
			info.optimized = true;
			return dn;
		}
	}

	return nullptr;
}

StringBuilder LoopStatement::ToMelon(const UInt indent) const {
	StringBuilder sb;
	String tabs1 = String('\t').Repeat(indent);
	String tabs2 = String('\t').Repeat(indent + 1);

	for (UInt i = 0; i < segments.Size(); i++) {
		const LoopSegment& segment = segments[i];

		if (i > 0) {
			sb += segment.also ? "also" : "else";
		}

		switch (segment.type) {
			case LoopType::If:    sb += "if ";    break;
			case LoopType::While: sb += "while "; break;
			case LoopType::For:   sb += "for ";   break;
		}

		if (segment.type != LoopType::None) {
			if (segment.type == LoopType::For) {
				sb += segment.init->ToMelon(indent);
				sb += ", ";

				if (segment.conditionOperator) {
					sb += segment.conditionOperator->ToSimpleString();
					sb += " ";
				}

				sb += segment.condition->ToMelon(indent);
				sb += ", ";

				if (segment.stepOperator) {
					sb += segment.stepOperator->ToSimpleString();
					sb += " ";
				}

				sb += segment.step->ToMelon(indent);
			}
			else {
				sb += segment.condition->ToMelon(indent);
			}
			
			if (segment.type == LoopType::If) {
				sb += " then";
			}
			else {
				sb += " do";
			}
		}

		sb += "\n";
		sb += tabs2;
		sb += segment.statements->ToMelon(indent + 1);
		sb += "\n";
		sb += tabs1;
	}

	sb += "end";

	return sb;
}