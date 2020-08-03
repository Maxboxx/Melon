#include "NewVariableNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;

NewVariableNode::NewVariableNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

NewVariableNode::~NewVariableNode() {

}

ScopeList NewVariableNode::Type() const {
	return types[0];
}

CompiledNode NewVariableNode::Compile(CompileInfo& info) { //TODO: more accurate error lines
	CompiledNode cn;
	cn.size = Symbol::FindInNamespace(Type(), file).size;

	if (attributes[0].Contains(SymbolAttribute::Ref)) {
		info.stack.Push(info.stack.ptrSize);
	}
	else {
		info.stack.Push(Symbol::FindInNamespace(types[0], file).size);
	}

	Symbol::Find(scope, file).Get(names[0], file).stack = info.stack.top;
	cn.argument = Argument(MemoryLocation(info.stack.Offset()));

	for (UInt i = 1; i < names.Size(); i++) {
		if (attributes[i].Contains(SymbolAttribute::Ref)) {
			info.stack.Push(info.stack.ptrSize);
		}
		else {
			if (types.Size() == 1) {
				info.stack.Push(Symbol::FindInNamespace(types[0], file).size);
			}
			else {
				info.stack.Push(Symbol::FindInNamespace(types[i], file).size);
			}
		}

		Symbol::Find(scope, file).Get(names[i], file).stack = info.stack.top;
	}

	return cn;
}

Set<ScanType> NewVariableNode::Scan(ScanInfo& info) const {
	Symbol::FindInNamespace(Type(), file);

	for (const ScopeList& s : types) {
		Symbol::FindInNamespace(s, file);
	}

	for (const Scope& n : names) {
		Symbol::FindInNamespace(scope.Add(n), file);
	}

	return Set<ScanType>();
}

Mango NewVariableNode::ToMango() const {
	Mango mango = Mango("newvar", MangoType::List);

	for (UInt i = 0; i < names.Size(); i++) {
		if (types.Size() == 1)
			mango.Add(Mango(types[0].ToString(), scope.Add(names[i]).ToString()));
		else
			mango.Add(Mango(types[i].ToString(), scope.Add(names[i]).ToString()));
	}

	return mango;
}