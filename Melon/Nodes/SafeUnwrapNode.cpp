#include "SafeUnwrapNode.h"

#include "SafeUnwrapEndNode.h"

#include "Kiwi/Kiwi.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

SafeUnwrapNode::SafeUnwrapNode(Symbols::Symbol* const scope, const FileInfo& file) : ExpressionNode(scope, file) {

}

SafeUnwrapNode::~SafeUnwrapNode() {

}

TypeSymbol* SafeUnwrapNode::Type() const  {
	return expression->Type()->Find(Name::Value, file)->Type();
}

Symbol* SafeUnwrapNode::Symbol() const {
	return Type();
}

CompiledNode SafeUnwrapNode::Compile(CompileInfo& info)  {
	CompiledNode cn = expression->Compile(info);

	Instruction jmp = Instruction(SafeUnwrapEndNode::jumpInstName, 1);
	jmp.arguments.Add(cn.argument);
	jmp.arguments.Add(Argument(0));
	cn.instructions.Add(jmp);

	cn.argument.mem.offset += 1;
	cn.size -= 1;

	return cn;
}

void SafeUnwrapNode::IncludeScan(ParsingInfo& info)  {
	expression->IncludeScan(info);
}

ScanResult SafeUnwrapNode::Scan(ScanInfoStack& info)  {
	return expression->Scan(info);
}

NameList SafeUnwrapNode::FindSideEffectScope(const bool assign) {
	return expression->GetSideEffectScope(assign);
}

Expression SafeUnwrapNode::Optimize(OptimizeInfo& info) {
	Node::Optimize(expression, info);
	return nullptr;
}

StringBuilder SafeUnwrapNode::ToMelon(const UInt indent) const  {
	StringBuilder sb = expression->ToMelon(indent);
	// TODO: Avoid ??
	sb += "?";
	return sb;
}
