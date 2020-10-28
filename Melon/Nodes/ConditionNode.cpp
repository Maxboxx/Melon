#include "ConditionNode.h"

#include "ConvertNode.h"
#include "AssignNode.h"
#include "BooleanNode.h"
#include "ArgumentNode.h"
#include "TypeNode.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

String ConditionNode::conditionInstName = "assigncond";

ConditionNode::ConditionNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

ConditionNode::~ConditionNode() {

}

ScopeList ConditionNode::Type() const {
	return cond->Type();//ScopeList().Add(Scope::Bool);
}

UInt ConditionNode::GetSize() const {
	return cond.Is<AssignNode>() ? cond->GetSize() : 0;
}

CompiledNode ConditionNode::Compile(CompileInfo& info) {
	if (Pointer<AssignNode> assign = cond.Cast<AssignNode>()) {
		CompiledNode c = assign->values[0]->Compile(info);

		Symbol s = Symbol::Find(assign->values[0]->Type(), file);
		Argument argCopy = c.argument;
		argCopy.mem.offset++;

		NodePtr tempValue = assign->values[0];

		Pointer<ArgumentNode> value = new ArgumentNode(argCopy);
		value->type = s.Get(Scope::Value, file).varType;
		assign->values[0] = value;

		OptimizerInstruction mov = Instruction(InstructionType::Mov, 1);
		mov.instruction.arguments.Add(Argument(Register(info.index++)));
		mov.instruction.arguments.Add(c.argument);
		//mov.important = true;
		c.argument = mov.instruction.arguments[0];
		c.instructions.Add(mov);

		Instruction eq = Instruction(InstructionType::Eq, 1);
		eq.arguments.Add(c.argument);
		eq.arguments.Add(Argument(0));

		UInt eqIndex = c.instructions.Size();
		c.instructions.Add(eq);

		c.AddInstructions(assign->Compile(info).instructions);
		assign->values[0] = tempValue;

		c.instructions[eqIndex].instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
		c.instructions.Add(Instruction::Label(info.label++));

		return c;
	}
	else {
		Pointer<ConvertNode> convert = new ConvertNode(scope, file);
		convert->isExplicit = true;
		convert->node = cond;
		convert->type = ScopeList::Bool;
		return convert->Compile(info);
	}
}

void ConditionNode::IncludeScan(ParsingInfo& info) {
	cond->IncludeScan(info);
}

Set<ScanType> ConditionNode::Scan(ScanInfoStack& info) {
	if (Pointer<AssignNode> assign = cond.Cast<AssignNode>()) {
		NodePtr tempValue = assign->values[0];

		Symbol s = Symbol::Find(assign->values[0]->Type(), file);

		if (s.scope.Size() > 0 && s.scope[0].name == Scope::Optional.name) {
			Pointer<TypeNode> value = new TypeNode(s.Get(Scope::Value, file).varType);
			assign->values[0] = value;
		}
		else {
			ErrorLog::Error(TypeError(TypeError::ConditionAssignment(tempValue->Type().ToString()), tempValue->file));
		}

		Set<ScanType> scanSet = Set<ScanType>::Union(cond->Scan(info), tempValue->Scan(info));

		assign->values[0] = tempValue;
		return scanSet;
	}
	else {
		Pointer<ConvertNode> convert = new ConvertNode(scope, file);
		convert->isExplicit = true;
		convert->node = cond;
		convert->type = ScopeList::Bool;
		return convert->Scan(info);
	}
}

ScopeList ConditionNode::FindSideEffectScope(const bool assign) {
	return cond->GetSideEffectScope(assign);
}

NodePtr ConditionNode::Optimize(OptimizeInfo& info) {
	if (Pointer<AssignNode> assign = cond.Cast<AssignNode>()) {
		NodePtr value = assign->values[0];

		if (NodePtr node = cond->Optimize(info)) {
			if (node.Is<AssignNode>()) {
				cond = node;
			}
			else {
				cond = value;
			}
		}
	}
	else if (NodePtr node = cond->Optimize(info)) cond = node;

	if (cond->IsImmediate()) {
		Pointer<BooleanNode> bn = new BooleanNode(cond->file);
		bn->boolean = cond->GetImmediate() != 0;
		info.optimized = true;
		return bn;
	}

	return nullptr;
}

Mango ConditionNode::ToMango() const {
	return cond->ToMango();
}

StringBuilder ConditionNode::ToMelon(const UInt indent) const {
	return cond->ToMelon(indent);
}