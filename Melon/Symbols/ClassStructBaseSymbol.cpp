#include "ClassStructBaseSymbol.h"

#include "SymbolTable.h"
#include "TypeSymbol.h"
#include "VariableSymbol.h"

#include "Melon/Nodes/RootNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

ClassStructBaseSymbol::ClassStructBaseSymbol(const FileInfo& file) : TemplateTypeSymbol(file) {

}

ClassStructBaseSymbol::~ClassStructBaseSymbol() {

}

ClassStructBaseSymbol* ClassStructBaseSymbol::SpecializeTemplate(const ReplacementMap<TypeSymbol*>& replacement, RootNode* const root, ClassStructBaseSymbol* sym) {
	sym->members = members.Copy();

	for (const Pair<Symbols::Name, Symbol*>& s : symbols) {
		sym->AddSymbol(s.key, s.value->SpecializeTemplate(replacement, root));
	}

	for (UInt i = 0; i < templateArguments.Count(); i++) {
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

bool ClassStructBaseSymbol::IsInitialized() {
	for (const Symbols::Name& name : members) {
		if (VariableSymbol* const var = Find<VariableSymbol>(name, file)) {
			if (!var->isAssigned) {
				return false;
			}
		}
	}

	return true;
}

void ClassStructBaseSymbol::PrepareInit() {
	for (const Symbols::Name& name : members) {
		if (VariableSymbol* const var = Find<VariableSymbol>(name, file)) {
			var->isAssigned = false;
		}
	}
}

void ClassStructBaseSymbol::CompleteInit() {
	for (const Symbols::Name& name : members) {
		if (VariableSymbol* const var = Find<VariableSymbol>(name, file)) {
			var->isAssigned = true;
		}
	}
}

Set<Symbols::Name> ClassStructBaseSymbol::UnassignedMembers() {
	Set<Symbols::Name> vars;

	for (const Symbols::Name& member : members) {
		if (VariableSymbol* const var = Find<VariableSymbol>(member, file)) {
			if (!var->isAssigned) {
				vars.Add(member);
			}
		}
	}

	return vars;
}

void ClassStructBaseSymbol::UpdateSize() {
	size = 0;

	for (const Symbols::Name& name : members) {
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
