#include "SafeUnwrapExpression.h"

#include "SafeUnwrapChain.h"

#include "Kiwi/Old/Kiwi.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

SafeUnwrapExpression::SafeUnwrapExpression(Symbols::Symbol* const scope, const FileInfo& file) : Expression(scope, file) {

}

SafeUnwrapExpression::~SafeUnwrapExpression() {

}

TypeSymbol* SafeUnwrapExpression::Type() const  {
	return expression->Type()->Find(Name::Value, file)->Type();
}

Symbol* SafeUnwrapExpression::Symbol() const {
	return Type();
}

CompiledNode SafeUnwrapExpression::Compile(OldCompileInfo& info)  {
	CompiledNode cn = expression->Compile(info);

	Instruction jmp = Instruction(SafeUnwrapChain::jumpInstName, 1);
	jmp.arguments.Add(cn.argument);
	jmp.arguments.Add(Argument(0));
	cn.instructions.Add(jmp);

	cn.argument.mem.offset += 1;
	cn.size -= 1;

	return cn;
}

Ptr<Kiwi::Value> SafeUnwrapExpression::Compile(CompileInfo& info)  {
	Ptr<Kiwi::Variable> var = expression->Compile(info).AsPtr<Kiwi::Variable>();
	if (!var) return nullptr;

	info.currentBlock->AddInstruction(new Kiwi::IfInstruction(new Kiwi::SubVariable(var->Copy(), expression->Type()->Find(Name::HasValue, expression->File())->KiwiName()), nullptr, info.optionalChains.Peek()));

	return new Kiwi::SubVariable(var, expression->Type()->Find(Name::Value, expression->File())->KiwiName());
}

void SafeUnwrapExpression::IncludeScan(ParsingInfo& info)  {
	expression->IncludeScan(info);
}

ScanResult SafeUnwrapExpression::Scan(ScanInfoStack& info)  {
	return expression->Scan(info);
}

NameList SafeUnwrapExpression::FindSideEffectScope(const bool assign) {
	return expression->GetSideEffectScope(assign);
}

Ptr<Expression> SafeUnwrapExpression::Optimize(OptimizeInfo& info) {
	Node::Optimize(expression, info);
	return nullptr;
}

StringBuilder SafeUnwrapExpression::ToMelon(const UInt indent) const  {
	StringBuilder sb = expression->ToMelon(indent);
	// TODO: Avoid ??
	sb += "?";
	return sb;
}
