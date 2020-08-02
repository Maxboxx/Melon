#include "StructNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

StructNode::StructNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

StructNode::~StructNode() {

}

CompiledNode StructNode::Compile(CompileInfo& info) {
	const ScopeList globalName = scope.Add(name);

	Symbol& s = Symbol::Find(scope, file).Get(name, file);
	s.varType = globalName;
	s.size = 0;

	for (const Scope& var : vars) {
		Symbol& v = Symbol::Find(globalName, file).Get(var, file);
		v.stack = s.size;
		s.size += Symbol::FindNearest(scope, s.Get(var, file).varType, file).size;
	}

	return CompiledNode();
}

Mango StructNode::ToMango() const {
	return Mango();
}