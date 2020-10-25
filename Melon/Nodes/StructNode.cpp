#include "StructNode.h"

#include "Melon/Parsing/Parser.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

StructNode::StructNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

StructNode::~StructNode() {

}

CompiledNode StructNode::Compile(CompileInfo& info) {
	return CompiledNode();
}

bool StructNode::IsRecursive(const Symbol& symbol) const {
	if (symbol.type != SymbolType::Struct) return false;
	if (symbol.isRecursive) return false;
	if (symbol.scope == this->symbol.scope) return true;

	for (const Scope& name : symbol.names) {
		if (IsRecursive(symbol.Get(name, file).GetType(file))) {
			return true;
		}
	}

	return false;
}

Set<ScanType> StructNode::Scan(ScanInfoStack& info) {
	Symbol& s = Symbol::Find(symbol.scope.Pop(), file).Get(symbol.scope.Last(), file);
	s.size = 0;

	for (const Scope& var : vars) {
		Symbol& v = Symbol::Find(symbol.scope, file).Get(var, file);
		v.offset = s.size;
		s.size += s.Get(var, file).GetType(file).size;

		// TODO: error line
		if (IsRecursive(v.GetType(file))) {
			s.isRecursive = true;
			ErrorLog::Error(SymbolError(SymbolError::RecursiveStruct(this->name.ToString()), file));
		}
	}

	return Set<ScanType>();
}

Mango StructNode::ToMango() const {
	return Mango();
}

StringBuilder StructNode::ToMelon(const UInt indent) const {
	if (name.name == Scope::Optional.name) return "";

	StringBuilder sb = "struct ";

	sb += name.ToSimpleString();

	String tabs = String('\t').Repeat(indent + 1);

	for (const Scope& var : vars) {
		sb += "\n";
		sb += tabs;
		sb += symbol.Get(var, file).varType.ToSimpleString();
		sb += ": ";
		sb += var.ToSimpleString();
	}

	for (const Pair<String, Symbol>& syms : symbol.scopes) {
		if (syms.value.type == SymbolType::Scope) {
			if (syms.value.Contains(Scope::Call)) {
				for (const Symbol& variant : syms.value.Get(Scope::Call, file).variants) {
					if (variant.node) {
						sb += "\n\n";
						sb += tabs;
						sb += variant.node->ToMelon(indent + 1);
					}
				}
			}

			for (const Symbol& variant : syms.value.variants) {
				if (variant.node) {
					sb += "\n\n";
					sb += tabs;
					sb += variant.node->ToMelon(indent + 1);
				}
			}
		}
	}

	sb += "\n";
	sb += String('\t').Repeat(indent);
	sb += "end";

	return sb;
}