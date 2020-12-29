#include "EnumNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

EnumNode::EnumNode(Symbol* const scope, const FileInfo& file) : Node(scope, file) {

}

EnumNode::~EnumNode() {

}

CompiledNode EnumNode::Compile(CompileInfo& info) {
	return CompiledNode();
}

StringBuilder EnumNode::ToMelon(const UInt indent) const {
	StringBuilder sb = "enum ";
	sb += name.ToString();
	sb += "\n";

	String tabs = String('\t').Repeat(indent + 1);

	for (UInt i = 0; i < values.Size(); i++) {
		sb += tabs;
		sb += values[i].ToString();
		sb += " = ";
		/* TODO: node
		sb += String::ToString(symbol.Get(values[i], file).value);
		*/
		sb += i != values.Size() - 1 ? ",\n" : "\n";
	}

	sb += String('\t').Repeat(indent);
	sb += "end";
	return sb;
}