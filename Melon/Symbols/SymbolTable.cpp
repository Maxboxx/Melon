#include "SymbolTable.h"

#include "TemplateSymbol.h"
#include "TemplateTypeSymbol.h"
#include "FunctionSymbol.h"
#include "NamespaceSymbol.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Symbols;

Pointer<ScopeSymbol> SymbolTable::symbols = new ScopeSymbol(FileInfo());

Symbol* SymbolTable::FindAbsolute(const ScopeList& name, const FileInfo& file) {
	if (name.Size() > 0 && name[0] == Scope::Global) {
		ScopeList list;

		for (UInt i = 1; i < name.Size(); i++) {
			list = list.Add(name[i]);
		}

		return symbols->Symbol::Find(list, file);
	}

	return symbols->Symbol::Find(name, file);
}

Symbol* SymbolTable::ContainsAbsolute(const ScopeList& name) {
	if (name.Size() > 0 && name[0] == Scope::Global) {
		ScopeList list;

		for (UInt i = 1; i < name.Size(); i++) {
			list = list.Add(name[i]);
		}

		return symbols->Symbol::Contains(list);
	}

	return symbols->Symbol::Contains(name);
}

Symbol* SymbolTable::Contains(const ScopeList& name, const ScopeList& scope, const FileInfo& file, const SearchOptions options) {
	ErrorLog::AddMarker();
	Symbol* const sym = Find(name, scope, file, options);
	ErrorLog::Revert();

	return sym;
}

Symbol* SymbolTable::Find(ScopeList name, ScopeList scope, const FileInfo& file, const SearchOptions options) {
	if ((options & SearchOptions::ReplaceTemplates) != SearchOptions::None) {
		scope = ReplaceTemplatesAbsolute(scope, file);
		name  = ReplaceTemplates(name, scope, file);
	}

	if (name.absolute || (name.Size() > 0 && name[0] == Scope::Global)) {
		return FindAbsolute(name, file);
	}

	const bool ignoreOrder = (options & SearchOptions::IgnoreOrder) != SearchOptions::None; 

	if (scope.Size() > 0) {
		ScopeList list = scope;

		while (list.Size() > 0) {
			Symbol* const s = FindInNamespaces(list, file);

			if (Symbol* const sym = s->Contains(name)) {
				if (ignoreOrder || sym->file.statement < file.statement) {
					return sym;
				}
			}

			list = list.Pop();
		}
	}

	return FindInNamespaces(name, file);
}

Symbol* SymbolTable::FindInNamespaces(const ScopeList& name, const FileInfo& file) {
	if (name.absolute || (name.Size() > 0 && name[0] == Scope::Global)) {
		return FindAbsolute(name, file);
	}

	if (file.currentNamespace.Size() > 0) {
		if (Symbol* const sym = ContainsAbsolute(file.currentNamespace.Add(name))) {
			if (FindAbsolute<NamespaceSymbol>(file.currentNamespace.Add(name[0]), file)) {
				return sym;
			}
		}
	}

	Symbol* foundSymbol = nullptr;
	Symbol* namespaceSymbol = FindAbsolute(file.currentNamespace, file);

	while (namespaceSymbol) {
		if (foundSymbol = namespaceSymbol->Contains(name)) {
			break;
		}

		namespaceSymbol = namespaceSymbol->Parent();
	}

	if (!foundSymbol) {
		if (file.currentNamespace.Size() > 0 && file.currentNamespace[0] == name[0]) {
			foundSymbol = ContainsAbsolute(name);
		}

		if (!foundSymbol) {
			if (Symbol* const sym = ContainsAbsolute(name)) {
				if (FindAbsolute<NamespaceSymbol>(ScopeList().Add(name[0]), file)) {
					foundSymbol = sym;
				}
			}
		}
	}

	for (const ScopeList& includedNamespace : file.includedNamespaces) {
		if (Symbol* const s = ContainsAbsolute(includedNamespace.Pop().Add(name))) {
			if (includedNamespace.Last() == name[0] || s->file.fileScope == name[0]) {
				Symbol* ns = s->Parent();

				for (UInt i = 0; i < name.Size(); i++) {
					if (NamespaceSymbol* const sym = ns->Find<NamespaceSymbol>(name[i], file)) {
						ns = sym;
					}
				}

				if (s->file.currentNamespace == ns->AbsoluteName()) {
					if (foundSymbol && foundSymbol->AbsoluteName() != s->AbsoluteName()) {
						ErrorLog::Error(SymbolError(SymbolError::Ambiguous(name.ToString()), file));
					}

					foundSymbol = s;
				}
			}
		}
	}

	if (foundSymbol) return foundSymbol;

	ErrorLog::Error(SymbolError(SymbolError::NotFoundStart + name.ToString() + SymbolError::NotFoundEnd, file));
	return nullptr;
}

ScopeList SymbolTable::ReplaceTemplatesAbsolute(const ScopeList& name, const FileInfo& file) {
	ScopeList list;
	list.absolute = true;

	for (UInt i = 0; i < name.Size(); i++) {
		Scope scope = name[i].Copy();

		if (scope.types) {
			Symbol* const s = FindAbsolute(list.Add(scope), FileInfo());
			List<ScopeList> args;

			if (i < name.Size() - 1) {
				if (TemplateSymbol* const t = s->Contains<TemplateSymbol>(name[i + 1])) {
					if (t->type != t->AbsoluteName()) {
						list = ReplaceTemplatesAbsolute(t->type, file);
					}
					else {
						list = t->type;
					}

					i++;
					continue;
				}
			}

			Optional<List<ScopeList>> templateArgs = nullptr;

			if (TemplateTypeSymbol* const tts = s->Cast<TemplateTypeSymbol>()) {
				templateArgs = tts->templateArguments;
			}
			else if (FunctionSymbol* const fs = s->Cast<FunctionSymbol>()) {
				templateArgs = fs->templateArguments;
			}

			if (templateArgs) {
				for (const ScopeList& arg : templateArgs.Get()) {
					args.Add(ReplaceTemplatesAbsolute(arg, file));
				}

				Scope newScope = scope.Copy();
				newScope.types = args;

				if (Symbol* const sym = ContainsAbsolute(list.Add(newScope))) {
					scope = sym->Name();
				}
				else {
					scope = newScope;
				}
			}
		}

		list = list.Add(scope);
	}

	return list;
}

ScopeList SymbolTable::ReplaceTemplates(const ScopeList& name, const ScopeList& scope, const FileInfo& file) {
	ScopeList typeScope = name;

	for (UInt i = 0; i < typeScope.Size(); i++) {
		if (typeScope[i].types) {
			for (ScopeList& type : typeScope[i].types.Get()) {
				if (Symbol* const sym = Find(type, scope, file)) {
					type = ReplaceTemplatesAbsolute(sym->AbsoluteName(), file);
				}
			}
		}
	}

	return typeScope;
}
