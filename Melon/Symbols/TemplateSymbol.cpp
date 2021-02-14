#include "TemplateSymbol.h"

#include "SymbolTable.h"

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
	if (type.Size() > 0) {
		return SymbolTable::ContainsAbsolute<TypeSymbol>(type);
	}
	else {
		return this;
	}
}

TemplateSymbol* TemplateSymbol::SpecializeTemplate(const ReplacementMap<TypeSymbol*>& replacement, RootNode* const root) {
	TemplateSymbol* const sym = new TemplateSymbol(file);
	TypeSymbol* const type = replacement.GetValue(this);

	if (type != this) {
		sym->type = type->AbsoluteName();
	}

	return sym;
}
