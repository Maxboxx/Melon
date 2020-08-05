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
	const ScopeList globalName = scope.Add(name);

	Symbol& s = Symbol::Find(scope, file).Get(name, file);
	s.varType = globalName;
	s.size = 0;

	for (const Scope& var : vars) {
		Symbol& v = Symbol::Find(globalName, file).Get(var, file);
		v.stack = s.size;
		s.size += s.Get(var, file).GetType(file).size;
	}

	return CompiledNode();
}

Set<ScanType> StructNode::Scan(ScanInfo& info) const {
	const ScopeList globalName = scope.Add(name);

	Symbol s = Symbol::Find(globalName, file);

	for (const Scope& var : vars) {
		s.Get(var, file).GetType(file);
	}

	return Set<ScanType>();
}

Mango StructNode::ToMango() const {
	return Mango();
}