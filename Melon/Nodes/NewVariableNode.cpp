#include "NewVariableNode.h"

#include "Melon/Parsing/Parser.h"
#include "Melon/Parsing/IncludeParser.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;

NewVariableNode::NewVariableNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

NewVariableNode::~NewVariableNode() {

}

ScopeList NewVariableNode::Type() const {
	return Symbol::FindInNamespace(types[0], file).scope;
}

List<ScopeList> NewVariableNode::GetVariables() const {
	List<ScopeList> vars{names.Size()};

	for (const Scope& n : names) {
		vars.Add(Symbol::FindInNamespace(scope.Add(n), file).scope);
	}

	return vars;
}

CompiledNode NewVariableNode::Compile(CompileInfo& info) { //TODO: more accurate error lines
	CompiledNode cn;
	cn.size = Symbol::Find(Type(), file).size;

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

void NewVariableNode::IncludeScan(ParsingInfo& info) {
	if (includeScanned) return;

	for (const ScopeList& type : types) {
		while (true) {
			Symbol s = Symbol::FindInNamespace(ScopeList().Add(type[0]), file);

			bool done = true;

			for (UInt i = 1; i < type.Size(); i++) {
				if (s.type == SymbolType::Namespace) {
					if (!s.Contains(type[i])) {
						IncludeParser::ParseInclude(s.scope.Add(type[i]), info);
						done = false;
						break;
					}
					else {
						s = s.Get(type[i], FileInfo());
					}
				}
			}

			if (done) break;
		}
	}

	includeScanned = true;
}

Set<ScanType> NewVariableNode::Scan(ScanInfo& info) const {
	Symbol::Find(Type(), file);

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
			mango.Add(Mango(Symbol::FindInNamespace(types[0], file).scope.ToString(), scope.Add(names[i]).ToString()));
		else
			mango.Add(Mango(Symbol::FindInNamespace(types[i], file).scope.ToString(), scope.Add(names[i]).ToString()));
	}

	return mango;
}