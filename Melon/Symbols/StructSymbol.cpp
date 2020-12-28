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
		sym->templateArguments.Add(replacement.GetValue(TemplateArgument(i))->AbsoluteName());
	}

	for (TemplateTypeSymbol* const s : templateVariants) {
		sym->AddTemplateVariant(s->SpecializeTemplate(replacement, root));
	}

	return sym;
}
