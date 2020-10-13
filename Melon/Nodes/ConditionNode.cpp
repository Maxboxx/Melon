#include "ConditionNode.h"

#include "ConvertNode.h"
#include "AssignNode.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

ConditionNode::ConditionNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

ConditionNode::~ConditionNode() {

}

ScopeList ConditionNode::Type() const {
	return cond->Type();//ScopeList().Add(Scope::Bool);
}

UInt ConditionNode::GetSize() const {
	return cond.Is<AssignNode>() ? cond->GetSize() : 0;
}

CompiledNode ConditionNode::Compile(CompileInfo& info) {
	if (cond.Is<AssignNode>()) {
		return CompiledNode();
	}
	else {
		Pointer<ConvertNode> convert = new ConvertNode(scope, file);
		convert->isExplicit = true;
		convert->node = cond;
		convert->type = ScopeList().Add(Scope::Bool);
		return convert->Compile(info);
	}
}

void ConditionNode::IncludeScan(ParsingInfo& info) {
	cond->IncludeScan(info);
}

Set<ScanType> ConditionNode::Scan(ScanInfoStack& info) {
	if (cond.Cast<AssignNode>()) {
		ErrorLog::Error(SyntaxError("assignment condition not supported yet", file));
		return Set<ScanType>();
	}
	else {
		Pointer<ConvertNode> convert = new ConvertNode(scope, file);
		convert->isExplicit = true;
		convert->node = cond;
		convert->type = ScopeList().Add(Scope::Bool);
		return convert->Scan(info);
	}
}

Mango ConditionNode::ToMango() const {
	return cond->ToMango();
}

StringBuilder ConditionNode::ToMelon(const UInt indent) const {
	return cond->ToMelon(indent);
}