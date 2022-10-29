#include "NewVariableNode.h"

#include "Melon/Symbols/NamespaceSymbol.h"
#include "Melon/Symbols/TemplateSymbol.h"

#include "Melon/Parsing/Parser.h"
#include "Melon/Parsing/IncludeParser.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;

NewVariableNode::NewVariableNode(Symbol* const scope, const FileInfo& file) : Node(scope, file) {

}

NewVariableNode::~NewVariableNode() {

}

TypeSymbol* NewVariableNode::GetType(const UInt index) const {
	NameList type = types[types.Count() > 1 ? index : 0];

	if (type == NameList::Discard) {
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

List<NameList> NewVariableNode::GetVariables() const {
	List<NameList> vars{names.Count()};

	for (const Name& n : names) {
		if (n == NameList::Discard.Last()) {
			vars.Add(NameList::Discard);
		}
		else {
			vars.Add(scope->Find(n, file)->AbsoluteName());
		}
	}

	return vars;
}

UInt NewVariableNode::GetSize() const {
	UInt size = 0;

	for (UInt i = 0; i < names.Count(); i++) {
		if (names[i] == NameList::Discard.Last()) continue;

		if ((modifiers[i] & VariableModifiers::Ref) != VariableModifiers::None) {
			size += StackPtr::ptrSize;
		}
		else {
			size += GetType(i)->Size();
		}
	}

	return size;
}

CompiledNode NewVariableNode::Compile(OldCompileInfo& info) { // TODO: more accurate error lines
	CompiledNode cn;
	cn.size = Type()->Size();

	if (GetType(0) != nullptr) {
		if ((modifiers[0] & VariableModifiers::Ref) != VariableModifiers::None) {
			info.stack.Push(info.stack.ptrSize);
		}
		else {
			info.stack.Push(GetType(0)->Size());
		}

		VariableSymbol* const var = SymbolTable::Find<VariableSymbol>(names[0], scope->AbsoluteName(), file, SymbolTable::SearchOptions::ReplaceTemplates);
		var->stackIndex = info.stack.top;
	}

	cn.argument = Argument(MemoryLocation(info.stack.Offset()));

	for (UInt i = 1; i < names.Count(); i++) {
		if (GetType(i) == nullptr) continue;

		if ((modifiers[i] & VariableModifiers::Ref) != VariableModifiers::None) {
			info.stack.Push(info.stack.ptrSize);
		}
		else {
			info.stack.Push(GetType(i)->Size());
		}

		VariableSymbol* const var = SymbolTable::Find<VariableSymbol>(names[i], scope->AbsoluteName(), file, SymbolTable::SearchOptions::ReplaceTemplates);
		var->stackIndex = info.stack.top;
	}

	return cn;
}

void NewVariableNode::IncludeScan(ParsingInfo& info) {
	for (const NameList& type : types) {
		if (type == NameList::Discard) continue;

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
	for (UInt i = 0; i < types.Count(); i++) {
		GetType(i);
	}

	for (UInt i = 0; i < names.Count(); i++) {
		scope->Find(names[i], file);
	}

	return ScanResult();
}

StringBuilder NewVariableNode::ToMelon(const UInt indent) const {
	StringBuilder sb;

	for (UInt i = 0; i < types.Count(); i++) {
		if (i > 0) sb += ", ";
		sb += types[i].ToSimpleString();
	}

	sb += ": ";

	for (UInt i = 0; i < names.Count(); i++) {
		if (i > 0) sb += ", ";

		VariableSymbol* const var = SymbolTable::Find<VariableSymbol>(names[i], scope->AbsoluteName(), file, SymbolTable::SearchOptions::ReplaceTemplates);

		if ((var->modifiers & VariableModifiers::Const) != VariableModifiers::None) {
			sb += "const ";
		}

		sb += names[i].ToSimpleString();
	}

	return sb;
}
