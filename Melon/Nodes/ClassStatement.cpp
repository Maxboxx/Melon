#include "ClassStatement.h"

#include "FunctionBody.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/VariableSymbol.h"
#include "Melon/Symbols/TemplateSymbol.h"
#include "Melon/Symbols/FunctionSymbol.h"
#include "Melon/Symbols/NamespaceSymbol.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

ClassStatement::ClassStatement(Symbol* const scope, const FileInfo& file) : Statement(scope, file) {

}

Ptr<Kiwi::Value> ClassStatement::Compile(CompileInfo& info) {
	Ptr<Kiwi::Struct>   struct_ = new Kiwi::Struct(symbol->KiwiName());
	Ptr<Kiwi::Struct>    vtable = new Kiwi::Struct(symbol->AbsoluteName().Add(Name::VTable).ToString());
	Ptr<Kiwi::StaticData> vdata = new Kiwi::StaticData(symbol->AbsoluteName().Add(Name::VData).ToString());

	struct_->AddVariable(Kiwi::Type(1, vtable->name), Name::VTable.name);
	
	CompileSymbol(struct_, vtable, vdata, symbol);

	info.program->AddStruct(struct_);

	//if (!vtable->vars.IsEmpty()) {
		info.program->AddStruct(vtable);
		info.program->AddStatic(vdata);
	//}

	return nullptr;
}

void ClassStatement::CompileSymbol(Weak<Kiwi::Struct> struct_, Weak<Kiwi::Struct> vtable, Weak<Kiwi::StaticData> vdata, ClassSymbol* sym) {
	if (ClassSymbol* baseSym = sym->BaseClass()) {
		CompileSymbol(struct_, vtable, vdata, baseSym);
	}

	for (const Name& name : sym->members) {
		if (VariableSymbol* const var = sym->Find<VariableSymbol>(name, file)) {
			struct_->AddVariable(var->Type()->KiwiType(), var->KiwiName());
		}
	}

	for (const Pair<Name, Symbol*>& inner : sym->symbols) {
		if (FunctionSymbol* const func = inner.value->Cast<FunctionSymbol>()) {
			for (FunctionSymbol* const overload : func->overloads) {
				if (((overload->modifiers & FunctionModifiers::Abstract)) != FunctionModifiers::None) {
					vtable->AddVariable(Kiwi::Type(1, ""), inner.key.ToString());
				}
				else if (((overload->modifiers & FunctionModifiers::Override)) != FunctionModifiers::None) {
					vdata->AddValue(Kiwi::Type(1, ""), inner.key.ToString(), new Kiwi::Variable(overload->KiwiName()));
				}
			}
		}
	}
}

NameList ClassStatement::FindSideEffectScope(const bool assign) {
	if (scope->Is<NamespaceSymbol>()) {
		return NameList(Name::Global);
	}

	return scope->AbsoluteName();
}

ScanResult ClassStatement::Scan(ScanInfoStack& info) {
	symbol->UpdateSize();
	return ScanResult();
}

StringBuilder ClassStatement::ToMelon(const UInt indent) const {
	if (name.name == Name::Optional.name) return "";
	if (symbol->templateParent != nullptr) return "";

	// Check if struct is completely specialized
	for (UInt i = 0; i < symbol->templateArguments.Count(); i++) {
		if (symbol->templateArguments[i].IsTemplate()) continue;

		if (TypeSymbol* const type = symbol->TemplateArgument(i)) {
			if (type->Is<TemplateSymbol>()) return "";
		}
	}

	StringBuilder sb = "class ";

	// Create struct name with template arguments
	if (symbol->templateArguments.IsEmpty()) {
		sb += symbol->Name().ToSimpleString();
	}
	else {
		sb += symbol->Parent()->Name().ToSimpleString();
		sb += "<";

		for (UInt i = 0; i < symbol->templateArguments.Count(); i++) {
			if (i > 0) sb += ", ";
			sb += symbol->templateArguments[i].ToSimpleString();
		}

		sb += ">";
	}

	if (symbol->baseClass) {
		sb += " ext ";
		sb += symbol->baseClass->ToSimpleString();
	}

	String tabs = String('\t').Repeat(indent + 1);

	// Add members
	for (const Name& var : symbol->members) {
		sb += "\n";
		sb += tabs;
		sb += symbol->Find<VariableSymbol>(var, file)->type.ToSimpleString();
		sb += ": ";
		sb += var.ToSimpleString();
	}

	// Add functions
	for (const Pair<Name, Symbol*>& syms : symbol->symbols) {
		if (FunctionSymbol* const func = syms.value->Cast<FunctionSymbol>()) {
			for (FunctionSymbol* const overload : func->overloads) {
				if (overload->node && overload->node->isUsed) {
					sb += "\n\n";
					sb += tabs;
					sb += overload->node->ToMelon(indent + 1);
				}
			}
		}
	}

	sb += "\n";
	sb += String('\t').Repeat(indent);
	sb += "end";

	// Add template variants
	if (!symbol->templateArguments.IsEmpty()) {
		for (TemplateTypeSymbol* const type : symbol->Parent<TemplateTypeSymbol>()->templateVariants) {
			if (type->templateParent == symbol) {
				type->templateParent = nullptr;
				StringBuilder s = type->Cast<ClassSymbol>()->node->ToMelon(indent);
				type->templateParent = symbol;

				if (s.Length() > 0) {
					sb += "\n\n";
					sb += String('\t').Repeat(indent);
					sb += s;
				}
			}
		}
	}

	return sb;
}