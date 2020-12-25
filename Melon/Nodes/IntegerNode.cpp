#include "IntegerNode.h"

#include "Boxx/Math.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;

IntegerNode::IntegerNode(const FileInfo& file) : Node(ScopeList(), file) {

}

IntegerNode::~IntegerNode() {

}

Symbol* IntegerNode::Type() const {
	if (isUnsigned) {
		if ((ULong)number <= Math::UByteMax()) {
			return SymbolTable::FindAbsolute(ScopeList::UByte, file);
		}
		else if ((ULong)number <= Math::UShortMax()) {
			return SymbolTable::FindAbsolute(ScopeList::UShort, file);
		}
		else if ((ULong)number <= Math::UIntMax()) {
			return SymbolTable::FindAbsolute(ScopeList::UInt, file);
		}
		else if ((ULong)number <= Math::ULongMax()) {
			return SymbolTable::FindAbsolute(ScopeList::ULong, file);
		}
		else {
			return SymbolTable::FindAbsolute(ScopeList::Huge, file);
		}
	}
	else {
		if (number >= Math::ByteMin()) {
			return SymbolTable::FindAbsolute(ScopeList::Byte, file);
		}
		else if (number >= Math::ShortMin()) {
			return SymbolTable::FindAbsolute(ScopeList::Short, file);
		}
		else if (number >= Math::IntMin()) {
			return SymbolTable::FindAbsolute(ScopeList::Int, file);
		}
		else if (number >= Math::LongMin()) {
			return SymbolTable::FindAbsolute(ScopeList::Long, file);
		}
		else {
			return SymbolTable::FindAbsolute(ScopeList::Huge, file);
		}
	}
}

bool IntegerNode::IsImmediate() const {
	return true;
}

Long IntegerNode::GetImmediate() const {
	return number;
}

CompiledNode IntegerNode::Compile(CompileInfo& info) {
	CompiledNode node;
	node.argument = Argument(number);
	
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

StringBuilder IntegerNode::ToMelon(const UInt indent) const {
	return isUnsigned ? String::ToString((ULong)number) : String::ToString(number);
}