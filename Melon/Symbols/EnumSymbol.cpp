#include "EnumSymbol.h"

#include "SymbolTable.h"
#include "TypeSymbol.h"
#include "ValueSymbol.h"
#include "StructSymbol.h"
#include "IntegerSymbol.h"

#include "Melon/Nodes/RootNode.h"

#include "Boxx/Math.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

EnumSymbol::EnumSymbol(const FileInfo& file) : TemplateTypeSymbol(file) {
	
}

EnumSymbol::~EnumSymbol() {
	
}

TypeSymbol* EnumSymbol::IdentifierType() const {
	return (TypeSymbol*)SymbolTable::Byte;
}

UInt EnumSymbol::IdentifierSize() const {
	return IdentifierType()->Size();
}

Kiwi::Type EnumSymbol::ValueKiwiType() const {
	Kiwi::Type type = SymbolTable::Byte->KiwiType();
	type.len = ValueSize();
	return type;
}

UInt EnumSymbol::ValueSize() const {
	return Size() - IdentifierSize();
}

bool EnumSymbol::IsPlain() {
	for (const Symbols::Name& member : members) {
		if (ValueSymbol* sym = Find<ValueSymbol>(member, FileInfo())) {
			if (sym->type) return false;
		}
	}

	return true;
}

void EnumSymbol::UpdateSize() {
	size = IdentifierSize();

	UInt valueSize = 0;

	for (const Symbols::Name& name : members) {
		if (ValueSymbol* const value = Contains<ValueSymbol>(name)) {
			if (TypeSymbol* const type = value->ValueType()) {
				if (type->Size() == 0) {
					type->UpdateSize();
				}
				
				valueSize = Math::Max(valueSize, type->Size());
			}
		}
		else if (StructSymbol* const value = Contains<StructSymbol>(name)) {
			if (value->Size() == 0) {
				value->UpdateSize();
			}

			valueSize = Math::Max(valueSize, value->Size());
		}
	}

	size += valueSize;
}

EnumSymbol* EnumSymbol::InitializeSpecialize() {
	EnumSymbol* const sym = new EnumSymbol(file);
	sym->members = members;
	return sym;
}

void EnumSymbol::SpecializeTemplate(Symbol* initSym, const ReplacementMap<TypeSymbol*>& replacement, RootNode* const root) {
	EnumSymbol* sym = initSym->Cast<EnumSymbol>();
	sym->members = members.Copy();

	List<Symbol*> addedSymbols;

	for (const Pair<Symbols::Name, Symbol*>& s : symbols) {
		Symbol* newSym = s.value->InitializeSpecialize();
		sym->AddSymbol(s.key, newSym);
		s.value->SpecializeTemplate(newSym, replacement, root);
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
		TemplateTypeSymbol* newSym = s->InitializeSpecialize();
		sym->AddTemplateVariant(newSym);
		s->SpecializeTemplate(newSym, replacement, root);
	}
}
