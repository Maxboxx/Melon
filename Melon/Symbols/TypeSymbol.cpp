#include "TypeSymbol.h"

#include "FunctionSymbol.h"
#include "TemplateSymbol.h"
#include "TemplateTypeSymbol.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Symbols;

TypeSymbol::TypeSymbol(const UInt size, const FileInfo& file) : MapSymbol(file) {
	this->size = size;
}

TypeSymbol::~TypeSymbol() {

}

UInt TypeSymbol::Size() const {
	return size;
}

UInt TypeSymbol::MemorySize() const {
	return size;
}

FunctionSymbol* TypeSymbol::ImplicitConversionTo(TypeSymbol* const type) {
	FunctionSymbol* const as = Contains<FunctionSymbol>(Scope::As);

	if (as == nullptr) return nullptr;

	for (FunctionSymbol* const op : as->overloads) {
		if (!op->isExplicit && op->ReturnType(0) == type && op->ArgumentType(0) == this) {
			return op;
		}
	}

	return nullptr;
}

FunctionSymbol* TypeSymbol::ExplicitConversionTo(TypeSymbol* const type) {
	FunctionSymbol* const as = Contains<FunctionSymbol>(Scope::As);

	if (as == nullptr) return nullptr;

	for (FunctionSymbol* const op : as->overloads) {
		if (op->ReturnType(0) == type && op->ArgumentType(0) == this) {
			return op;
		}
	}

	return nullptr;
}


FunctionSymbol* TypeSymbol::ImplicitConversionFrom(TypeSymbol* const type) {
	Scope as = Scope::As;
	as.arguments = List<ScopeList>();
	as.arguments.Get().Add(type->AbsoluteName());

	FunctionSymbol* const op = Contains<FunctionSymbol>(as);

	if (op == nullptr || op->isExplicit) {
		return nullptr;
	}
	else {
		return op;
	}
}

FunctionSymbol* TypeSymbol::ExplicitConversionFrom(TypeSymbol* const type) {
	Scope as = Scope::As;
	as.arguments = List<ScopeList>();
	as.arguments.Get().Add(type->AbsoluteName());

	return Contains<FunctionSymbol>(as);
}

FunctionSymbol* TypeSymbol::FindMethod(const Scope& name, const List<TypeSymbol*>& args, const FileInfo& file) {
	if (FunctionSymbol* const f = Find<FunctionSymbol>(name, file)) {
		return f->FindMethodOverload(args, file);
	}

	return nullptr;
}

FunctionSymbol* TypeSymbol::FindMethod(const Scope& name, const List<TypeSymbol*>& templateArgs, const List<TypeSymbol*>& args, const FileInfo& file) {
	if (FunctionSymbol* const f = Find<FunctionSymbol>(name, file)) {
		return f->FindMethodOverload(templateArgs, args, file);
	}

	return nullptr;
}

FunctionSymbol* TypeSymbol::FindUnaryOperator(const Scope& name, const FileInfo& file) {
	return nullptr;
}

bool TypeSymbol::IsInitialized() {
	return true;
}

void TypeSymbol::PrepareInit() {
	
}

void TypeSymbol::CompleteInit() {

}

Set<Scope> TypeSymbol::UnassignedMembers() {
	return Set<Scope>();
}

void TypeSymbol::UpdateSize() {

}

Map<TemplateSymbol*, TypeSymbol*> TypeSymbol::DeduceTemplates(TypeSymbol* const type) {
	Map<TemplateSymbol*, TypeSymbol*> templateMap;

	if (TemplateSymbol* const templateSym = this->Cast<TemplateSymbol>()) {
		templateMap.Add(templateSym, type);
	}
	else if (TemplateTypeSymbol* const type1 = this->Cast<TemplateTypeSymbol>()) {
		if (TemplateTypeSymbol* const type2 = type->Cast<TemplateTypeSymbol>()) {
			if (type1->CanBeDeduced(type2)) {
				DeduceTemplates(type1, type2, templateMap);
			}
		}
	}

	return templateMap;
}

void TypeSymbol::DeduceTemplates(TemplateTypeSymbol* const type1, TemplateTypeSymbol* const type2, Map<TemplateSymbol*, TypeSymbol*>& templateMap) {
	for (UInt i = 0; i < type1->templateArguments.Size(); i++) {
		TypeSymbol* const arg1 = type1->TemplateArgument(i);
		TypeSymbol* const arg2 = type2->TemplateArgument(i);

		if (TemplateSymbol* const arg = arg1->Cast<TemplateSymbol>()) {
			templateMap.Add(arg, arg2);
		}
		else if (arg1->Is<TemplateTypeSymbol>() && arg2->Is<TemplateTypeSymbol>()) {
			for (const Pair<TemplateSymbol*, TypeSymbol*>& pair : arg1->DeduceTemplates(arg2)) {
				TypeSymbol* t = nullptr;

				if (templateMap.Contains(pair.key, t)) {
					if (t != pair.value) {
						templateMap[pair.key] = nullptr;
					}
				}
				else {
					templateMap.Add(pair);
				}
			}
		}
	}
}
