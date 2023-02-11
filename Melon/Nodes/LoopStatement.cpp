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
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

LoopStatement::LoopStatement(Symbol* const scope, const FileInfo& file) : Statement(scope, file) {

}

LoopStatement::~LoopStatement() {

}

bool LoopStatement::IsScope() const {
	return true;
}

Ptr<Kiwi::Value> LoopStatement::Compile(CompileInfo& info) {
	Array<String> segmentLabels = Array<String>(segments.Count());

	for (UInt i = 0; i < segmentLabels.Length(); i++) {
		segmentLabels[i] = info.NewLabel();
	}

	for (UInt i = 0; i < segments.Count(); i++) {
		UInt nextTrue, nextFalse;
		GetNextSegments(i, nextTrue, nextFalse);

		LabelInfo labels;
		labels.trueLabel  = segmentLabels[nextTrue - 1];
		labels.falseLabel = segmentLabels[nextFalse - 1];
		labels.endLabel   = segmentLabels.Last();

		LoopSegment& segment = segments[i];

		if (i > 0) {
			info.NewInstructionBlock(segmentLabels[i - 1]);
		}

		switch (segment.type) {
			case LoopType::If:    CompileIfSegment(segment, labels, info);    break;
			case LoopType::While: CompileWhileSegment(segment, labels, info); break;
			case LoopType::For:   CompileForSegment(segment, labels, info);   break;
			case LoopType::None:  CompileNoneSegment(segment, labels, info);  break;
		}
	}

	info.NewInstructionBlock(segmentLabels.Last());

	return nullptr;
}

void LoopStatement::CompileIfSegment(LoopSegment& segment, LabelInfo& labels, CompileInfo& info) const {
	String label = info.NewLabel();
	info.AddInstruction(new Kiwi::IfInstruction(segment.condition->Compile(info), label, labels.falseLabel));
	info.NewInstructionBlock(label);

	segment.statements->Compile(info);
	info.AddInstruction(new Kiwi::GotoInstruction(labels.trueLabel));
}

void LoopStatement::CompileWhileSegment(LoopSegment& segment, LabelInfo& labels, CompileInfo& info) const {
	Ptr<Kiwi::Variable> success = nullptr; 

	if (labels.trueLabel != labels.endLabel || labels.falseLabel != labels.endLabel) {
		success = new Kiwi::Variable(info.NewRegister());
		info.AddInstruction(new Kiwi::AssignInstruction(Kiwi::Type("u8"), success->Copy(), new Kiwi::Integer(0)));
	}

	String outer = info.NewLabel();
	String inner = info.NewLabel();
	info.NewInstructionBlock(outer);

	Ptr<Kiwi::Value> cond = segment.condition->Compile(info);

	if (success) {
		info.AddInstruction(new Kiwi::IfInstruction(cond, inner));
		info.AddInstruction(new Kiwi::IfInstruction(success->Copy(), labels.trueLabel, labels.falseLabel));
	}
	else {
		info.AddInstruction(new Kiwi::IfInstruction(cond, inner, labels.endLabel));
	}

	info.NewInstructionBlock(inner);

	if (success) {
		info.AddInstruction(new Kiwi::AssignInstruction(success->Copy(), new Kiwi::Integer(1)));
	}

	Nodes::LoopInfo loopInfo;
	loopInfo.continueLabel = outer;
	loopInfo.trueLabel     = labels.trueLabel;
	loopInfo.falseLabel    = labels.falseLabel;
	loopInfo.endLabel      = labels.endLabel;

	info.PushLoop(loopInfo);
	segment.statements->Compile(info);
	info.PopLoop();

	info.AddInstruction(new Kiwi::GotoInstruction(outer));
}

void LoopStatement::CompileForSegment(LoopSegment& segment, LabelInfo& labels, CompileInfo& info) const {
	segment.init->Compile(info);

	String outer = info.NewLabel();
	String inner = info.NewLabel();
	String end   = info.NewLabel();

	info.NewInstructionBlock(outer);

	Ptr<Kiwi::Value> cond;

	if (segment.conditionOperator) {
		Ptr<BinaryOperatorExpression> binOp = new BinaryOperatorExpression(segment.condition->scope, *segment.conditionOperator, segment.condition->File());
		binOp->operand1 = new WeakExpression(segment.init->assignableValues[0]);
		binOp->operand2 = new WeakExpression(segment.condition->expression);

		Ptr<Condition> condition = new Condition(segment.condition->scope, segment.condition->File());

		cond = binOp->Compile(info);
	}
	else {
		cond = segment.condition->Compile(info);
	}

	info.AddInstruction(new Kiwi::IfInstruction(cond, inner, labels.trueLabel));

	info.NewInstructionBlock(inner);

	Nodes::LoopInfo loopInfo;
	loopInfo.continueLabel = end;
	loopInfo.trueLabel     = labels.trueLabel;
	loopInfo.falseLabel    = labels.falseLabel;
	loopInfo.endLabel      = labels.endLabel;

	info.PushLoop(loopInfo);
	segment.statements->Compile(info);
	info.PopLoop();

	info.NewInstructionBlock(end);

	if (segment.stepOperator) {
		Ptr<BinaryOperatorExpression> binOp = new BinaryOperatorExpression(segment.step->scope, *segment.stepOperator, segment.step->File());
		binOp->operand1 = new WeakExpression(segment.init->assignableValues[0]);
		binOp->operand2 = new WeakExpression(segment.step.As<Expression>());

		CompileAssignment(segment.init->assignableValues[0], binOp, info, segment.step->File(), false);
	}
	else {
		segment.step->Compile(info);
	}

	info.AddInstruction(new Kiwi::GotoInstruction(outer));
}

void LoopStatement::CompileNoneSegment(LoopSegment& segment, LabelInfo& labels, CompileInfo& info) const {
	segment.statements->Compile(info);
	info.AddInstruction(new Kiwi::GotoInstruction(labels.trueLabel));
}

void LoopStatement::GetNextSegments(const UInt segment, UInt& nextTrue, UInt& nextFalse) const {
	nextTrue = segments.Count();
	nextFalse = segment + 1;

	// Check if the current segment is the last segment
	if (segment != 0 && !IsSegmentLast(segment)) {
		return;
	}

	// Set next false to end for also mismatch
	if (segments[segment].also != segments[nextFalse].also) {
		nextFalse = segments.Count();
	}

	// Get next for main segment
	if (segment == 0) {
		nextTrue = segments.Count();

		// Set next true to first also
		for (UInt u = 1; u < segments.Count(); u++) {
			if (segments[u].also) {
				nextTrue = u;
				break;
			}
		}

		nextFalse = segments.Count();

		// Set next false to first else
		for (UInt u = 1; u < segments.Count(); u++) {
			if (!segments[u].also) {
				nextFalse = u;
				break;
			}
		}
	}
}

bool LoopStatement::IsSegmentLast(const UInt segment) const {
	// Check if the segmet is last
	if (segment == segments.Count() - 1) return true;

	// Check if the segent is the last also
	return segment != 0 && segments[segment].also != segments[segment + 1].also;
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

	for (UInt i = 0; i < segments.Count(); i++) {
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

	ScopeInfo::ScopeType type = info.scopeInfo.type;

	// Copy scope branch
	if (!segment.also) {
		info.scopeInfo = loopInfo.scope.CopyBranch();
	}
	else {
		info.scopeInfo = loopInfo.mainSegment.CopyBranch();
	}

	info.scopeInfo.type = type;
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

		for (UInt i = 1; i < loopInfo.alsoSegments.Count(); i++) {
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
	for (UInt i = 0; i < loopInfo.elseSegments.Count(); i++) {
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
		op->operand1 = new WeakExpression(segment.init->assignableValues[0]);
		op->operand2 = new WeakExpression(segment.condition->expression);

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
		Ptr<Assignment> assign  = new Assignment(segment.step->scope, segment.step->File()); 
		assign->assignableValues.Add(new WeakExpression(segment.init->assignableValues[0]));

		Ptr<BinaryOperatorExpression> op = new BinaryOperatorExpression(segment.step->scope, *segment.stepOperator, segment.step->File());
		op->operand1 = new WeakExpression(assign->assignableValues[0]);
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
	for (UInt i = 0; i < segments.Count(); i++) {
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

	// Remove empty segments
	for (UInt i = segments.Count(); i > 0;) {
		i--;

		if (!segments[i].IsLoop() && IsSegmentLast(i) && IsEmpty(segments[i].statements)) {
			info.optimized = true;
			segments.RemoveAt(i);
		}
	}

	// Remove loop if all segments are removed
	if (segments.IsEmpty()) {
		info.optimized = true;
		return new EmptyStatement();
	}
	// Convert plain segment to do node
	else if (segments.Count() == 1 && !segments[0].condition) {
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

	for (UInt i = 0; i < segments.Count(); i++) {
		const LoopSegment& segment = segments[i];

		if (i > 0) {
			sb += segment.also ? "al" : "el";
		}

		switch (segment.type) {
			case LoopType::If:    sb += "if ";    break;
			case LoopType::While: sb += "while "; break;
			case LoopType::For:   sb += "for ";   break;
			case LoopType::None:  sb += segment.also ? "so" : "se"; break;
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