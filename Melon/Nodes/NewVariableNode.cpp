#include "NewVariableNode.h"

#include "Melon/Symbols/NamespaceSymbol.h"
#include "Melon/Symbols/TemplateSymbol.h"

#include "Melon/Parsing/Parser.h"
#include "Melon/Parsing/IncludeParser.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;

NewVariableNode::NewVariableNode(Symbol* const scope, const FileInfo& file) : Node(scope, file) {

}

NewVariableNode::~NewVariableNode() {

}

TypeSymbol* NewVariableNode::GetType(const UInt index) const {
	ScopeList type = types[types.Size() > 1 ? index : 0];

	if (type == ScopeList::Discard) {
		return nullptr;
	}

	TypeSymbol* const sym = SymbolTable::Find<TypeSymbol>(type, scope->AbsoluteName(), file, SymbolTable::SearchOptions::ReplaceTemplates);

	if (TemplateSymbol* const t = sym->Cast<TemplateSymbol>()) {
		return t->Type();
	}
	else {
		return sym;
	}
}

TypeSymbol* NewVariableNode::Type() const {
	return GetType(0);
}

List<ScopeList> NewVariableNode::GetVariables() const {
	List<ScopeList> vars{names.Size()};

	for (const Scope& n : names) {
		if (n == ScopeList::Discard.Last()) {
			vars.Add(ScopeList::Discard);
		}
		else {
			/* TODO: node
			vars.Add(Symbols::FindInNamespace(scope.Add(n), file).scope);
			*/
		}
	}

	return vars;
}

UInt NewVariableNode::GetSize() const {
	UInt size = 0;

	for (UInt i = 0; i < names.Size(); i++) {
		if (names[i] == ScopeList::Discard.Last()) continue;

		if ((attributes[i] & VariableAttributes::Ref) != VariableAttributes::None) {
			size += StackPtr::ptrSize;
		}
		else {
			size += GetType(i)->Size();
		}
	}

	return size;
}

CompiledNode NewVariableNode::Compile(CompileInfo& info) { // TODO: more accurate error lines
	CompiledNode cn;
	/* TODO: node
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
	*/

	return cn;
}

void NewVariableNode::IncludeScan(ParsingInfo& info) {
	for (const ScopeList& type : types) {
		if (type == ScopeList::Discard) continue;

		while (true) {
			Symbol* s = SymbolTable::Find(type, scope->AbsoluteName(), file, SymbolTable::SearchOptions::ReplaceTemplates);

			bool done = true;

			for (UInt i = 1; i < type.Size(); i++) {
				if (s->Is<NamespaceSymbol>()) {
					if (!s->Contains(type[i])) {
						IncludeParser::ParseInclude(s->AbsoluteName().Add(type[i]), info);
						done = false;
						break;
					}
					else {
						s = s->Find(type[i], FileInfo());
					}
				}
			}

			if (done) break;
		}
	}
}

ScanResult NewVariableNode::Scan(ScanInfoStack& info) {
	for (UInt i = 0; i < types.Size(); i++) {
		GetType(i);
	}

	for (UInt i = 0; i < names.Size(); i++) {
		scope->Find(names[i], file);
	}

	return ScanResult();
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

		VariableSymbol* const var = SymbolTable::Find<VariableSymbol>(names[i], scope->AbsoluteName(), file, SymbolTable::SearchOptions::ReplaceTemplates);

		if ((var->attributes & VariableAttributes::Const) != VariableAttributes::None) {
			sb += "const ";
		}

		sb += names[i].ToSimpleString();
	}

	return sb;
}
