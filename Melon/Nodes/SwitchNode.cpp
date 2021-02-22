#include "SwitchNode.h"

#include "Boxx/Math.h"

#include "MemoryNode.h"
#include "BreakNode.h"
#include "TypeNode.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/FunctionSymbol.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Optimizing;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

SwitchNode::SwitchNode(Symbol* const scope, const FileInfo& file) : Node(scope, file) {

}

SwitchNode::~SwitchNode() {

}

TypeSymbol* SwitchNode::Type() const {
	if (!expr) return nullptr;

	TypeSymbol* type = nodes[0]->Type();

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

void SwitchNode::CompileCaseMatches(SwitchCompileInfo& switchInfo, CompileInfo& info) {
	// Compile cases
	for (const List<NodePtr>& values : cases) {
		List<UInt> jumps;

		// Compile case values
		for (NodePtr node : values) {
			List<NodePtr> nodeArgs;
			nodeArgs.Add(switchInfo.match);
			nodeArgs.Add(node);

			CompiledNode comp = SymbolTable::FindOperator(Scope::Equal, this->match->Type(), node->Type(), node->file)->symbolNode->Compile(nodeArgs, info);
			switchInfo.cn.AddInstructions(comp.instructions);

			Instruction eq = Instruction(InstructionType::Ne, 1);
			eq.arguments.Add(comp.argument);
			eq.arguments.Add(Argument(0));
			switchInfo.cn.instructions.Add(eq);

			jumps.Add(switchInfo.cn.instructions.Size() - 1);
		}

		switchInfo.caseJumps.Add(jumps);
	}
}

void SwitchNode::CompileCaseBodies(SwitchCompileInfo& switchInfo, CompileInfo& info) {
	// Compile nodes
	for (NodePtr expr : nodes) {
		// Add label for case
		switchInfo.cn.instructions.Add(Instruction::Label(info.label));

		for (const UInt i : switchInfo.caseJumps[switchInfo.caseIndex]) {
			switchInfo.cn.instructions[i].instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
		}

		info.label++;
		switchInfo.caseIndex++;

		// Compile statements
		if (!this->expr) {
			CompiledNode compExpr = expr->Compile(info);

			// Check for scopewise breaks
			for (const OptimizerInstruction& in : compExpr.instructions) {
				if (in.instruction.type != InstructionType::Custom) {
					switchInfo.cn.instructions.Add(in);
					continue;
				}

				const String type = in.instruction.instructionName;

				if (type != BreakNode::scopeBreakInstName) {
					switchInfo.cn.instructions.Add(in);
					continue;
				}

				if (in.instruction.sizes[0] > 1) {
					OptimizerInstruction inst = in;
					inst.instruction.sizes[0]--;
					switchInfo.cn.instructions.Add(inst);
				}
				else {
					switchInfo.endJumps.Add(switchInfo.cn.instructions.Size());
					switchInfo.cn.instructions.Add(Instruction(InstructionType::Jmp, 0));
				}
			}
		}
		// Compile expression
		else {
			switchInfo.cn.AddInstructions(CompileAssignment(switchInfo.resultNode, expr, info, expr->file).instructions);
		}

		// Jump to end
		switchInfo.endJumps.Add(switchInfo.cn.instructions.Size());
		switchInfo.cn.instructions.Add(Instruction(InstructionType::Jmp, 0));
	}
}

void SwitchNode::CompileDefault(SwitchCompileInfo& switchInfo, CompileInfo& info) {
	if (def) {
		// Compile statements
		if (!this->expr) {
			CompiledNode defNode = def->Compile(info);

			// Check for scopewise breaks
			for (const OptimizerInstruction& in : defNode.instructions) {
				if (in.instruction.type != InstructionType::Custom) {
					switchInfo.cn.instructions.Add(in);
					continue;
				}

				const String type = in.instruction.instructionName;

				if (type != BreakNode::scopeBreakInstName) {
					switchInfo.cn.instructions.Add(in);
					continue;
				}

				if (in.instruction.sizes[0] > 1) {
					OptimizerInstruction inst = in;
					inst.instruction.sizes[0]--;
					switchInfo.cn.instructions.Add(inst);
				}
				else {
					switchInfo.endJumps.Add(switchInfo.cn.instructions.Size());
					switchInfo.cn.instructions.Add(Instruction(InstructionType::Jmp, 0));
				}
			}
		}
		// Compile expression
		else {
			switchInfo.cn.AddInstructions(CompileAssignment(switchInfo.resultNode, def, info, def->file).instructions);
		}
	}
}

CompiledNode SwitchNode::Compile(CompileInfo& info) {
	// Setup compile info
	SwitchCompileInfo switchInfo;
	switchInfo.caseIndex = 0;

	// Get size of return type
	if (this->expr) {
		switchInfo.cn.size = Type()->Size();
	}

	// Compile match expression
	info.stack.PushExpr(this->match->Type()->Size(), switchInfo.cn);
	switchInfo.match = new MemoryNode(info.stack.Offset());
	switchInfo.match->type = this->match->Type()->AbsoluteName();

	const UInt frame = info.stack.frame;
	switchInfo.cn.AddInstructions(CompileAssignment(switchInfo.match, this->match, info, this->match->file).instructions);

	// Compile cases
	CompileCaseMatches(switchInfo, info);

	if (!this->expr) {
		info.stack.PopExpr(frame, switchInfo.cn);
	}

	info.stack.Pop(this->match->Type()->Size());

	// Jump to default
	Instruction defJmp = Instruction(InstructionType::Jmp, 0);
	switchInfo.defaultJump = switchInfo.cn.instructions.Size();
	switchInfo.cn.instructions.Add(defJmp);

	if (expr) {
		info.stack.PushExpr(Type()->Size(), switchInfo.cn);
	}

	// Get result memory location
	switchInfo.result = Argument(MemoryLocation(info.stack.Offset()));
	switchInfo.resultNode = new MemoryNode(switchInfo.result.mem.offset);
	switchInfo.resultNode->type = Type()->AbsoluteName();

	// Compile case bodies
	CompileCaseBodies(switchInfo, info);

	// Add default label
	switchInfo.cn.instructions.Add(Instruction::Label(info.label));
	switchInfo.cn.instructions[switchInfo.defaultJump].instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
	info.label++;

	// Compile default case
	CompileDefault(switchInfo, info);

	// Add end label
	switchInfo.cn.instructions.Add(Instruction::Label(info.label));

	for (const UInt i : switchInfo.endJumps) {
		switchInfo.cn.instructions[i].instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
	}

	info.label++;

	// Set result argument
	if (expr) {
		switchInfo.cn.argument = switchInfo.result;
		info.stack.Pop(Type()->Size());
	}

	return switchInfo.cn;
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
			switchInfo.scope.unassigned = info.type->UnassignedMembers();
		}
	}

	return switchInfo;
}

void SwitchNode::ScanPreContents(SwitchScanInfo& switchInfo, ScanInfo& info) const {
	if (!expr) {
		if (switchInfo.init) {
			info.init = true;

			for (const Scope& var : switchInfo.scope.unassigned) {
				if (VariableSymbol* const v = info.type->Find<VariableSymbol>(var, FileInfo())) {
					v->isAssigned = false;
				}
			}
		}

		info.scopeInfo = switchInfo.scope.CopyBranch();
	}
}

void SwitchNode::ScanPostContents(SwitchScanInfo& switchInfo, ScanInfo& info) const {
	if (!expr) {
		if (switchInfo.init) {
			info.scopeInfo.unassigned = info.type->UnassignedMembers();
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
				if (VariableSymbol* const v = info.type->Find<VariableSymbol>(var, FileInfo())) {
					v->isAssigned = true;
				}
			}

			for (const Scope& var : switchInfo.scope.unassigned) {
				if (VariableSymbol* const v = info.type->Find<VariableSymbol>(var, FileInfo())) {
					v->isAssigned = false;
				}
			}
		}

		info.scopeInfo = switchInfo.scope;
	}
}

ScanResult SwitchNode::ScanNodes(ScanInfoStack& info) const {
	ScanResult result;

	// Setup scan
	SwitchScanInfo switchInfo = ScanSetup(info.Get());

	// Get type of switch expression
	TypeSymbol* const type = Type();
	Pointer<TypeNode> typeNode = nullptr;

	if (type) {
		typeNode = new TypeNode(type->AbsoluteName());
	}

	// Scan regular case nodes
	for (const NodePtr& node : nodes) {
		ScanPreContents(switchInfo, info.Get());

		ScanResult r = node->Scan(info);
		r.SelfUseCheck(info, node->file);
		result |= r;

		if (expr && typeNode) {
			ScanAssignment(typeNode, node, info, node->file);
		}

		ScanPostContents(switchInfo, info.Get());
	}

	// Scan default node
	if (def) {
		ScanPreContents(switchInfo, info.Get());

		ScanResult r = def->Scan(info);
		r.SelfUseCheck(info, def->file);
		result |= r;

		if (expr && typeNode) {
			ScanAssignment(typeNode, def, info, def->file);
		}

		ScanPostContents(switchInfo, info.Get());
	}

	// Cleanup scan
	ScanCleanup(switchInfo, info.Get());

	return result;
}

ScanResult SwitchNode::Scan(ScanInfoStack& info) {
	ScanResult result = match->Scan(info);
	result.SelfUseCheck(info, match->file);

	// Scan match type
	TypeSymbol* const matchType = match->Type();
	
	if (matchType) {
		ScanAssignment(new TypeNode(matchType->AbsoluteName()), new TypeNode(matchType->AbsoluteName()), info, match->file);
	}

	// Scan Nodes
	result |= ScanNodes(info);

	// Scan case values
	for (const List<NodePtr>& nodeList : cases) {
		for (const NodePtr& node : nodeList) {
			ScanResult r = node->Scan(info);
			r.SelfUseCheck(info, node->file);
			result |= r;

			SymbolTable::FindOperator(Scope::Equal, matchType, node->Type(), node->file);
		}
	}

	return result;
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
	// Optimize nodes
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