#include "GuardStatement.h"

#include "BreakStatement.h"
#include "ContinueStatement.h"
#include "ReturnStatement.h"
#include "Expression.h"
#include "Condition.h"
#include "Statements.h"
#include "DoStatement.h"
#include "EmptyStatement.h"

#include "Boxx/Math.h"

#include "Kiwi/Old/Kiwi.h"

#include "Melon/Optimizing/OptimizerInstruction.h"

#include "Melon/Symbols/VariableSymbol.h"
#include "Melon/Symbols/FunctionSymbol.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

GuardStatement::GuardStatement(Symbol* const scope, const FileInfo& file) : Statement(scope, file) {

}

GuardStatement::~GuardStatement() {

}

UInt GuardStatement::GetSize() const {
	return Math::Max(cond->GetSize() + continue_->GetSize(), else_ ? else_->GetSize() : 0);
}

bool GuardStatement::IsScope() const {
	return true;
}

void GuardStatement::CompileElse(CompiledNode& compiled, CompileInfo& info, List<UInt>& jumps) {
	for (const OptimizerInstruction& in : else_->Compile(info).instructions) {
		// Check for custom instructions
		if (in.instruction.type != InstructionType::Custom) {
			compiled.instructions.Add(in);
			continue;
		}

		const String type = in.instruction.instructionName;

		// Check for scope wise breaks
		if (type != BreakStatement::scopeBreakInstName) {
			compiled.instructions.Add(in);
			continue;
		}

		// Handle scope wise breaks
		if (in.instruction.sizes[0] > 1) {
			OptimizerInstruction inst = in;
			inst.instruction.sizes[0]--;
			compiled.instructions.Add(inst);
		}
		else {
			Instruction jmp = Instruction(InstructionType::Jmp);
			jumps.Add(compiled.instructions.Size());
			compiled.instructions.Add(jmp);
		}
	}
}

CompiledNode GuardStatement::Compile(CompileInfo& info) {
	const UInt frame = info.stack.frame;

	// Compile condition
	CompiledNode compiled = cond->Compile(info);

	info.stack.PopExpr(frame, compiled);

	Instruction ne = Instruction(InstructionType::Ne, 1);
	ne.arguments.Add(compiled.argument);
	ne.arguments.Add(Argument(0));

	UInt jumpIndex = compiled.instructions.Size();
	compiled.instructions.Add(ne);

	List<UInt> jumps;

	// Compile else statements
	if (else_) {
		CompileElse(compiled, info, jumps);
	}

	// Add labels for breaks
	if (!jumps.IsEmpty()) {
		Instruction lbl = Instruction::Label(info.label);
		compiled.instructions.Add(lbl);

		for (const UInt jump : jumps) {
			compiled.instructions[jump].instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
		}

		info.label++;
	}

	// Compile extra break
	if (end) {
		compiled.AddInstructions(end->Compile(info).instructions);
	}

	// Compile the continue statements
	compiled.instructions.Add(Instruction::Label(info.label));
	compiled.instructions[jumpIndex].instruction.arguments.Add(Argument(ArgumentType::Label, info.label++));

	compiled.AddInstructions(continue_->Compile(info).instructions);

	return compiled;
}

void GuardStatement::IncludeScan(ParsingInfo& info) {
	cond->IncludeScan(info);

	if (else_) else_->IncludeScan(info);
	continue_->IncludeScan(info);
}

ScanResult GuardStatement::Scan(ScanInfoStack& info) {
	ScanResult result = cond->Scan(info);
	result.SelfUseCheck(info, cond->File());
	
	// Setup scope info
	ScopeInfo scopeInfo = info->scopeInfo.CopyBranch();

	if (info->init) {
		scopeInfo.unassigned = info->type->UnassignedMembers();
	}

	info->scopeInfo.EnterScope(ScopeInfo::ScopeType::Scope);

	// Scan else
	if (else_) {
		ScanResult r = else_->Scan(info);
		r.SelfUseCheck(info, else_->File());
		result |= r;
	}

	info->scopeInfo.ExitScope();

	ScopeInfo elseScope = info->scopeInfo.CopyBranch();

	if (info->init) {
		elseScope.unassigned = info->type->UnassignedMembers();
	}

	AddScopeBreak(info);

	// Reset unassigned variables
	for (const Name& var : scopeInfo.unassigned) {
		if (VariableSymbol* const v = info->type->Find<VariableSymbol>(var, file)) {
			v->isAssigned = false;
		}
	}
	
	info->scopeInfo = scopeInfo;

	// Scan continue
	ScanResult r = continue_->Scan(info);
	r.SelfUseCheck(info, continue_->File());
	result |= r;

	info->scopeInfo = ScopeInfo::BranchIntersection(elseScope, info->scopeInfo);

	return result;
}

NameList GuardStatement::FindSideEffectScope(const bool assign) {
	if (else_) {
		return CombineSideEffects(cond->GetSideEffectScope(assign), CombineSideEffects(else_->GetSideEffectScope(assign), continue_->GetSideEffectScope(assign)));
	}
	else {
		return CombineSideEffects(cond->GetSideEffectScope(assign), continue_->GetSideEffectScope(assign));
	}
}

Ptr<Statement> GuardStatement::Optimize(OptimizeInfo& info) {
	// Optimize nodes
	Node::Optimize(cond, info);

	if (else_) Node::Optimize(else_, info);

	Node::Optimize(continue_, info);

	// Optimize condition for immediate values
	if (cond->IsImmediate()) {
		info.optimized = true;

		// Optimize false
		if (cond->GetImmediate() == 0) {
			return OptimizeFalseCondition(info);
		}
		// Optimize true
		else {
			return OptimizeTrueCondition(info);
		}
	}

	return nullptr;
}

Ptr<Statement> GuardStatement::OptimizeFalseCondition(OptimizeInfo& info) {
	// Optimize else segment if it exists
	if (else_) {
		if (IsEmpty(else_) || !else_->HasSideEffects()) {
			return new EmptyStatement();
		}

		Ptr<DoStatement> dn = new DoStatement(else_->scope, else_->File());
		dn->statements = else_;
		return dn;
	}
	// Replace guard with empty node
	else {
		return new EmptyStatement();
	}
}

Ptr<Statement> GuardStatement::OptimizeTrueCondition(OptimizeInfo& info) {
	if (IsEmpty(continue_) || !continue_->HasSideEffects()) {
		return new EmptyStatement();
	}
	else {
		Ptr<DoStatement> dn = new DoStatement(else_->scope, else_->File());
		dn->statements = continue_;
		return dn;
	}
}

void GuardStatement::AddScopeBreak(ScanInfoStack& info) {
	if (info->scopeInfo.CanContinue()) {
		switch (info->scopeInfo.type) {
			case ScopeInfo::ScopeType::Scope: {
				AddScopeWiseBreak(info);
				break;
			}

			case ScopeInfo::ScopeType::Loop: {
				AddContinue(info);
				break;
			}

			// Return from function
			case ScopeInfo::ScopeType::Function: {
				AddReturn(info);
				break;
			}
			
			// Throw error
			case ScopeInfo::ScopeType::Main: {
				AddThrow(info);
				break;
			}
		}
	}
}

void GuardStatement::AddScopeWiseBreak(ScanInfoStack& info) {
	Ptr<BreakStatement> bn = new BreakStatement(scope, file);
	bn->isBreak = true;
	bn->loops = 1;
	bn->scopewise = true;
	end = bn;

	info->scopeInfo.maxScopeBreakCount = Math::Max(bn->loops, info->scopeInfo.maxScopeBreakCount);
}

void GuardStatement::AddContinue(ScanInfoStack& info) {
	Ptr<ContinueStatement> cn = new ContinueStatement(scope, file);
	cn->loops = 1;
	end = cn;

	info->scopeInfo.maxLoopBreakCount = Math::Max(cn->loops, info->scopeInfo.maxLoopBreakCount);
}

void GuardStatement::AddReturn(ScanInfoStack& info) {
	FunctionSymbol* func = SymbolTable::Find<FunctionSymbol>(scope->CurrentFunction()->AbsoluteName(), scope->AbsoluteName(), file, SymbolTable::SearchOptions::ReplaceTemplates);

	if (func) {
		if (func->returnValues.IsEmpty() && !info->scopeInfo.hasReturned) {
			end = new ReturnStatement(scope, file);
		}
		else if (!info->scopeInfo.hasReturned) {
			ErrorLog::Error(LogMessage("error.scan.guard.return"), file);
		}
	}

	info->scopeInfo.willNotReturn = false;
}

void GuardStatement::AddThrow(ScanInfoStack& info) {
	ErrorLog::Error(LogMessage("error.scan.guard.main"), file);
}

StringBuilder GuardStatement::ToMelon(const UInt indent) const {
	StringBuilder sb = "guard ";
	sb += cond->ToMelon(indent);

	if (else_) {
		sb += " else\n";
		sb += String('\t').Repeat(indent + 1);
		sb += else_->ToMelon(indent + 1);
		sb += "\n";
		sb += String('\t').Repeat(indent);
		sb += "end";
	}

	if (!continue_->statements.IsEmpty()) {
		sb += "\n\n";
		sb += String('\t').Repeat(indent);
		sb += continue_->ToMelon(indent);
	}

	return sb;
}
