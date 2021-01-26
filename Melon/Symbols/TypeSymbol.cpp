#include "TypeSymbol.h"

#include "FunctionSymbol.h"

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
