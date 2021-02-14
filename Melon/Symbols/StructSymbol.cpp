#include "StructSymbol.h"

#include "SymbolTable.h"
#include "TypeSymbol.h"

#include "Melon/Nodes/RootNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

StructSymbol::StructSymbol(const FileInfo& file) : TemplateTypeSymbol(file) {

}

StructSymbol::~StructSymbol() {

}

StructSymbol* StructSymbol::SpecializeTemplate(const ReplacementMap<TypeSymbol*>& replacement, RootNode* const root) {
	StructSymbol* const sym = new StructSymbol(file);
	sym->members = members.Copy();

	for (const Pair<Scope, Symbol*>& s : symbols) {
		sym->AddSymbol(s.key, s.value->SpecializeTemplate(replacement, root));
	}

	for (UInt i = 0; i < templateArguments.Size(); i++) {
		TypeSymbol* const type = TemplateArgument(i);

		if (templateArguments[i].IsTemplate() && type == replacement.GetValue(type)) {
			sym->templateArguments.Add(templateArguments[i]);
		}
		else {
			sym->templateArguments.Add(ReplaceTypeScope(type, replacement, file));
		}
	}

	for (TemplateTypeSymbol* const s : templateVariants) {
		sym->AddTemplateVariant(s->SpecializeTemplate(replacement, root));
	}

	return sym;
}

bool StructSymbol::IsInitialized() {
	for (const Scope& name : members) {
		if (VariableSymbol* const var = Find<VariableSymbol>(name, file)) {
			if (!var->isAssigned) {
				return false;
			}
		}
	}

	return true;
}

void StructSymbol::PrepareInit() {
	for (const Scope& name : members) {
		if (VariableSymbol* const var = Find<VariableSymbol>(name, file)) {
			var->isAssigned = false;
		}
	}
}

void StructSymbol::CompleteInit() {
	for (const Scope& name : members) {
		if (VariableSymbol* const var = Find<VariableSymbol>(name, file)) {
			var->isAssigned = true;
		}
	}
}

Set<Scope> StructSymbol::UnassignedMembers() {
	Set<Scope> vars;

	for (const Scope& member : members) {
		if (VariableSymbol* const var = Find<VariableSymbol>(member, file)) {
			if (!var->isAssigned) {
				vars.Add(member);
			}
		}
	}

	return vars;
}

void StructSymbol::UpdateSize() {
	size = 0;

	for (const Scope& name : members) {
		if (VariableSymbol* const var = Find<VariableSymbol>(name, file)) {
			if (TypeSymbol* const type = var->Type()) {
				if (type->Size() == 0) {
					type->UpdateSize();
				}

				var->stackIndex = size;
				size += type->Size();
			}
		}
	}
}
