#include "NameNode.h"

#include "PtrNode.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/TemplateSymbol.h"
#include "Melon/Symbols/FunctionSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;

NameNode::NameNode(Symbol* const scope, const FileInfo& file) : Node(scope, file) {

}

NameNode::~NameNode() {

}

TypeSymbol* NameNode::Type() const {
	Symbol* const s = GetSymbol();

	if (s == nullptr) return nullptr;

	if (TypeSymbol* const t = s->Cast<TypeSymbol>()) {
		return t;
	}
	else {
		return s->Type();
	}
}

Symbol* NameNode::GetSymbol() const {
	Scope s = name.Copy();

	if (s.types) {
		for (ScopeList& type : *s.types) {
			TypeSymbol* const sym = SymbolTable::Find<TypeSymbol>(type, scope->AbsoluteName(), file, SymbolTable::SearchOptions::ReplaceTemplates);
			if (!sym) continue;

			if (sym->Is<TemplateSymbol>()) {
				type = sym->Type()->AbsoluteName();
			}
			else {
				type = sym->AbsoluteName();
			}
		}
	}

	Scope noTemplateScope = s.Copy();
	noTemplateScope.types = nullptr;

	Symbol* const sym = SymbolTable::Find(noTemplateScope, scope->AbsoluteName(), file, SymbolTable::SearchOptions::ReplaceTemplates);

	if (sym->Is<TemplateSymbol>()) {
		ScopeList type = sym->Type()->AbsoluteName();
		type[type.Size() - 1].types = s.types;
		return SymbolTable::FindAbsolute(type, file);
	}

	if (sym->Is<FunctionSymbol>()) {
		return sym;
	}
	else if (s.types) {
		Scope scope = s.Copy();
		scope.name  = "";
		return sym->Find(scope, file);
	}
	else {
		return sym;
	}
}

CompiledNode NameNode::Compile(CompileInfo& info) {
	CompiledNode cn;

	VariableSymbol* const sym = GetSymbol()->Cast<VariableSymbol>();

	if (!sym) return cn;

	if (!ignoreRef && sym->HasAttribute(VariableAttributes::Ref)) {
		Pointer<NameNode> name = new NameNode(scope, file);
		name->name = this->name;
		name->ignoreRef = true;
		Pointer<PtrNode> ptr = new PtrNode(name);
		return ptr->Compile(info);
	}
	else {
		cn.argument = Argument(MemoryLocation(info.stack.Offset(sym->stackIndex)));
	}

	cn.size = sym->Type()->Size();

	return cn;
}

ScanResult NameNode::Scan(ScanInfoStack& info) {
	ScanResult result;
	Symbol* const s = GetSymbol();

	if (name == Scope::Self) {
		result.selfUsed = true;
	}
	else if (!info->assign) {
		if (VariableSymbol* const var = s->Cast<VariableSymbol>()) {
			info.usedVariables.Add(var);
		}
	}

	return result;
}

ScopeList NameNode::FindSideEffectScope(const bool assign) {
	if (assign) {
		Symbol* const s = GetSymbol();

		if (s->Is<VariableSymbol>() && (s->Cast<VariableSymbol>()->attributes & VariableAttributes::Ref) != VariableAttributes::None) {
			return s->Parent()->Parent()->AbsoluteName();
		}
		else {
			return s->Parent()->AbsoluteName();
		}
	}

	return scope->AbsoluteName();
}

StringBuilder NameNode::ToMelon(const UInt indent) const {
	return name.ToSimpleString();
}