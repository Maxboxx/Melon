#include "SafeUnwrapChain.h"

#include "KiwiMemoryExpression.h"
#include "RootNode.h"
#include "KiwiVariable.h"
#include "NilValue.h"

#include "Kiwi/Old/Kiwi.h"

#include "Melon/Optimizing/OptimizerInstruction.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

String SafeUnwrapChain::jumpInstName = "?jmp";

SafeUnwrapChain::SafeUnwrapChain(Symbols::Symbol* const scope, const FileInfo& file) : Expression(scope, file) {

}

SafeUnwrapChain::~SafeUnwrapChain() {

}

TypeSymbol* SafeUnwrapChain::Type() const  {
	Name scope = Name::Optional;
	scope.types = List<NameList>();
	scope.types->Add(expression->Type()->AbsoluteName());

	return SymbolTable::FindAbsolute<TypeSymbol>(NameList(scope), file);
}

CompiledNode SafeUnwrapChain::Compile(OldCompileInfo& info)  {
	// Compile node
	CompiledNode cn = expression->Compile(info);

	info.stack.PushExpr(cn.size + 1, cn);
	Argument arg = Argument(MemoryLocation(info.stack.Offset()));

	// Set has value of optional result to 1
	Instruction mov1 = Instruction(InstructionType::Mov, 1);
	mov1.arguments.Add(arg);
	mov1.arguments.Add(Argument(1));
	cn.instructions.Add(mov1);

	// Get memory location for result optional value
	Ptr<KiwiMemoryExpression> sn1 = new KiwiMemoryExpression(arg.mem.offset + 1, expression->Type()->AbsoluteName());

	// Get memory location of compiled value
	Ptr<KiwiMemoryExpression> sn2 = new KiwiMemoryExpression(cn.argument.mem, expression->Type()->AbsoluteName());

	// Compile assignment to resulting optional
	cn.AddInstructions(CompileAssignment(sn1, sn2, info, file).instructions);

	// Add jump
	UInt jmpIndex = cn.instructions.Count();
	cn.instructions.Add(Instruction(InstructionType::Jmp));

	// Convert custom instructions to eq instructions with jump
	for (OptimizerInstruction& inst : cn.instructions) {
		if (inst.instruction.type == InstructionType::Custom) {
			if (inst.instruction.instructionName == jumpInstName) {
				inst.instruction.type = InstructionType::Eq;
				inst.instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
			}
		}
	}

	// Add label for eq jumps
	cn.instructions.Add(Instruction::Label(info.label++));

	// Set has value of optional result to 0 
	Instruction mov2 = Instruction(InstructionType::Mov, 1);
	mov2.arguments.Add(arg);
	mov2.arguments.Add(Argument(0));
	cn.instructions.Add(mov2);

	// Add end label and jump
	cn.instructions[jmpIndex].instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
	cn.instructions.Add(Instruction::Label(info.label++));

	cn.size++;
	cn.argument = arg;
	info.stack.Pop(cn.size);

	return cn;
}

Ptr<Kiwi::Value> SafeUnwrapChain::Compile(CompileInfo& info)  {
	info.optionalChains.Push(info.NewLabel());

	Ptr<Kiwi::Variable> var = new Kiwi::Variable(info.NewRegister());
	info.AddInstruction(new Kiwi::AssignInstruction(Type()->KiwiType(), var->Copy(), nullptr));

	Ptr<KiwiVariable> kiwiVar = new KiwiVariable(var->Copy(), Type()->AbsoluteName());
	CompileAssignment(kiwiVar, expression, info, expression->File(), false);

	const String lbl = info.NewLabel();
	info.AddInstruction(new Kiwi::GotoInstruction(lbl));

	info.NewInstructionBlock(info.optionalChains.Pop());
	Ptr<NilValue> nil = new NilValue(File());
	CompileAssignment(kiwiVar, nil, info, expression->File(), false);

	info.NewInstructionBlock(lbl);
	return var;
}

void SafeUnwrapChain::IncludeScan(ParsingInfo& info)  {
	expression->IncludeScan(info);

	const NameList nodeType = expression->Type()->AbsoluteName();

	if (nodeType == NameList::undefined) {
		throw IncludeScanError();
	}

	Name type = Name::Optional;
	type.types = List<NameList>();
	type.types->Add(nodeType);

	Root()->AddTemplateSpecialization(NameList(true, type), scope->AbsoluteName(), file);
}

ScanResult SafeUnwrapChain::Scan(ScanInfoStack& info)  {
	return expression->Scan(info);
}

NameList SafeUnwrapChain::FindSideEffectScope(const bool assign) {
	return expression->GetSideEffectScope(assign);
}

Ptr<Expression> SafeUnwrapChain::Optimize(OptimizeInfo& info) {
	Node::Optimize(expression, info);
	return nullptr;
}

StringBuilder SafeUnwrapChain::ToMelon(const UInt indent) const  {
	return StringBuilder();
}
