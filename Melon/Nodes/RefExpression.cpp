#include "RefExpression.h"

#include "Melon/Parsing/Parser.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

RefExpression::RefExpression(Ptr<Expression> expression) : Expression(expression->scope, expression->File()) {
	this->expression = expression;
}

RefExpression::~RefExpression() {

}

TypeSymbol* RefExpression::Type() const {
	return expression->Type();
}

Boxx::List<TypeSymbol*> RefExpression::Types() const {
	return expression->Types();
}

Symbol* RefExpression::Symbol() const {
	return expression->Symbol();
}

CompiledNode RefExpression::Compile(CompileInfo& info) {
	CompiledNode cn = expression->Compile(info);

	Register reg = Register(info.index++);

	Instruction adr = Instruction(InstructionType::Adr, info.stack.ptrSize);
	adr.arguments.Add(reg);
	adr.arguments.Add(cn.argument);

	cn.instructions.Add(adr);

	cn.argument = reg;
	cn.size = info.stack.ptrSize;
	return cn;
}

void RefExpression::IncludeScan(ParsingInfo& info) {
	expression->IncludeScan(info);
}

ScanResult RefExpression::Scan(ScanInfoStack& info) {
	return expression->Scan(info);
}

StringBuilder RefExpression::ToMelon(const UInt indent) const {
	return expression->ToMelon(indent);
}

WeakRefExpression::WeakRefExpression(Weak<Expression> expression) : Expression(expression->scope, expression->File()) {
	this->expression = expression;
}

WeakRefExpression::~WeakRefExpression() {

}

TypeSymbol* WeakRefExpression::Type() const {
	return expression->Type();
}

Boxx::List<TypeSymbol*> WeakRefExpression::Types() const {
	return expression->Types();
}

Symbol* WeakRefExpression::Symbol() const {
	return expression->Symbol();
}

CompiledNode WeakRefExpression::Compile(CompileInfo& info) {
	CompiledNode cn = expression->Compile(info);

	Register reg = Register(info.index++);

	Instruction adr = Instruction(InstructionType::Adr, info.stack.ptrSize);
	adr.arguments.Add(reg);
	adr.arguments.Add(cn.argument);

	cn.instructions.Add(adr);

	cn.argument = reg;
	cn.size = info.stack.ptrSize;
	return cn;
}

void WeakRefExpression::IncludeScan(ParsingInfo& info) {
	expression->IncludeScan(info);
}

ScanResult WeakRefExpression::Scan(ScanInfoStack& info) {
	return expression->Scan(info);
}

StringBuilder WeakRefExpression::ToMelon(const UInt indent) const {
	return expression->ToMelon(indent);
}
