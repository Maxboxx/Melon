#include "PtrNode.h"

#include "Melon/Parsing/Parser.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;

PtrNode::PtrNode(const NodePtr& node) : Node(node->scope, node->file) {this->node = node;}
PtrNode::~PtrNode() {}

ScopeList PtrNode::Type() const {
	return node->Type();
}

Boxx::List<ScopeList> PtrNode::Types() const {
	return node->Types();
}

Symbol PtrNode::GetSymbol() const {
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
	sv.reg = reg;

	cn.argument = Argument(sv);
	return cn;
}

void PtrNode::IncludeScan(ParsingInfo& info) {
	
}

Set<ScanType> PtrNode::Scan(ScanInfo& info) const {
	return node->Scan(info);
}

Boxx::Mango PtrNode::ToMango() const {
	return node->ToMango();
}