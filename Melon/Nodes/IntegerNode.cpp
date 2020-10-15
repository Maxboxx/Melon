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
			return ScopeList().Add(Scope::UByte);
		}
		else if ((ULong)number <= Math::UShortMax()) {
			return ScopeList().Add(Scope::UShort);
		}
		else if ((ULong)number <= Math::UIntMax()) {
			return ScopeList().Add(Scope::UInt);
		}
		else if ((ULong)number <= Math::ULongMax()) {
			return ScopeList().Add(Scope::ULong);
		}
		else {
			return ScopeList().Add(Scope::Huge);
		}
	}
	else {
		if (number >= Math::ByteMin()) {
			return ScopeList().Add(Scope::Byte);
		}
		else if (number >= Math::ShortMin()) {
			return ScopeList().Add(Scope::Short);
		}
		else if (number >= Math::IntMin()) {
			return ScopeList().Add(Scope::Int);
		}
		else if (number >= Math::LongMin()) {
			return ScopeList().Add(Scope::Long);
		}
		else {
			return ScopeList().Add(Scope::Huge);
		}
	}
}

bool IntegerNode::IsImmediate() const {
	return true;
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