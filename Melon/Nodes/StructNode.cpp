#include "StructNode.h"

#include "Melon/Parsing/Parser.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

StructNode::StructNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

StructNode::~StructNode() {

}

CompiledNode StructNode::Compile(CompileInfo& info) {
	return CompiledNode();
}

Set<ScanType> StructNode::Scan(ScanInfoStack& info) {
	Symbol& s = Symbol::Find(symbol.scope.Pop(), file).Get(symbol.scope.Last(), file);
	s.size = 0;

	for (const Scope& var : vars) {
		Symbol& v = Symbol::Find(symbol.scope, file).Get(var, file);
		v.stack = s.size;
		s.size += s.Get(var, file).GetType(file).size;
	}

	return Set<ScanType>();
}

Mango StructNode::ToMango() const {
	return Mango();
}