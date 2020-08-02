#include "NameNode.h"

#include "PtrNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;

NameNode::NameNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

NameNode::~NameNode() {

}

ScopeList NameNode::Type() const {
	Symbol s = Symbol::FindNearest(scope, name, file);

	if (s.type == SymbolType::Scope) {
		return s.scope;
	}
	else if (s.type != SymbolType::None) {
		const Symbol s2 = Symbol::FindNearest(s.scope, s.varType, file);

		if (s2.type != SymbolType::None) {
			return s2.scope;
		}
	}

	return ScopeList::undefined;
}

Symbol NameNode::GetSymbol() const {
	return Symbol::FindNearest(scope, name, file);
}

CompiledNode NameNode::Compile(CompileInfo& info) {
	CompiledNode cn;
	Symbol s = Symbol::FindNearest(scope, name, file);

	if (!ignoreRef && s.attributes.Contains(SymbolAttribute::Ref)){
		Pointer<NameNode> name = new NameNode(scope, file);
		name->name = this->name;
		name->ignoreRef = true;
		Pointer<PtrNode> ptr = new PtrNode(name);
		return ptr->Compile(info);
	}
	else {
		cn.argument = Argument(MemoryLocation(info.stack.Offset(s.stack)));
	}

	cn.size = Symbol::Find(s.varType, file).size;
	return cn;
}

Set<ScanType> NameNode::Scan(ScanInfo& info) const {
	Set<ScanType> scanSet = Set<ScanType>();

	if (name == Scope::Self) {
		scanSet.Add(ScanType::Self);
	}

	Symbol::FindNearest(scope, name, file);

	return scanSet;
}

Mango NameNode::ToMango() const {
	return Mango(Type().ToString(), Symbol::FindNearest(scope, name, file).scope.ToString());
}