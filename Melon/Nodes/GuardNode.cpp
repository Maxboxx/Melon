#include "GuardNode.h"

#include "BreakNode.h"
#include "ContinueNode.h"
#include "ReturnNode.h"
#include "StatementsNode.h"

#include "Boxx/Math.h"

#include "Kiwi/Kiwi.h"

#include "Melon/Optimizing/OptimizerInstruction.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

GuardNode::GuardNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

GuardNode::~GuardNode() {

}

CompiledNode GuardNode::Compile(CompileInfo& info) {
	CompiledNode compiled = cond->Compile(info);

	Instruction ne = Instruction(InstructionType::Ne, 1);
	ne.arguments.Add(compiled.argument);
	ne.arguments.Add(Argument(0));

	UInt jumpIndex = compiled.instructions.Size();
	compiled.instructions.Add(ne);

	List<UInt> jumps;

	if (else_) for (const OptimizerInstruction& in : else_->Compile(info).instructions) {
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
			jumps.Add(compiled.instructions.Size());
			compiled.instructions.Add(jmp);
		}
	}

	if (!jumps.IsEmpty()) {
		Instruction lbl = Instruction::Label(info.label);
		compiled.instructions.Add(lbl);

		for (const UInt jump : jumps) {
			compiled.instructions[jump].instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
		}

		info.label++;
	}

	if (end) {
		compiled.AddInstructions(end->Compile(info).instructions);
	}

	compiled.instructions.Add(Instruction::Label(info.label));
	compiled.instructions[jumpIndex].instruction.arguments.Add(Argument(ArgumentType::Label, info.label++));

	compiled.AddInstructions(continue_->Compile(info).instructions);

	return compiled;
}

void GuardNode::IncludeScan(ParsingInfo& info) {
	if (includeScanned) return;

	cond->IncludeScan(info);

	if (else_) else_->IncludeScan(info);
	continue_->IncludeScan(info);

	includeScanned = true;
}

Set<ScanType> GuardNode::Scan(ScanInfoStack& info) {
	Set<ScanType> scanSet = cond->Scan(info);

	ScopeInfo scopeInfo = info.Get().scopeInfo.CopyBranch();
	if (info.Get().init) scopeInfo.unassigned = info.Get().symbol.GetUnassignedVarsSet();

	info.Get().scopeInfo.EnterScope(ScopeInfo::ScopeType::Scope);

	if (else_) for (const ScanType type : else_->Scan(info)) {
		scanSet.Add(type);

		if (info.Get().init && type == ScanType::Self && !info.Get().symbol.IsAssigned()) {
			ErrorLog::Error(CompileError(CompileError::SelfInit, else_->file));
		}
	}

	info.Get().scopeInfo.ExitScope();

	ScopeInfo elseScope = info.Get().scopeInfo.CopyBranch();
	if (info.Get().init) elseScope.unassigned = info.Get().symbol.GetUnassignedVarsSet();

	AddScopeBreak(info);

	for (const Scope& var : scopeInfo.unassigned) {
		info.Get().symbol.Get(var, file).sign = false;
	}
	
	info.Get().scopeInfo = scopeInfo;

	for (const ScanType type : continue_->Scan(info)) {
		scanSet.Add(type);
	}

	info.Get().scopeInfo = ScopeInfo::BranchIntersection(elseScope, info.Get().scopeInfo);

	return scanSet;
}

void GuardNode::AddScopeBreak(ScanInfoStack& info) {
	if (info.Get().scopeInfo.CanContinue()) {
		switch (info.Get().scopeInfo.type) {
			case ScopeInfo::ScopeType::Scope: {
				Pointer<BreakNode> bn = new BreakNode(scope, file);
				bn->isBreak = true;
				bn->loops = 1;
				bn->scopeWise = true;
				end = bn;

				info.Get().scopeInfo.maxScopeBreakCount = Math::Max(bn->loops, info.Get().scopeInfo.maxScopeBreakCount);
				break;
			}

			case ScopeInfo::ScopeType::Loop: {
				Pointer<ContinueNode> cn = new ContinueNode(scope, file);
				cn->loops = 1;
				end = cn;

				info.Get().scopeInfo.maxLoopBreakCount = Math::Max(cn->loops, info.Get().scopeInfo.maxLoopBreakCount);
				break;
			}

			case ScopeInfo::ScopeType::Function: {
				Symbol func = Symbol::FindCurrentFunction(scope, file);

				if (func.ret.Size() == 0 && !info.Get().scopeInfo.hasReturned) {
					end = new ReturnNode(scope, file);
				}
				else if (!info.Get().scopeInfo.hasReturned) {
					ErrorLog::Error(CompileError("guard must return from function", file));
				}

				info.Get().scopeInfo.willNotReturn = false;

				break;
			}

			case ScopeInfo::ScopeType::Main: {
				ErrorLog::Error(CompileError("guard statements in main scope are not supported yet", file));
				break;
			}
		}
	}
}

Mango GuardNode::ToMango() const {
	Mango mango = Mango("guard", MangoType::Map);
	mango.Add("condition", cond->ToMango());
	if (else_) mango.Add("else", else_->ToMango());
	mango.Add("continue", continue_->ToMango());
	return mango;
}
