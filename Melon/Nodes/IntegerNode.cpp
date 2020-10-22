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

ScopeList IntegerNode::Type() const {
	if (isUnsigned) {
		if ((ULong)number <= Math::UByteMax()) {
			return ScopeList::UByte;
		}
		else if ((ULong)number <= Math::UShortMax()) {
			return ScopeList::UShort;
		}
		else if ((ULong)number <= Math::UIntMax()) {
			return ScopeList::UInt;
		}
		else if ((ULong)number <= Math::ULongMax()) {
			return ScopeList::ULong;
		}
		else {
			return ScopeList::Huge;
		}
	}
	else {
		if (number >= Math::ByteMin()) {
			return ScopeList::Byte;
		}
		else if (number >= Math::ShortMin()) {
			return ScopeList::Short;
		}
		else if (number >= Math::IntMin()) {
			return ScopeList::Int;
		}
		else if (number >= Math::LongMin()) {
			return ScopeList::Long;
		}
		else {
			return ScopeList::Huge;
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

Mango IntegerNode::ToMango() const {
	return Mango(Type().ToString(), number);
}

StringBuilder IntegerNode::ToMelon(const UInt indent) const {
	return isUnsigned ? String::ToString((ULong)number) : String::ToString(number);
}