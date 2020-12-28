#include "TemplateSymbol.h"

#include "Melon/Nodes/RootNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

TemplateSymbol::TemplateSymbol(const FileInfo& file) : TypeSymbol(0, file) {

}

TemplateSymbol::~TemplateSymbol() {

}

TypeSymbol* TemplateSymbol::Type() {
	return nullptr;
}

TemplateSymbol* TemplateSymbol::SpecializeTemplate(const ReplacementMap<TypeSymbol*>& replacement, RootNode* const root) {
	TemplateSymbol* const sym = new TemplateSymbol(file);
	sym->type = replacement.GetValue(this)->AbsoluteName();
	return sym;
}
