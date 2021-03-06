#include "PtrNode.h"

#include "Melon/Parsing/Parser.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;

PtrNode::PtrNode(const NodePtr& node) : Node(node->scope, node->file) {this->node = node;}
PtrNode::~PtrNode() {}

TypeSymbol* PtrNode::Type() const {
	return node->Type();
}

Boxx::List<TypeSymbol*> PtrNode::Types() const {
	return node->Types();
}

Symbol* PtrNode::GetSymbol() const {
	return node->GetSymbol();
}

CompiledNode PtrNode::Compile(CompileInfo& info) {
	CompiledNode cn = node->Compile(info);

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
	node->IncludeScan(info);
}

ScanResult PtrNode::Scan(ScanInfoStack& info) {
	return node->Scan(info);
}

StringBuilder PtrNode::ToMelon(const UInt indent) const {
	return node->ToMelon(indent);
}