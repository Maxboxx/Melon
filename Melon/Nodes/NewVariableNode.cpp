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
	ScopeList replacedScope = Symbols::ReplaceTemplates(scope, file);

	ScopeList type = types[types.Size() > 1 ? index : 0];

	if (type == ScopeList::Discard) {
		return ScopeList::Discard;
	}

	Symbols s = Symbols::FindNearestInNamespace(replacedScope, Symbols::ReplaceNearestTemplates(replacedScope, type, file), file);

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
		if (n == ScopeList::Discard.Last()) {
			vars.Add(ScopeList::Discard);
		}
		else {
			vars.Add(Symbols::FindInNamespace(scope.Add(n), file).scope);
		}
	}

	return vars;
}

UInt NewVariableNode::GetSize() const {
	UInt size = 0;

	for (UInt i = 0; i < names.Size(); i++) {
		if (names[i] == ScopeList::Discard.Last()) continue;

		if (attributes[i].Contains(SymbolAttribute::Ref)) {
			size += StackPtr::ptrSize;
		}
		else {
			size += Symbols::Find(GetType(i), file).size;
		}
	}

	return size;
}

CompiledNode NewVariableNode::Compile(CompileInfo& info) { // TODO: more accurate error lines
	CompiledNode cn;
	cn.size = Symbols::Find(Type(), file).size;

	if (GetType(0) != ScopeList::Discard) {
		if (attributes[0].Contains(SymbolAttribute::Ref)) {
			info.stack.Push(info.stack.ptrSize);
		}
		else {
			info.stack.Push(Symbols::Find(GetType(0), file).size);
		}

		Symbols::Find(Symbols::ReplaceTemplates(scope, file), file).Get(names[0], file).stackIndex = info.stack.top;
	}

	cn.argument = Argument(MemoryLocation(info.stack.Offset()));

	for (UInt i = 1; i < names.Size(); i++) {
		if (GetType(i) == ScopeList::Discard) continue;

		if (attributes[i].Contains(SymbolAttribute::Ref)) {
			info.stack.Push(info.stack.ptrSize);
		}
		else {
			info.stack.Push(Symbols::Find(GetType(i), file).size);
		}

		Symbols::Find(Symbols::ReplaceTemplates(scope, file), file).Get(names[i], file).stackIndex = info.stack.top;
	}

	return cn;
}

void NewVariableNode::IncludeScan(ParsingInfo& info) {
	for (const ScopeList& type : types) {
		if (type == ScopeList::Discard) continue;

		while (true) {
			ScopeList replacedScope = Symbols::ReplaceTemplates(scope, file);

			Symbols s = Symbols::FindNearestInNamespace(replacedScope, Symbols::ReplaceNearestTemplates(replacedScope, type, file), file);

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
	Symbols::Find(Type(), file);

	for (UInt i = 0; i < types.Size(); i++) {
		GetType(i);
	}

	for (UInt i = 0; i < names.Size(); i++) {
		if (GetType(i) == ScopeList::Discard) continue;

		Symbols::FindInNamespace(scope.Add(names[i]), file);
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
		sb += types[i].ToSimpleString();
	}

	sb += ": ";

	for (UInt i = 0; i < names.Size(); i++) {
		if (i > 0) sb += ", ";

		Symbols var = Symbols::FindInNamespace(scope.Add(names[i]), file);

		for (const SymbolAttribute attr : var.attributes) {
			switch (attr) {
				case SymbolAttribute::Const: sb += "const "; break;
				case SymbolAttribute::Copy:  sb += "copy "; break;
			}
		}

		sb += names[i].ToSimpleString();
	}

	return sb;
}
