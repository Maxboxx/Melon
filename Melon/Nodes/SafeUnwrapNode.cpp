#include "SafeUnwrapNode.h"

#include "SafeUnwrapEndNode.h"

#include "Kiwi/Kiwi.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

SafeUnwrapNode::SafeUnwrapNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

SafeUnwrapNode::~SafeUnwrapNode() {

}

ScopeList SafeUnwrapNode::Type() const  {
	return Symbol::Find(node->Type(), file).Get(Scope::Value, file).varType;
}

Symbol SafeUnwrapNode::GetSymbol() const  {
	return Symbol::Find(Type(), file);
}

CompiledNode SafeUnwrapNode::Compile(CompileInfo& info)  {
	CompiledNode cn = node->Compile(info);

	Instruction jmp = Instruction(SafeUnwrapEndNode::jumpInstName, 1);
	jmp.arguments.Add(cn.argument);
	jmp.arguments.Add(Argument(0));
	cn.instructions.Add(jmp);

	cn.argument.mem.offset += 1;
	cn.size -= 1;

	return cn;
}

void SafeUnwrapNode::IncludeScan(ParsingInfo& info)  {
	node->IncludeScan(info);
}

Set<ScanType> SafeUnwrapNode::Scan(ScanInfoStack& info)  {
	return node->Scan(info);
}

NodePtr SafeUnwrapNode::Optimize() {
	if (NodePtr n = node->Optimize()) node = n;

	return nullptr;
}

Mango SafeUnwrapNode::ToMango() const  {
	Mango mango = Mango("?", MangoType::List);
	mango.Add(node->ToMango());
	return mango;
}

StringBuilder SafeUnwrapNode::ToMelon(const UInt indent) const  {
	StringBuilder sb = node->ToMelon(indent);
	sb += "?";
	return sb;
}
