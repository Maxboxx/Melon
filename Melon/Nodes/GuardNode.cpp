#include "GuardNode.h"

#include "BreakNode.h"
#include "ContinueNode.h"
#include "ReturnNode.h"
#include "StatementsNode.h"
#include "DoNode.h"

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

UInt GuardNode::GetSize() const {
	return Math::Max(cond->GetSize() + continue_->GetSize(), else_ ? else_->GetSize() : 0);
}

bool GuardNode::IsScope() const {
	return true;
}

CompiledNode GuardNode::Compile(CompileInfo& info) {
	const UInt frame = info.stack.frame;

	CompiledNode compiled = cond->Compile(info);

	info.stack.PopExpr(frame, compiled);

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
	cond->IncludeScan(info);

	if (else_) else_->IncludeScan(info);
	continue_->IncludeScan(info);
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
		info.Get().symbol.Get(var, file).isAssigned = false;
	}
	
	info.Get().scopeInfo = scopeInfo;

	for (const ScanType type : continue_->Scan(info)) {
		scanSet.Add(type);
	}

	info.Get().scopeInfo = ScopeInfo::BranchIntersection(elseScope, info.Get().scopeInfo);

	return scanSet;
}

NodePtr GuardNode::Optimize() {
	if (NodePtr node = cond->Optimize()) cond = node;
	if (NodePtr node = else_->Optimize()) else_ = node;
	if (NodePtr node = continue_->Optimize()) continue_ = node;

	if (cond->IsImmediate()) {
		if (cond->GetImmediate() == 0) {
			Pointer<DoNode> dn = new DoNode(else_->scope, else_->file);
			dn->nodes = else_;
			return dn;
		}
		else {
			return continue_;
		}
	}

	return nullptr;
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

				if (func.returnValues.Size() == 0 && !info.Get().scopeInfo.hasReturned) {
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
