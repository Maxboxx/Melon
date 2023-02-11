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

bool GuardStatement::IsScope() const {
	return true;
}

Ptr<Kiwi::Value> GuardStatement::Compile(CompileInfo& info) {
	Ptr<Kiwi::Value> condition = cond->Compile(info);

	const String endLbl = info.NewLabel(); 

	info.currentBlock->AddInstruction(new Kiwi::IfInstruction(condition, endLbl));

	info.PushScope(LoopInfo(endLbl));

	if (else_) {
		else_->Compile(info);
	}

	if (end) {
		end->Compile(info);
	}

	info.PopScope();

	info.NewInstructionBlock(endLbl);
	continue_->Compile(info);
	return nullptr;
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
	bn->loops = 2;
	bn->scopewise = true;

	info->scopeInfo.maxScopeBreakCount = Math::Max(bn->loops, info->scopeInfo.maxScopeBreakCount);

	end = bn;
}

void GuardStatement::AddContinue(ScanInfoStack& info) {
	Ptr<ContinueStatement> cn = new ContinueStatement(scope, file);
	cn->loops = 1;

	info->scopeInfo.maxLoopBreakCount = Math::Max(cn->loops, info->scopeInfo.maxLoopBreakCount);

	end = cn;
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
