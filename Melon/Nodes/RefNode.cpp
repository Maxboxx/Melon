#include "RefNode.h"

#include "Melon/Parsing/Parser.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

RefNode::RefNode(const Expression& expression) : ExpressionNode(expression->scope, expression->File()) {
	this->expression = expression;
}

RefNode::~RefNode() {

}

TypeSymbol* RefNode::Type() const {
	return expression->Type();
}

Boxx::List<TypeSymbol*> RefNode::Types() const {
	return expression->Types();
}

Symbol* RefNode::Symbol() const {
	return expression->Symbol();
}

CompiledNode RefNode::Compile(CompileInfo& info) {
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

void RefNode::IncludeScan(ParsingInfo& info) {
	expression->IncludeScan(info);
}

ScanResult RefNode::Scan(ScanInfoStack& info) {
	return expression->Scan(info);
}

StringBuilder RefNode::ToMelon(const UInt indent) const {
	return expression->ToMelon(indent);
}