#include "GuardNode.h"

#include "BreakNode.h"
#include "ContinueNode.h"
#include "ReturnNode.h"
#include "StatementsNode.h"
#include "DoNode.h"
#include "EmptyNode.h"

#include "Boxx/Math.h"

#include "Kiwi/Kiwi.h"

#include "Melon/Optimizing/OptimizerInstruction.h"

#include "Melon/Symbols/VariableSymbol.h"
#include "Melon/Symbols/FunctionSymbol.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

GuardNode::GuardNode(Symbol* const scope, const FileInfo& file) : Node(scope, file) {

}

GuardNode::~GuardNode() {

}

UInt GuardNode::GetSize() const {
	return Math::Max(cond->GetSize() + continue_->GetSize(), else_ ? else_->GetSize() : 0);
}

bool GuardNode::IsScope() const {
	return true;
}

void GuardNode::CompileElse(CompiledNode& compiled, CompileInfo& info, List<UInt>& jumps) {
	for (const OptimizerInstruction& in : else_->Compile(info).instructions) {
		// Check for custom instructions
		if (in.instruction.type != InstructionType::Custom) {
			compiled.instructions.Add(in);
			continue;
		}

		const String type = in.instruction.instructionName;

		// Check for scope wise breaks
		if (type != BreakNode::scopeBreakInstName) {
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

CompiledNode GuardNode::Compile(CompileInfo& info) {
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

void GuardNode::IncludeScan(ParsingInfo& info) {
	cond->IncludeScan(info);

	if (else_) else_->IncludeScan(info);
	continue_->IncludeScan(info);
}

ScanResult GuardNode::Scan(ScanInfoStack& info) {
	ScanResult result = cond->Scan(info);
	result.SelfUseCheck(info, cond->file);
	
	ScopeInfo scopeInfo = info->scopeInfo.CopyBranch();

	if (info->init) {
		scopeInfo.unassigned = info->type->UnassignedMembers();
	}

	info->scopeInfo.EnterScope(ScopeInfo::ScopeType::Scope);

	if (else_) {
		ScanResult r = else_->Scan(info);
		r.SelfUseCheck(info, else_->file);
		result |= r;
	}

	info->scopeInfo.ExitScope();

	ScopeInfo elseScope = info->scopeInfo.CopyBranch();

	if (info->init) {
		elseScope.unassigned = info->type->UnassignedMembers();
	}

	AddScopeBreak(info);

	for (const Scope& var : scopeInfo.unassigned) {
		if (VariableSymbol* const v = info->type->Find<VariableSymbol>(var, file)) {
			v->isAssigned = false;
		}
	}
	
	info->scopeInfo = scopeInfo;

	ScanResult r = continue_->Scan(info);
	r.SelfUseCheck(info, continue_->file);
	result |= r;

	info->scopeInfo = ScopeInfo::BranchIntersection(elseScope, info->scopeInfo);

	return result;
}

ScopeList GuardNode::FindSideEffectScope(const bool assign) {
	if (else_) {
		return CombineSideEffects(cond->GetSideEffectScope(assign), CombineSideEffects(else_->GetSideEffectScope(assign), continue_->GetSideEffectScope(assign)));
	}
	else {
		return CombineSideEffects(cond->GetSideEffectScope(assign), continue_->GetSideEffectScope(assign));
	}
}

NodePtr GuardNode::Optimize(OptimizeInfo& info) {
	if (NodePtr node = cond->Optimize(info)) cond = node;

	if (else_) {
		if (NodePtr node = else_->Optimize(info)) else_ = node;
	}

	if (NodePtr node = continue_->Optimize(info)) continue_ = node;

	if (cond->IsImmediate()) {
		if (cond->GetImmediate() == 0) {
			if (else_) {
				if (IsEmpty(else_) || !else_->HasSideEffects()) {
					info.optimized = true;
					return new EmptyNode();
				}

				Pointer<DoNode> dn = new DoNode(else_->scope, else_->file);
				dn->nodes = else_;
				info.optimized = true;
				return dn;
			}
			else {
				info.optimized = true;
				return new EmptyNode();
			}
		}
		else {
			info.optimized = true;

			if (IsEmpty(continue_) || !continue_->HasSideEffects()) {
				return new EmptyNode();
			}
			else {
				return continue_;
			}
		}
	}

	return nullptr;
}

void GuardNode::AddScopeBreak(ScanInfoStack& info) {
	if (info->scopeInfo.CanContinue()) {
		switch (info->scopeInfo.type) {
			case ScopeInfo::ScopeType::Scope: {
				Pointer<BreakNode> bn = new BreakNode(scope, file);
				bn->isBreak = true;
				bn->loops = 1;
				bn->scopeWise = true;
				end = bn;

				info->scopeInfo.maxScopeBreakCount = Math::Max(bn->loops, info->scopeInfo.maxScopeBreakCount);
				break;
			}

			case ScopeInfo::ScopeType::Loop: {
				Pointer<ContinueNode> cn = new ContinueNode(scope, file);
				cn->loops = 1;
				end = cn;

				info->scopeInfo.maxLoopBreakCount = Math::Max(cn->loops, info->scopeInfo.maxLoopBreakCount);
				break;
			}

			case ScopeInfo::ScopeType::Function: {
				FunctionSymbol* func = nullptr;
				Symbol* sym = scope;

				while (sym) {
					if (func = sym->Cast<FunctionSymbol>()) {
						break;
					}
					else {
						sym = sym->Parent();
					}
				}

				if (func) {
					if (func->returnValues.IsEmpty() && !info->scopeInfo.hasReturned) {
						end = new ReturnNode(scope, file);
					}
					else if (!info->scopeInfo.hasReturned) {
						// TODO: error
						ErrorLog::Error(CompileError("guard must return from function", file));
					}
				}

				info->scopeInfo.willNotReturn = false;
				break;
			}

			case ScopeInfo::ScopeType::Main: {
				// TODO: error
				ErrorLog::Error(CompileError("guard statements in main scope are not supported yet", file));
				break;
			}
		}
	}
}

StringBuilder GuardNode::ToMelon(const UInt indent) const {
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

	if (!continue_.Cast<StatementsNode>()->statements.IsEmpty()) {
		sb += "\n\n";
		sb += String('\t').Repeat(indent);
		sb += continue_->ToMelon(indent);
	}

	return sb;
}
