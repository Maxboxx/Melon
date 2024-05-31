#include "IsExpression.h"

#include "Melon/Symbols/EnumSymbol.h"
#include "Melon/Symbols/ValueSymbol.h"
#include "Melon/Symbols/StructSymbol.h"
#include "Melon/Symbols/IntegerSymbol.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

IsExpression::IsExpression(Symbols::Symbol* const scope, const FileInfo& file) : Expression(scope, file) {

}

IsExpression::~IsExpression() {
	
}

TypeSymbol* IsExpression::Type(Symbols::TypeSymbol* expected) const {
	return (Symbols::TypeSymbol*)SymbolTable::Bool;
}

Symbol* IsExpression::TypeSymbol() const {
	return SymbolTable::Find(type, scope ? scope->AbsoluteName() : NameList(true), file, SymbolTable::SearchOptions::ReplaceTemplates);
}

void IsExpression::IncludeScan(Parsing::ParsingInfo& info) {
	expression->IncludeScan(info);
}

ScanResult IsExpression::Scan(ScanInfoStack& info) {
	ScanResult result = expression->Scan(info);

	Symbols::Symbol* sym = TypeSymbol();
	Symbols::TypeSymbol* type = expression->Type();

	if (type->Is<EnumSymbol>() && type == sym->ParentType() && (sym->Is<ValueSymbol>() || sym->Is<StructSymbol>())) {
		
	}
	else {
		ErrorLog::Error(LogMessage("invalid is type"), File());
	}

	return result;
}

Ptr<Kiwi::Value> IsExpression::Compile(CompileInfo& info) {
	Symbols::Symbol* sym = TypeSymbol();
	Symbols::TypeSymbol* type = expression->Type();

	if (type->Is<EnumSymbol>() && type == sym->ParentType()) {
		EnumSymbol* enumSym = type->Cast<EnumSymbol>();
		Long value;

		if (ValueSymbol* const valueSym = sym->Cast<ValueSymbol>()) {
			value = valueSym->value;
		}
		else if (StructSymbol* const structSym = sym->Cast<StructSymbol>()) {
			value = structSym->value;
		}

		Ptr<Kiwi::Variable> var = expression->Compile(info).AsPtr<Kiwi::Variable>();
		Ptr<Kiwi::Variable> res = new Kiwi::Variable(info.NewRegister());

		info.AddInstruction(new Kiwi::AssignInstruction(
			SymbolTable::Bool->KiwiType(), res->Copy(),
				new Kiwi::EqualExpression(
				new Kiwi::SubVariable(var->Copy(), Name::Value.name),
				new Kiwi::Integer(enumSym->IdentifierType()->KiwiType(), value)
			) 
		));

		return res;
	}

	return nullptr;
}

StringBuilder IsExpression::ToMelon(const UInt indent) const {
	StringBuilder str = expression->ToMelon(indent);
	str += " is ";
	str += type.ToSimpleString();
	return str;
}
