#include "StructNode.h"

#include "FunctionNode.h"

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

bool StructNode::IsRecursive(const Symbols::Symbols& symbol) const {
	if (symbol.type != SymbolType::Struct) return false;
	if (symbol.isRecursive) return false;

	/* TODO: node
	if (symbol.scope == this->symbol.scope) return true;
	
	for (const Scope& name : symbol.names) {
		if (IsRecursive(symbol.Get(name, file).GetType(file))) {
			return true;
		}
	}
	*/

	return false;
}

ScopeList StructNode::FindSideEffectScope(const bool assign) {
	/* TODO: node
	if (Symbols::Find(scope, file).type == SymbolType::Namespace) {
		return ScopeList().Add(Scope::Global);
	}
	*/

	return scope;
}

Set<ScanType> StructNode::Scan(ScanInfoStack& info) {
	/* TODO: node
	Symbols& s = Symbols::Find(symbol.scope.Pop(), file).Get(symbol.scope.Last(), file);
	s.size = 0;

	for (const Scope& var : vars) {
		Symbols& v = Symbols::Find(symbol.scope, file).Get(var, file);
		v.offset = s.size;
		s.size += s.Get(var, file).GetType(file).size;

		// TODO: error line
		if (IsRecursive(v.GetType(file))) {
			s.isRecursive = true;
			ErrorLog::Error(SymbolError(SymbolError::RecursiveStruct(this->name.ToString()), file));
		}
	}
	*/

	return Set<ScanType>();
}

StringBuilder StructNode::ToMelon(const UInt indent) const {
	if (name.name == Scope::Optional.name) return "";

	/* TODO: node
	for (const ScopeList& arg : Symbols::Find(symbol.scope, file).templateArgs) {
		if (Symbols::Find(arg, file).type == SymbolType::Template) {
			return "";
		}
	}
	*/

	StringBuilder sb = "struct ";

	sb += name.ToSimpleString();

	String tabs = String('\t').Repeat(indent + 1);

	for (const Scope& var : vars) {
		sb += "\n";
		sb += tabs;

		/* TODO: node
		sb += symbol.Get(var, file).varType.ToSimpleString();
		*/

		sb += ": ";
		sb += var.ToSimpleString();
	}

	/* TODO: node
	for (const Pair<String, Symbols>& syms : symbol.scopes) {
		if (syms.value.type == SymbolType::Scope) {
			if (syms.value.Contains(Scope::Call)) {
				for (const Symbols& variant : syms.value.Get(Scope::Call, file).variants) {
					if (variant.node && variant.node.Cast<FunctionNode>()->isUsed) {
						sb += "\n\n";
						sb += tabs;
						sb += variant.node->ToMelon(indent + 1);
					}
				}
			}

			for (const Symbols& variant : syms.value.variants) {
				if (variant.node && variant.node.Cast<FunctionNode>()->isUsed) {
					sb += "\n\n";
					sb += tabs;
					sb += variant.node->ToMelon(indent + 1);
				}
			}

			for (const Symbols& t : syms.value.templateVariants) {
				if (t.Contains(Scope::Call)) {
					for (const Symbols& variant : t.Get(Scope::Call, file).variants) {
						if (variant.node && variant.node.Cast<FunctionNode>()->isUsed) {
							sb += "\n\n";
							sb += tabs;
							sb += variant.node->ToMelon(indent + 1);
						}
					}
				}

				for (const Symbols& variant : t.variants) {
					if (variant.node && variant.node.Cast<FunctionNode>()->isUsed) {
						sb += "\n\n";
						sb += tabs;
						sb += variant.node->ToMelon(indent + 1);
					}
				}
			}
		}
	}
	*/

	sb += "\n";
	sb += String('\t').Repeat(indent);
	sb += "end";

	return sb;
}