#include "PtrNode.h"

#include "Melon/Parsing/Parser.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;

PtrNode::PtrNode(const _Expression_& address) : Expression(address->scope, address->File()) {
	this->address = address;
}

PtrNode::~PtrNode() {

}

TypeSymbol* PtrNode::Type() const {
	return address->Type();
}

Boxx::List<TypeSymbol*> PtrNode::Types() const {
	return address->Types();
}

Symbol* PtrNode::Symbol() const {
	return address->Symbol();
}

CompiledNode PtrNode::Compile(CompileInfo& info) {
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

void PtrNode::IncludeScan(ParsingInfo& info) {
	address->IncludeScan(info);
}

ScanResult PtrNode::Scan(ScanInfoStack& info) {
	return address->Scan(info);
}

StringBuilder PtrNode::ToMelon(const UInt indent) const {
	return address->ToMelon(indent);
}