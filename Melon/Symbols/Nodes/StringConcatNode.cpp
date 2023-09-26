#include "StringConcatNode.h"

#include "Melon/Symbols/IntegerSymbol.h"
#include "Melon/Symbols/StructSymbol.h"
#include "Melon/Symbols/SymbolTable.h"

#include "Melon/Optimizing/OptimizerInstruction.h"

#include "Boxx/Math.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Optimizing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

Ptr<Kiwi::Value> StringConcatNode::Compile(Weak<Expression> operand1, Weak<Expression> operand2, CompileInfo& info, bool includeType) const {
	Ptr<Kiwi::Variable> value1 = operand1->Compile(info).AsPtr<Kiwi::Variable>();
	Ptr<Kiwi::Variable> value2 = operand2->Compile(info).AsPtr<Kiwi::Variable>();

	Ptr<Kiwi::Variable> var = new Kiwi::Variable(info.NewRegister());
	info.AddInstruction(new Kiwi::AssignInstruction(SymbolTable::String->KiwiType(), var->Copy(), nullptr));

	info.AddInstruction(new Kiwi::AssignInstruction(
		new Kiwi::SubVariable(var->Copy(), Name::Length.name),
		new Kiwi::AddExpression(
			new Kiwi::SubVariable(value1->Copy(), Name::Length.name),
			new Kiwi::SubVariable(value2->Copy(), Name::Length.name)
		)
	));

	info.AddInstruction(new Kiwi::AssignInstruction(
		new Kiwi::SubVariable(var->Copy(), Name::Items.name),
		new Kiwi::AllocExpression(new Kiwi::SubVariable(var->Copy(), Name::Length.name))
	));

	info.AddInstruction(new Kiwi::CopyInstruction(
		new Kiwi::SubVariable(var->Copy(), Name::Items.name),
		new Kiwi::SubVariable(value1->Copy(), Name::Items.name),
		new Kiwi::SubVariable(value1->Copy(), Name::Length.name)
	));

	Ptr<Kiwi::Variable> offset = new Kiwi::Variable(info.NewRegister());

	info.AddInstruction(new Kiwi::AssignInstruction(
		SymbolTable::String->Find(Name::Items, FileInfo())->KiwiType(),
		offset->Copy(),
		new Kiwi::AddExpression(
			new Kiwi::SubVariable(var->Copy(), Name::Items.name),
			new Kiwi::SubVariable(value1->Copy(), Name::Length.name)
		)
	));

	info.AddInstruction(new Kiwi::CopyInstruction(
		offset,
		new Kiwi::SubVariable(value2->Copy(), Name::Items.name),
		new Kiwi::SubVariable(value2->Copy(), Name::Length.name)
	));
	
	return var;
}
