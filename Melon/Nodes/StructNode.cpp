#include "StructNode.h"

#include "FunctionNode.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/VariableSymbol.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

StructNode::StructNode(Symbol* const scope, const FileInfo& file) : Node(scope, file) {

}

StructNode::~StructNode() {

}

CompiledNode StructNode::Compile(CompileInfo& info) {
	return CompiledNode();
}

bool StructNode::IsRecursive(StructSymbol* const symbol) const {
	if (symbol == nullptr) return false;
	if (symbol->isRecursive) return false;
	if (symbol == this->symbol) return true;
	
	for (const Scope& name : symbol->members) {
		if (VariableSymbol* const var = symbol->Find<VariableSymbol>(name, file)) {
			if (TypeSymbol* const type = var->Type()) {
				if (StructSymbol* const s = type->Cast<StructSymbol>()) {
					if (IsRecursive(s)) {
						return true;
					}
				}
			}
		}
	}

	return false;
}

ScopeList StructNode::FindSideEffectScope(const bool assign) {
	/* TODO: node
	if (Symbols::Find(scope, file).type == SymbolType::Namespace) {
		return ScopeList().Add(Scope::Global);
	}
	*/

	return scope->AbsoluteName();
}

ScanResult StructNode::Scan(ScanInfoStack& info) {
	symbol->UpdateSize();

	for (const Scope& var : symbol->members) {
		if (VariableSymbol* const member = symbol->Find<VariableSymbol>(var, file)) {
			if (TypeSymbol* const type = member->Type()) {
				if (IsRecursive(type->Cast<StructSymbol>())) {
					symbol->isRecursive = true;
					ErrorLog::Error(SymbolError(SymbolError::RecursiveStruct(this->name.ToString()), member->File()));
				}
			}
		}
	}

	return ScanResult();
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