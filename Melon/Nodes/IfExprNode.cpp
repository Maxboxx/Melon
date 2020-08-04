#include "IfExprNode.h"

#include "Melon/Parsing/Parser.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

IfExprNode::IfExprNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {
	
}

IfExprNode::~IfExprNode() {

}

ScopeList IfExprNode::Type() const {
	ScopeList type = nodes[0]->Type();

	for (UInt i = 1; i < nodes.Size(); i++) {
		if (type != nodes[i]->Type()) ErrorLog::Error(TypeError(TypeError::IfExprType, file));
	}

	return type;
}

CompiledNode IfExprNode::Compile(CompileInfo& info) {
	CompiledNode cn;
	cn.size = info.stack.ptrSize;

	List<UInt> jumps;

	info.stack.Push(Symbol::Find(Type(), file).size);
	cn.argument = Argument(MemoryLocation(info.stack.Offset()));

	for (UInt i = 0; i < conditions.Size(); i++) {
		CompiledNode cond = conditions[i]->Compile(info);

		for (const OptimizerInstruction& inst : cond.instructions) {
			cn.instructions.Add(inst);
		}

		Instruction condInst = Instruction(InstructionType::Eq, 1);
		condInst.arguments.Add(cond.argument);
		condInst.arguments.Add(Argument(0));
		cn.instructions.Add(condInst);

		UInt jump = cn.instructions.Size() - 1;

		CompiledNode expr = nodes[i]->Compile(info);

		for (const OptimizerInstruction& inst : expr.instructions) {
			cn.instructions.Add(inst);
		}

		if (cn.argument != expr.argument) {
			Instruction mov = Instruction(InstructionType::Mov, cn.size);
			mov.arguments.Add(cn.argument);
			mov.arguments.Add(expr.argument);
			cn.instructions.Add(mov);
		}

		jumps.Add(cn.instructions.Size());
		cn.instructions.Add(Instruction(InstructionType::Jmp, 0));

		cn.instructions[jump].instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
		cn.instructions.Add(Instruction::Label(info.label++));
	}

	CompiledNode expr = nodes.Last()->Compile(info);

	for (const OptimizerInstruction& inst : expr.instructions) {
		cn.instructions.Add(inst);
	}

	Instruction mov = Instruction(InstructionType::Mov, cn.size);
	mov.arguments.Add(cn.argument);
	mov.arguments.Add(expr.argument);
	cn.instructions.Add(mov);

	for (UInt i : jumps) {
		cn.instructions[i].instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
	}

	cn.instructions.Add(Instruction::Label(info.label++));

	info.stack.Pop(Symbol::Find(Type(), file).size);

	return cn;
}

void IfExprNode::IncludeScan(ParsingInfo& info) {
	
}

Set<ScanType> IfExprNode::Scan(ScanInfo& info) const { //TODO: add cast node for conditions
	Set<ScanType> scanSet = Set<ScanType>();

	for (const NodePtr& node : nodes) {
		for (const ScanType type : node->Scan(info)) {
			scanSet.Add(type);

			if (info.init && type == ScanType::Self && !info.symbol.IsAssigned()) {
				ErrorLog::Error(CompileError(CompileError::SelfInit, node->file));
			}
		}
	}

	for (const NodePtr& node : conditions) {
		for (const ScanType type : node->Scan(info)) {
			scanSet.Add(type);

			if (info.init && type == ScanType::Self && !info.symbol.IsAssigned()) {
				ErrorLog::Error(CompileError(CompileError::SelfInit, node->file));
			}
		}
	}

	Symbol::Find(Type(), file);

	return scanSet;
}

Mango IfExprNode::ToMango() const {
	Mango mango = Mango("ifexp", MangoType::List);

	for (UInt i = 0; i < nodes.Size(); i++) {
		Mango m = Mango(MangoType::Map);

		if (i == 0) m.SetLabel("if");
		else if (i < conditions.Size()) m.SetLabel("elseif");
		else m.SetLabel("else");

		if (i < conditions.Size()) {
			m.Add("condition", conditions[i]->ToMango());
		}

		m.Add("expr", nodes[i]->ToMango());

		mango.Add(m);
	}

	return mango;
}