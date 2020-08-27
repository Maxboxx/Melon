#include "SwitchNode.h"

#include "Boxx/Math.h"

#include "StackNode.h"
#include "BreakNode.h"

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

CompiledNode SwitchNode::Compile(CompileInfo& info) {
	CompiledNode cn;

	if (this->expr) 
		cn.size = Symbol::Find(Type(), file).size;

	info.stack.Push(Symbol::Find(this->match->Type(), this->match->file).size);
	Pointer<StackNode> matchStack = new StackNode(info.stack.Offset());
	matchStack->type = this->match->Type();

	List<ScopeList> args;
	args.Add(this->match->Type());

	List<NodePtr> nodeArgs;
	nodeArgs.Add(matchStack);
	nodeArgs.Add(this->match);
	CompiledNode match = Symbol::FindFunction(this->match->Type().Add(Scope::Assign), args, this->match->file).node->Compile(nodeArgs, info);

	cn.AddInstructions(match.instructions);

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

			CompiledNode comp = Symbol::FindOperator(Scope::Equal, this->match->Type(), node->Type(), node->file).node->Compile(nodeArgs, info);
			cn.AddInstructions(comp.instructions);

			Instruction eq = Instruction(InstructionType::Ne, 1);
			eq.arguments.Add(comp.argument);
			eq.arguments.Add(Argument(0));
			cn.instructions.Add(eq);

			jumps.Add(cn.instructions.Size() - 1);
		}

		exprJumps.Add(jumps);
	}

	info.stack.Pop(Symbol::Find(this->match->Type(), this->match->file).size);

	Instruction defJmp = Instruction(InstructionType::Jmp, 0);
	defaultJump = cn.instructions.Size();
	cn.instructions.Add(defJmp);

	if (expr)
		info.stack.Push(Symbol::Find(Type(), file).size);

	Argument result = Argument(MemoryLocation(info.stack.Offset()));

	for (NodePtr expr : nodes) {
		cn.instructions.Add(Instruction::Label(info.label));

		for (const UInt i : exprJumps[exprIndex]) {
			cn.instructions[i].instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
		}

		info.label++;
		exprIndex++;

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

		if (this->expr && result != compExpr.argument) {
			Instruction mov = Instruction(InstructionType::Mov, cn.size);
			mov.arguments.Add(result);
			mov.arguments.Add(compExpr.argument);
			cn.instructions.Add(mov);
		}

		endJumps.Add(cn.instructions.Size());
		cn.instructions.Add(Instruction(InstructionType::Jmp, 0));
	}

	cn.instructions.Add(Instruction::Label(info.label));
	cn.instructions[defaultJump].instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
	info.label++;

	if (def) {
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

		if (this->expr) {
			Instruction mov = Instruction(InstructionType::Mov, cn.size);
			mov.arguments.Add(result);
			mov.arguments.Add(defNode.argument);

			cn.instructions.Add(mov);
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
	if (includeScanned) return;

	match->IncludeScan(info);

	for (NodePtr node : nodes) {
		node->IncludeScan(info);
	}

	for (List<NodePtr>& caseList : cases) {
		for (NodePtr c : caseList) {
			c->IncludeScan(info);
		}
	}

	includeScanned = true;
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
				info.symbol.Get(var, FileInfo()).sign = false;
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
			if (switchInfo.willACaseRun) {
				if (i == 0) {
					switchInfo.scope = switchInfo.cases[i];
				}
				else {
					switchInfo.scope = ScopeInfo::BranchIntersection(switchInfo.scope, switchInfo.cases[i]);
				}
			}
			else {
				switchInfo.scope = ScopeInfo::WeakBranchIntersection(switchInfo.scope, switchInfo.cases[i]);
			}
		}

		if (switchInfo.init) {
			for (const Scope& var : switchInfo.scope.unassigned) {
				info.symbol.Get(var, FileInfo()).sign = true;
			}

			for (const Scope& var : switchInfo.scope.unassigned) {
				info.symbol.Get(var, FileInfo()).sign = false;
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

	for (const NodePtr& node : nodes) {
		ScanPreContents(switchInfo, info.Get());

		for (const ScanType type : node->Scan(info)) {
			scanSet.Add(type);

			if (info.Get().init && type == ScanType::Self && !info.Get().symbol.IsAssigned()) {
				ErrorLog::Error(CompileError(CompileError::SelfInit, node->file));
			}
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