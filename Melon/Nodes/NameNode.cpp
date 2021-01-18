#include "NameNode.h"

#include "PtrNode.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/TemplateSymbol.h"
#include "Melon/Symbols/FunctionSymbol.h"

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
	/* TODO: node
	Symbols s = GetSymbol();

	if (s.type == SymbolType::Scope || s.type == SymbolType::Namespace || s.type == SymbolType::Struct || s.type == SymbolType::Enum) {
		return s.scope;
	}
	else if (s.type != SymbolType::None) {
		const Symbols s2 = s.GetType(file);

		if (s2.type != SymbolType::None) {
			return s2.scope;
		}
	}
	*/

	return nullptr;
}

Symbol* NameNode::GetSymbol() const {
	Scope s = name.Copy();

	if (s.types) {
		for (ScopeList& type : s.types.Get()) {
			TypeSymbol* const sym = SymbolTable::Find<TypeSymbol>(type, scope->AbsoluteName(), file, SymbolTable::SearchOptions::ReplaceTemplates);
			
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

	/* TODO: node
	Symbols s = GetSymbol();

	if (!ignoreRef && s.attributes.Contains(SymbolAttribute::Ref)) {
		Pointer<NameNode> name = new NameNode(scope, file);
		name->name = this->name;
		name->ignoreRef = true;
		Pointer<PtrNode> ptr = new PtrNode(name);
		return ptr->Compile(info);
	}
	else {
		cn.argument = Argument(MemoryLocation(info.stack.Offset(s.stackIndex)));
	}

	cn.size = s.GetType(file).size;
	*/

	return cn;
}

Set<ScanType> NameNode::Scan(ScanInfoStack& info) {
	Set<ScanType> scanSet = Set<ScanType>();

	/* TODO: node
	Symbols s = GetSymbol();

	if (name == Scope::Self) {
		scanSet.Add(ScanType::Self);
	}
	else if (s.type == SymbolType::Variable) {
		info.usedVariables.Add(s.scope);
	}
	*/

	return scanSet;
}

ScopeList NameNode::FindSideEffectScope(const bool assign) {
	if (assign) {
		/* TODO: node
		Symbols s = GetSymbol();

		if (s.IsArgument() && s.attributes.Contains(SymbolAttribute::Ref)) {
			return s.scope.Pop().Pop();
		}
		else {
			return s.scope.Pop();
		}
		*/
	}

	return scope->AbsoluteName();
}

StringBuilder NameNode::ToMelon(const UInt indent) const {
	return name.ToString();
}