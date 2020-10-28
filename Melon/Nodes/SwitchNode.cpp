#include "SwitchNode.h"

#include "Boxx/Math.h"

#include "StackNode.h"
#include "BreakNode.h"
#include "TypeNode.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Optimizing;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

SwitchNode::SwitchNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

SwitchNode::~SwitchNode() {

}

ScopeList SwitchNode::Type() const {
	if (!expr) return ScopeList::undefined;

	ScopeList type = nodes[0]->Type();

	for (UInt i = 1; i < nodes.Size(); i++) {
		if (type != nodes[i]->Type()) ErrorLog::Error(TypeError(TypeError::SwitchExprType, file));
	}

	return type;
}

UInt SwitchNode::GetSize() const {
	if (expr) return 0;

	UInt size = 0;

	for (const NodePtr& node : nodes) {
		size = Math::Max(size, node->GetSize());
	}

	return size;
}

bool SwitchNode::IsScope() const {
	return !expr;
}

CompiledNode SwitchNode::Compile(CompileInfo& info) {
	CompiledNode cn;

	if (this->expr) 
		cn.size = Symbol::Find(Type(), file).size;

	info.stack.PushExpr(Symbol::Find(this->match->Type(), this->match->file).size, cn);
	Pointer<StackNode> matchStack = new StackNode(info.stack.Offset());
	matchStack->type = this->match->Type();

	List<ScopeList> args;
	args.Add(this->match->Type());

	const UInt frame = info.stack.frame;
	cn.AddInstructions(CompileAssignment(matchStack, this->match, info, this->match->file).instructions);

	List<UInt> endJumps;
	List<List<UInt>> exprJumps;
	UInt exprIndex = 0;
	UInt defaultJump;

	for (const List<NodePtr>& values : cases) {
		List<UInt> jumps;

		for (NodePtr node : values) {
			List<NodePtr> nodeArgs;
			nodeArgs.Add(matchStack);
			nodeArgs.Add(node);

			CompiledNode comp = Symbol::FindOperator(Scope::Equal, this->match->Type(), node->Type(), node->file).symbolNode->Compile(nodeArgs, info);
			cn.AddInstructions(comp.instructions);

			Instruction eq = Instruction(InstructionType::Ne, 1);
			eq.arguments.Add(comp.argument);
			eq.arguments.Add(Argument(0));
			cn.instructions.Add(eq);

			jumps.Add(cn.instructions.Size() - 1);
		}

		exprJumps.Add(jumps);
	}

	if (!this->expr) {
		info.stack.PopExpr(frame, cn);
	}

	info.stack.Pop(Symbol::Find(this->match->Type(), this->match->file).size);

	Instruction defJmp = Instruction(InstructionType::Jmp, 0);
	defaultJump = cn.instructions.Size();
	cn.instructions.Add(defJmp);

	if (expr)
		info.stack.PushExpr(Symbol::Find(Type(), file).size, cn);

	Argument result = Argument(MemoryLocation(info.stack.Offset()));

	Pointer<StackNode> sn = new StackNode(result.mem.offset);
	sn->type = Type();

	for (NodePtr expr : nodes) {
		cn.instructions.Add(Instruction::Label(info.label));

		for (const UInt i : exprJumps[exprIndex]) {
			cn.instructions[i].instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
		}

		info.label++;
		exprIndex++;

		if (!this->expr) {
			CompiledNode compExpr = expr->Compile(info);

			for (const OptimizerInstruction& in : compExpr.instructions) {
				if (in.instruction.type != InstructionType::Custom) {
					cn.instructions.Add(in);
					continue;
				}

				const String type = in.instruction.instructionName;

				if (type != BreakNode::scopeBreakInstName) {
					cn.instructions.Add(in);
					continue;
				}

				if (in.instruction.sizes[0] > 1) {
					OptimizerInstruction inst = in;
					inst.instruction.sizes[0]--;
					cn.instructions.Add(inst);
				}
				else {
					endJumps.Add(cn.instructions.Size());
					cn.instructions.Add(Instruction(InstructionType::Jmp, 0));
				}
			}
		}
		else {
			cn.AddInstructions(CompileAssignment(sn, expr, info, expr->file).instructions);
		}

		endJumps.Add(cn.instructions.Size());
		cn.instructions.Add(Instruction(InstructionType::Jmp, 0));
	}

	cn.instructions.Add(Instruction::Label(info.label));
	cn.instructions[defaultJump].instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
	info.label++;

	if (def) {
		if (!this->expr) {
			CompiledNode defNode = def->Compile(info);

			for (const OptimizerInstruction& in : defNode.instructions) {
				if (in.instruction.type != InstructionType::Custom) {
					cn.instructions.Add(in);
					continue;
				}

				const String type = in.instruction.instructionName;

				if (type != BreakNode::scopeBreakInstName) {
					cn.instructions.Add(in);
					continue;
				}

				if (in.instruction.sizes[0] > 1) {
					OptimizerInstruction inst = in;
					inst.instruction.sizes[0]--;
					cn.instructions.Add(inst);
				}
				else {
					endJumps.Add(cn.instructions.Size());
					cn.instructions.Add(Instruction(InstructionType::Jmp, 0));
				}
			}
		}
		else {
			cn.AddInstructions(CompileAssignment(sn, def, info, def->file).instructions);
		}
	}

	cn.instructions.Add(Instruction::Label(info.label));

	for (const UInt i : endJumps) {
		cn.instructions[i].instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
	}

	info.label++;

	if (expr) {
		cn.argument = result;
		info.stack.Pop(Symbol::Find(Type(), file).size);
	}

	return cn;
}

void SwitchNode::IncludeScan(ParsingInfo& info) {
	match->IncludeScan(info);

	for (NodePtr node : nodes) {
		node->IncludeScan(info);
	}

	for (List<NodePtr>& caseList : cases) {
		for (NodePtr c : caseList) {
			c->IncludeScan(info);
		}
	}
}

SwitchNode::SwitchScanInfo SwitchNode::ScanSetup(ScanInfo& info) const {
	SwitchScanInfo switchInfo;
	switchInfo.init = info.init;

	if (!expr) {
		switchInfo.willACaseRun = def != nullptr;
		switchInfo.scope = info.scopeInfo.CopyBranch();

		if (switchInfo.init) {
			switchInfo.scope.unassigned = info.symbol.GetUnassignedVarsSet();
		}
	}

	return switchInfo;
}

void SwitchNode::ScanPreContents(SwitchScanInfo& switchInfo, ScanInfo& info) const {
	if (!expr) {
		if (switchInfo.init) {
			info.init = true;

			for (const Scope& var : switchInfo.scope.unassigned) {
				info.symbol.Get(var, FileInfo()).isAssigned = false;
			}
		}

		info.scopeInfo = switchInfo.scope.CopyBranch();
	}
}

void SwitchNode::ScanPostContents(SwitchScanInfo& switchInfo, ScanInfo& info) const {
	if (!expr) {
		if (switchInfo.init) {
			info.scopeInfo.unassigned = info.symbol.GetUnassignedVarsSet();
		}

		switchInfo.cases.Add(info.scopeInfo);
	}
}

void SwitchNode::ScanCleanup(SwitchScanInfo& switchInfo, ScanInfo& info) const {
	if (!expr) {
		for (UInt i = 0; i < switchInfo.cases.Size(); i++) {
			if (i == 0) {
				switchInfo.scope = switchInfo.cases[i];
			}
			else if (switchInfo.willACaseRun) {
				switchInfo.scope = ScopeInfo::BranchIntersection(switchInfo.scope, switchInfo.cases[i]);
			}
			else {
				switchInfo.scope = ScopeInfo::WeakBranchIntersection(switchInfo.scope, switchInfo.cases[i]);
			}
		}

		if (switchInfo.init) {
			for (const Scope& var : switchInfo.scope.unassigned) {
				info.symbol.Get(var, FileInfo()).isAssigned = true;
			}

			for (const Scope& var : switchInfo.scope.unassigned) {
				info.symbol.Get(var, FileInfo()).isAssigned = false;
			}
		}

		info.scopeInfo = switchInfo.scope;
	}
}

Set<ScanType> SwitchNode::Scan(ScanInfoStack& info) {
	Set<ScanType> scanSet = match->Scan(info);

	if (info.Get().init && scanSet.Contains(ScanType::Self) && !info.Get().symbol.IsAssigned()) {
		ErrorLog::Error(CompileError(CompileError::SelfInit, match->file));
	}

	if (expr) {
		Symbol::Find(Type(), file);
	}

	Symbol::Find(this->match->Type(), this->match->file);

	List<ScopeList> args;
	args.Add(this->match->Type());
	Symbol::FindFunction(this->match->Type().Add(Scope::Assign), args, this->match->file);

	SwitchScanInfo switchInfo = ScanSetup(info.Get());

	Pointer<TypeNode> type = new TypeNode(Type());

	for (const NodePtr& node : nodes) {
		ScanPreContents(switchInfo, info.Get());

		for (const ScanType type : node->Scan(info)) {
			scanSet.Add(type);

			if (info.Get().init && type == ScanType::Self && !info.Get().symbol.IsAssigned()) {
				ErrorLog::Error(CompileError(CompileError::SelfInit, node->file));
			}
		}

		if (expr) {
			ScanAssignment(type, node, info, node->file);
		}

		ScanPostContents(switchInfo, info.Get());
	}

	if (def) {
		ScanPreContents(switchInfo, info.Get());

		for (const ScanType type : def->Scan(info)) {
			scanSet.Add(type);

			if (info.Get().init && type == ScanType::Self && !info.Get().symbol.IsAssigned()) {
				ErrorLog::Error(CompileError(CompileError::SelfInit, def->file));
			}
		}

		if (expr) {
			ScanAssignment(type, def, info, def->file);
		}

		ScanPostContents(switchInfo, info.Get());
	}

	ScanCleanup(switchInfo, info.Get());

	for (const List<NodePtr>& nodeList : cases) {
		for (const NodePtr& node : nodeList) {
			for (const ScanType type : node->Scan(info)) {
				scanSet.Add(type);

				if (info.Get().init && type == ScanType::Self && !info.Get().symbol.IsAssigned()) {
					ErrorLog::Error(CompileError(CompileError::SelfInit, node->file));
				}
			}

			Symbol::FindOperator(Scope::Equal, this->match->Type(), node->Type(), node->file);
		}
	}

	return scanSet;
}

ScopeList SwitchNode::FindSideEffectScope(const bool assign) {
	ScopeList list = match->GetSideEffectScope(assign);

	for (NodePtr& node : nodes) {
		list = CombineSideEffects(list, node->GetSideEffectScope(assign));
	}

	for (List<NodePtr>& caseList : cases) {
		for (NodePtr& node : caseList) {
			list = CombineSideEffects(list, node->GetSideEffectScope(assign));
		}
	}

	if (def) {
		list = CombineSideEffects(list, def->GetSideEffectScope(assign));
	}

	return list;
}

NodePtr SwitchNode::Optimize(OptimizeInfo& info) {
	if (NodePtr node = match->Optimize(info)) match = node;

	for (NodePtr& node : nodes) {
		if (NodePtr n = node->Optimize(info)) node = n;
	}

	for (List<NodePtr>& caseList : cases) {
		for (NodePtr& node : caseList) {
			if (NodePtr n = node->Optimize(info)) node = n;
		}
	}

	if (def) {
		if (NodePtr node = def->Optimize(info)) def = node;
	}

	// TODO: Check for side effects in cases

	// Remove empty cases
	if (!expr) {
		for (UInt i = 0; i < nodes.Size(); i++) {
			if (IsEmpty(nodes[i])) {
				cases.RemoveAt(i);
				nodes.RemoveAt(i);
				i--;
				info.optimized = true;
			}
		}

		if (def && IsEmpty(def)) {
			def = nullptr;
			info.optimized = true;
		}
	}

	return nullptr;
}

Mango SwitchNode::ToMango() const {
	Mango mango = Mango(expr ? "switchexpr" : "switch", MangoType::Map);

	mango.Add("match", match->ToMango());

	Mango cases = Mango(MangoType::List);

	for (UInt i = 0; i < nodes.Size(); i++) {
		Mango m = Mango(MangoType::Map);

		Mango list = Mango(MangoType::List);

		for (NodePtr node : this->cases[i]) {
			list.Add(node->ToMango());
		}

		m.Add("values", list);

		m.Add(expr ? "expr" : "content", nodes[i]->ToMango());

		cases.Add(m);
	}

	mango.Add("cases", cases);

	mango.Add("default", def ? def->ToMango() : Mango(MangoType::Nil));

	return mango;
}

StringBuilder SwitchNode::ToMelon(const UInt indent) const {
	StringBuilder sb = "switch ";
	sb += match->ToMelon(indent);
	
	String tabs1 = String('\t').Repeat(indent + 1);
	String tabs2 = String('\t').Repeat(indent + 2);

	for (UInt i = 0; i < nodes.Size(); i++) {
		sb += "\n";
		sb += tabs1;
		sb += "case ";

		for (UInt u = 0; u < cases[i].Size(); u++) {
			if (u > 0) sb += ", ";
			sb += cases[i][u]->ToMelon(indent + 1);
		}

		sb += " then\n";
		sb += tabs2;
		sb += nodes[i]->ToMelon(indent + 2);
		sb += "\n";
		sb += tabs1;
		sb += "end\n";
	}

	if (def) {
		sb += "\n";
		sb += tabs1;
		sb += "default\n";
		sb += tabs2;
		sb += def->ToMelon(indent + 2);
		sb += "\n";
		sb += tabs1;
		sb += "end\n";
	}

	sb += String('\t').Repeat(indent);
	sb += "end";

	return sb;
}