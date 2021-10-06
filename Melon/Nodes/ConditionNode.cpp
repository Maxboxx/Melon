#include "ConditionNode.h"

#include "ConvertNode.h"
#include "AssignNode.h"
#include "BooleanNode.h"
#include "ArgumentNode.h"
#include "TypeNode.h"

#include "Melon/Nodes/AssignNode.h"

#include "Melon/Symbols/VariableSymbol.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

ConditionNode::ConditionNode(Symbols::Symbol* const scope, const FileInfo& file) : Node(scope, file) {

}

ConditionNode::~ConditionNode() {
	
}

bool ConditionNode::IsImmediate() const {
	return !assign && cond->IsImmediate();
}

Boxx::Long ConditionNode::GetImmediate() const {
	return !assign ? cond->GetImmediate() : 0;
}

TypeSymbol* ConditionNode::Type() const {
	return assign ? (TypeSymbol*)SymbolTable::Bool : cond->Type();
}

UInt ConditionNode::GetSize() const {
	return assign ? assign->GetSize() : 0;
}

CompiledNode ConditionNode::CompileAssignCondition(CompileInfo& info) {
	TypeSymbol* const type = assign->values[0]->Type();

	// Compile value
	CompiledNode c = assign->values[0]->Compile(info);
	Argument argCopy = c.argument;
	argCopy.mem.offset++;

	// Temporary replaces the value node with argCopy
	Pointer<ArgumentNode> value = new ArgumentNode(argCopy);
	value->type = type->Find<VariableSymbol>(Name::Value, file)->Type()->AbsoluteName();

	Expression tempValue = assign->values[0];
	assign->values[0] = value;

	// Stores the condition result in a register
	OptimizerInstruction mov = Instruction(InstructionType::Mov, 1);
	mov.instruction.arguments.Add(Argument(Register(info.index++)));
	mov.instruction.arguments.Add(c.argument);
	//mov.important = true;
	c.argument = mov.instruction.arguments[0];
	c.instructions.Add(mov);

	// Checks if the condition is false
	Instruction eq = Instruction(InstructionType::Eq, 1);
	eq.arguments.Add(c.argument);
	eq.arguments.Add(Argument(0));

	UInt eqIndex = c.instructions.Size();
	c.instructions.Add(eq);

	// Compile assignment and restore the value node
	c.AddInstructions(assign->Compile(info).instructions);
	assign->values[0] = tempValue;

	// Add a jump to label for skipping assignment
	c.instructions[eqIndex].instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
	c.instructions.Add(Instruction::Label(info.label++));

	return c;
}

CompiledNode ConditionNode::Compile(CompileInfo& info) {
	// Compile assign condition
	if (assign) {
		return CompileAssignCondition(info);
	}
	// Compile regular condition
	else {
		Pointer<ConvertNode> convert = new ConvertNode(scope, file);
		convert->isExplicit = true;
		convert->expression = cond;
		convert->type = NameList::Bool;
		return convert->Compile(info);
	}
}

void ConditionNode::IncludeScan(ParsingInfo& info) {
	cond->IncludeScan(info);
}

ScanResult ConditionNode::Scan(ScanInfoStack& info) {
	// Scan assignment condition
	if (assign) {
		Expression tempValue = assign->values[0];
		
		TypeSymbol* const type = assign->values[0]->Type();

		// Check if the types match
		if (type && type->AbsoluteName()[0].name == Name::Optional.name) {
			assign->values[0] = new TypeNode(type->Find(Name::Value, file)->Type()->AbsoluteName());
		}
		else {
			ErrorLog::Error(LogMessage("error.type.conditional_assign", tempValue->Type()->ToString()), tempValue->File());
		}

		ScanResult result = cond->Scan(info) | tempValue->Scan(info);
		assign->values[0] = tempValue;
		return result;
	}
	// Scan regular condition
	else {
		Pointer<ConvertNode> convert = new ConvertNode(scope, file);
		convert->isExplicit = true;
		convert->expression = cond;
		convert->type = NameList::Bool;
		return convert->Scan(info);
	}
}

NameList ConditionNode::FindSideEffectScope(const bool assign) {
	return cond->GetSideEffectScope(assign);
}

Condition ConditionNode::Optimize(OptimizeInfo& info) {
	// Optimize assignment
	if (assign) {
		Expression value = assign->values[0];

		if (Statement node = assign->Optimize(info)) {
			if (const Pointer<AssignNode>& a = node.Cast<AssignNode>()) {
				assign = a;
			}
			else {
				assign = nullptr;
				cond = value;
			}
		}
	}
	// Optimize regular condition
	else {
		Node::Optimize(cond, info);
	}

	// Replace condition with immediate value
	if (cond->IsImmediate()) {
		Pointer<BooleanNode> bn = new BooleanNode(cond->File());
		bn->boolean = cond->GetImmediate() != 0;
		info.optimized = true;
		return bn;
	}

	return nullptr;
}

StringBuilder ConditionNode::ToMelon(const UInt indent) const {
	return cond->ToMelon(indent);
}
