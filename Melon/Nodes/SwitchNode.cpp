#include "SwitchNode.h"

#include "StackNode.h"

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

		for (const OptimizerInstruction& instruction : compExpr.instructions) {
			cn.instructions.Add(instruction);
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

	CompiledNode defNode = def->Compile(info);

	for (const OptimizerInstruction& instruction : defNode.instructions) {
		cn.instructions.Add(instruction);
	}

	if (this->expr) {
		Instruction mov = Instruction(InstructionType::Mov, cn.size);
		mov.arguments.Add(result);
		mov.arguments.Add(defNode.argument);

		cn.instructions.Add(mov);
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

Set<ScanType> SwitchNode::Scan(ScanInfo& info) const {
	Set<ScanType> scanSet = match->Scan(info);

	if (info.init && scanSet.Contains(ScanType::Self) && !info.symbol.IsAssigned()) {
		ErrorLog::Error(CompileError(CompileError::SelfInit, match->file));
	}

	Symbol::Find(Type(), file);
	Symbol::Find(this->match->Type(), this->match->file);

	List<ScopeList> args;
	args.Add(this->match->Type());
	Symbol::FindFunction(this->match->Type().Add(Scope::Assign), args, this->match->file);

	for (const NodePtr& node : nodes) {
		for (const ScanType type : node->Scan(info)) {
			scanSet.Add(type);

			if (info.init && type == ScanType::Self && !info.symbol.IsAssigned()) {
				ErrorLog::Error(CompileError(CompileError::SelfInit, node->file));
			}
		}
	}

	for (const List<NodePtr>& nodeList : cases) {
		for (const NodePtr& node : nodeList) {
			for (const ScanType type : node->Scan(info)) {
				scanSet.Add(type);

				if (info.init && type == ScanType::Self && !info.symbol.IsAssigned()) {
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