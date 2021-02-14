#include "SafeUnwrapNode.h"

#include "SafeUnwrapEndNode.h"

#include "Kiwi/Kiwi.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

SafeUnwrapNode::SafeUnwrapNode(Symbol* const scope, const FileInfo& file) : Node(scope, file) {

}

SafeUnwrapNode::~SafeUnwrapNode() {

}

TypeSymbol* SafeUnwrapNode::Type() const  {
	return node->Type()->Find(Scope::Value, file)->Type();
}

Symbol* SafeUnwrapNode::GetSymbol() const {
	return Type();
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

ScanResult SafeUnwrapNode::Scan(ScanInfoStack& info)  {
	return node->Scan(info);
}

ScopeList SafeUnwrapNode::FindSideEffectScope(const bool assign) {
	return node->GetSideEffectScope(assign);
}

NodePtr SafeUnwrapNode::Optimize(OptimizeInfo& info) {
	if (NodePtr n = node->Optimize(info)) node = n;

	return nullptr;
}

StringBuilder SafeUnwrapNode::ToMelon(const UInt indent) const  {
	StringBuilder sb = node->ToMelon(indent);
	sb += "?";
	return sb;
}
