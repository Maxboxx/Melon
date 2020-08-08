#include "NameNode.h"

#include "PtrNode.h"

#include "Melon/Parsing/Parser.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;

NameNode::NameNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

NameNode::~NameNode() {

}

ScopeList NameNode::Type() const {
	Symbol s = GetSymbol();

	if (s.type == SymbolType::Scope || s.type == SymbolType::Namespace || s.type == SymbolType::Struct) {
		return s.scope;
	}
	else if (s.type != SymbolType::None) {
		const Symbol s2 = s.GetType(file);

		if (s2.type != SymbolType::None) {
			return s2.scope;
		}
	}

	return ScopeList::undefined;
}

Symbol NameNode::GetSymbol() const {
	return Symbol::FindNearestInNamespace(scope, name, file);
}

CompiledNode NameNode::Compile(CompileInfo& info) {
	CompiledNode cn;
	Symbol s = GetSymbol();

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

	cn.size = s.GetType(file).size;
	return cn;
}

Set<ScanType> NameNode::Scan(ScanInfoStack& info) const {
	Set<ScanType> scanSet = Set<ScanType>();

	if (name == Scope::Self) {
		scanSet.Add(ScanType::Self);
	}

	GetSymbol();

	return scanSet;
}

Mango NameNode::ToMango() const {
	return Mango(Type().ToString(), GetSymbol().scope.ToString());
}