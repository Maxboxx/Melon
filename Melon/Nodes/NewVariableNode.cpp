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

ScopeList NewVariableNode::GetType(const UInt index) const {
	ScopeList replacedScope = Symbol::ReplaceTemplates(scope, file);

	Symbol s = Symbol::FindNearestInNamespace(replacedScope, Symbol::ReplaceNearestTemplates(replacedScope, types[types.Size() > 1 ? index : 0], file), file);

	if (s.type == SymbolType::Template) {
		return s.varType;
	}
	else {
		return s.scope;
	}
}

ScopeList NewVariableNode::Type() const {
	return GetType(0);
}

List<ScopeList> NewVariableNode::GetVariables() const {
	List<ScopeList> vars{names.Size()};

	for (const Scope& n : names) {
		vars.Add(Symbol::FindInNamespace(scope.Add(n), file).scope);
	}

	return vars;
}

UInt NewVariableNode::GetSize() const {
	UInt size = 0;

	for (UInt i = 0; i < names.Size(); i++) {
		if (attributes[i].Contains(SymbolAttribute::Ref)) {
			size += StackPtr::ptrSize;
		}
		else {
			size += Symbol::Find(GetType(i), file).size;
		}
	}

	return size;
}

CompiledNode NewVariableNode::Compile(CompileInfo& info) { // TODO: more accurate error lines
	CompiledNode cn;
	cn.size = Symbol::Find(Type(), file).size;

	if (attributes[0].Contains(SymbolAttribute::Ref)) {
		info.stack.Push(info.stack.ptrSize);
	}
	else {
		info.stack.Push(Symbol::Find(GetType(0), file).size);
	}

	Symbol::Find(Symbol::ReplaceTemplates(scope, file), file).Get(names[0], file).stackIndex = info.stack.top;
	cn.argument = Argument(MemoryLocation(info.stack.Offset()));

	for (UInt i = 1; i < names.Size(); i++) {
		if (attributes[i].Contains(SymbolAttribute::Ref)) {
			info.stack.Push(info.stack.ptrSize);
		}
		else {
			info.stack.Push(Symbol::Find(GetType(i), file).size);
		}

		Symbol::Find(Symbol::ReplaceTemplates(scope, file), file).Get(names[i], file).stackIndex = info.stack.top;
	}

	return cn;
}

void NewVariableNode::IncludeScan(ParsingInfo& info) {
	for (const ScopeList& type : types) {
		while (true) {
			ScopeList replacedScope = Symbol::ReplaceTemplates(scope, file);

			Symbol s = Symbol::FindNearestInNamespace(replacedScope, Symbol::ReplaceNearestTemplates(replacedScope, type, file), file);

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
}

Set<ScanType> NewVariableNode::Scan(ScanInfoStack& info) {
	Symbol::Find(Type(), file);

	for (UInt i = 0; i < types.Size(); i++) {
		GetType(i);
	}

	for (const Scope& n : names) {
		Symbol::FindInNamespace(scope.Add(n), file);
	}

	return Set<ScanType>();
}

Mango NewVariableNode::ToMango() const {
	Mango mango = Mango("newvar", MangoType::List);

	for (UInt i = 0; i < names.Size(); i++) {
		mango.Add(Mango(GetType(i).ToString(), scope.Add(names[i]).ToString()));
	}

	return mango;
}

StringBuilder NewVariableNode::ToMelon(const UInt indent) const {
	StringBuilder sb;

	for (UInt i = 0; i < types.Size(); i++) {
		if (i > 0) sb += ", ";
		sb += types[i].ToString();
	}

	sb += ": ";

	for (UInt i = 0; i < names.Size(); i++) {
		if (i > 0) sb += ", ";

		Symbol var = Symbol::FindInNamespace(scope.Add(names[i]), file);

		for (const SymbolAttribute attr : var.attributes) {
			switch (attr) {
				case SymbolAttribute::Const: sb += "const "; break;
				case SymbolAttribute::Copy:  sb += "copy "; break;
			}
		}

		sb += names[i].ToString();
	}

	return sb;
}
