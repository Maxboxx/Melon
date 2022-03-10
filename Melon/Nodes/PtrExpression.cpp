#include "PtrExpression.h"

#include "Melon/Parsing/Parser.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;

PtrExpression::PtrExpression(Ptr<Expression> address) : Expression(address->scope, address->File()) {
	this->address = address;
}

PtrExpression::~PtrExpression() {

}

TypeSymbol* PtrExpression::Type() const {
	return address->Type();
}

Boxx::List<TypeSymbol*> PtrExpression::Types() const {
	return address->Types();
}

Symbol* PtrExpression::Symbol() const {
	return address->Symbol();
}

CompiledNode PtrExpression::Compile(CompileInfo& info) {
	CompiledNode cn = address->Compile(info);

	Register reg = Register(info.index++);

	Instruction mov = Instruction(InstructionType::Mov, info.stack.ptrSize);
	mov.arguments.Add(reg);
	mov.arguments.Add(cn.argument);

	cn.instructions.Add(mov);

	MemoryLocation sv = MemoryLocation(0);
	sv.memptr = reg;

	cn.argument = Argument(sv);
	return cn;
}

void PtrExpression::IncludeScan(ParsingInfo& info) {
	address->IncludeScan(info);
}

ScanResult PtrExpression::Scan(ScanInfoStack& info) {
	return address->Scan(info);
}

StringBuilder PtrExpression::ToMelon(const UInt indent) const {
	return address->ToMelon(indent);
}