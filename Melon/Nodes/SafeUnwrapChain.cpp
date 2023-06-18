#include "SafeUnwrapChain.h"

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

SafeUnwrapChain::SafeUnwrapChain(Symbols::Symbol* const scope, const FileInfo& file) : Expression(scope, file) {

}

SafeUnwrapChain::~SafeUnwrapChain() {

}

TypeSymbol* SafeUnwrapChain::Type(TypeSymbol* expected) const  {
	Name scope = Name::Optional;
	scope.types = List<NameList>();
	scope.types->Add(expression->Type()->AbsoluteName());

	return SymbolTable::FindAbsolute<TypeSymbol>(NameList(scope), file);
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
