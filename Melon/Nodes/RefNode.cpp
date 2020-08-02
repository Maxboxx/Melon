#include "RefNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;

RefNode::RefNode(const NodePtr& node) : Node(node->scope, node->file) {this->node = node;}
RefNode::~RefNode() {}

ScopeList RefNode::Type() const {
	return node->Type();
}

Boxx::List<ScopeList> RefNode::Types() const {
	return node->Types();
}

Symbol RefNode::GetSymbol() const {
	return node->GetSymbol();
}

CompiledNode RefNode::Compile(CompileInfo& info) {
	CompiledNode cn = node->Compile(info);

	Register reg = Register(info.index++);

	Instruction adr = Instruction(InstructionType::Adr, info.stack.ptrSize);
	adr.arguments.Add(reg);
	adr.arguments.Add(cn.argument);

	cn.instructions.Add(adr);

	cn.argument = reg;
	cn.size = info.stack.ptrSize;
	return cn;
}

Set<ScanType> RefNode::Scan(ScanInfo& info) const {
	return node->Scan(info);
}

Boxx::Mango RefNode::ToMango() const {
	return node->ToMango();
}