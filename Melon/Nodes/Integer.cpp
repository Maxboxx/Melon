#include "Integer.h"

#include "Boxx/Math.h"

#include "Melon/Symbols/IntegerSymbol.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Symbols;

Integer::Integer(const FileInfo& file) : Expression(nullptr, file) {

}

Integer::~Integer() {

}

TypeSymbol* Integer::Type(TypeSymbol* expected) const {
	if (expected->Is<IntegerSymbol>()) return expected;

	// Unsigned integers
	if (isUnsigned) {
		if ((ULong)number <= Math::UByteMax()) {
			return (TypeSymbol*)SymbolTable::UTiny;
		}
		else if ((ULong)number <= Math::UShortMax()) {
			return (TypeSymbol*)SymbolTable::UShort;
		}
		else if ((ULong)number <= Math::UIntMax()) {
			return (TypeSymbol*)SymbolTable::UInt;
		}
		else if ((ULong)number <= Math::ULongMax()) {
			return (TypeSymbol*)SymbolTable::ULong;
		}
		else {
			return SymbolTable::FindAbsolute<TypeSymbol>(NameList::Huge, file);
		}
	}
	// Signed integers
	else {
		if (number >= Math::ByteMin()) {
			return (TypeSymbol*)SymbolTable::Tiny;
		}
		else if (number >= Math::ShortMin()) {
			return (TypeSymbol*)SymbolTable::Short;
		}
		else if (number >= Math::IntMin()) {
			return (TypeSymbol*)SymbolTable::Int;
		}
		else if (number >= Math::LongMin()) {
			return (TypeSymbol*)SymbolTable::Long;
		}
		else {
			return SymbolTable::FindAbsolute<TypeSymbol>(NameList::Huge, file);
		}
	}
}

Ptr<Kiwi::Value> Integer::Compile(CompileInfo& info) {
	return new Kiwi::Integer(Type(info.PeekExpectedType())->KiwiType(), number);
}

StringBuilder Integer::ToMelon(const UInt indent) const {
	return isUnsigned ? String::ToString((ULong)number) : String::ToString(number);
}