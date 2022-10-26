#include "Integer.h"

#include "Boxx/Math.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Symbols;

Integer::Integer(const FileInfo& file) : Expression(nullptr, file) {

}

Integer::~Integer() {

}

TypeSymbol* Integer::Type() const {
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

bool Integer::IsImmediate() const {
	return true;
}

Long Integer::GetImmediate() const {
	return number;
}

CompiledNode Integer::Compile(OldCompileInfo& info) {
	CompiledNode node;
	node.argument = Argument(number);
	
	// Size of unsigned integers
	if (isUnsigned) {
		if ((ULong)number <= Math::UByteMax()) {
			node.size = 1;
		}
		else if ((ULong)number <= Math::UShortMax()) {
			node.size = 2;
		}
		else if ((ULong)number <= Math::UIntMax()) {
			node.size = 4;
		}
		else if ((ULong)number <= Math::ULongMax()) {
			node.size = 8;
		}
	}
	// Size of signed integers
	else {
		if (number >= Math::ByteMin()) {
			node.size = 1;
		}
		else if (number >= Math::ShortMin()) {
			node.size = 2;
		}
		else if (number >= Math::IntMin()) {
			node.size = 4;
		}
		else if (number >= Math::LongMin()) {
			node.size = 8;
		}
	}

	return node;
}

Ptr<Kiwi::Value> Integer::Compile(CompileInfo& info) {
	return new Kiwi::Integer(number);
}

StringBuilder Integer::ToMelon(const UInt indent) const {
	return isUnsigned ? String::ToString((ULong)number) : String::ToString(number);
}