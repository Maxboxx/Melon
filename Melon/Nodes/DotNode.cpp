#include "DotNode.h"

#include "NameNode.h"

#include "Melon/Parsing/Parser.h"
#include "Melon/Parsing/IncludeParser.h"

#include "Melon/Symbols/ValueSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"
#include "Melon/Symbols/TemplateSymbol.h"
#include "Melon/Symbols/StructSymbol.h"
#include "Melon/Symbols/NamespaceSymbol.h"
#include "Melon/Symbols/FunctionSymbol.h"

#include "Kiwi/Kiwi.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

DotNode::DotNode(Symbol* const scope, const FileInfo& file) : Node(scope, file) {

}

DotNode::~DotNode() {

}

TypeSymbol* DotNode::Type() const {
	Symbol* const s = GetSymbol();

	if (s == nullptr) return nullptr;

	if (s->Is<ValueSymbol>() || s->Is<VariableSymbol>() || s->Is<TemplateSymbol>()) {
		return s->Type();
	}
	else if (TypeSymbol* const t = s->Cast<TypeSymbol>()) {
		return t;
	}

	return nullptr;
}

Symbol* DotNode::GetSymbol() const {
	Symbol* const nodeSym = node->GetSymbol();

	if (nodeSym == nullptr) {
		return nullptr;
	}

	if (nodeSym->Is<VariableSymbol>()) {
		TypeSymbol* const type = nodeSym->Type();

		if (StructSymbol* const s = type->Cast<StructSymbol>()) {
			return s->Find(name, file);
		}
	}
	else {
		if (name.types) {
			if (Symbol* const s = nodeSym->Contains(Scope(name.name))) {
				if (s->Is<FunctionSymbol>()) {
					return s;
				}
			}

			if (!nodeSym->Contains(name)) {
				Node::root->AddTemplateSpecialization(nodeSym->AbsoluteName().Add(name), scope->AbsoluteName(), file);
			}
		}

		return nodeSym->Find(name, file);
	}

	return nullptr;
}

CompiledNode DotNode::Compile(CompileInfo& info) {
	Symbol* const type = node->Type();

	/* TODO: node
	Symbols s = Symbols::Find(type, file);
	CompiledNode c = node->Compile(info);

	if (s.type == SymbolType::Struct || s.type == SymbolType::Type) {
		Symbols var = s.Get(name, file);
		Symbols varType = var.GetType(file);

		if (var.attributes.Contains(SymbolAttribute::Static)) {
			c.argument = MemoryLocation(0);
			c.argument.mem.memptr = var.scope.ToString();
		}

		c.argument.mem.offset += var.offset;
		c.size = varType.size;
	}
	else if (s.type == SymbolType::Enum) {
		c.argument = Argument(s.Get(name, file).stackIndex);
		c.size = s.size;
	}

	return c;
	*/

	return CompiledNode();
}

void DotNode::IncludeScan(ParsingInfo& info) {
	node->IncludeScan(info);
	
	Symbol* const s = node->GetSymbol();

	if (!name.types) {
		if (s->Is<NamespaceSymbol>() && !s->Contains(name)) {
			IncludeParser::ParseInclude(s->AbsoluteName().Add(name), info);
		}
	}
}

ScanResult DotNode::Scan(ScanInfoStack& info) {
	const bool assign = info.Assign();
	info.Assign(false);

	ScanResult result = node->Scan(info);

	TypeSymbol* const type = node->Type();
	if (type == nullptr) return result;

	Symbol* const sym = type->Find(name, file);
	if (sym == nullptr) return result;

	if (VariableSymbol* const var = sym->Cast<VariableSymbol>()) {
		if ((var->attributes & VariableAttributes::Static) != VariableAttributes::None) {
			info.usedVariables.Add(var);
		}
	}

	info.Assign(assign);

	if (info.Assign()) {
		if (const Pointer<NameNode>& nn = node.Cast<NameNode>()) {
			if (nn->name == Scope::Self) {
				if (VariableSymbol* const var = sym->Cast<VariableSymbol>()) {
					if (info.ScopeInfo().WillContinue()) {
						var->isAssigned = true;
					}

					if (info.Type()->IsInitialized()) {
						info.Init(false);
					}
				}

				result.selfUsed = false;
			}
		}
	}
	else if (info.Init()) {
		if (const Pointer<NameNode>& nn = node.Cast<NameNode>()) {
			if (nn->name == Scope::Self) {
				if (VariableSymbol* const var = sym->Cast<VariableSymbol>()) {
					if (!var->isAssigned) {
						ErrorLog::Error(CompileError(CompileError::SelfVarUseStart + name.ToString() + CompileError::SelfVarUseEnd, file));
					}
				}

				result.selfUsed = false;
			}
		}
	}
	else {
		result.selfUsed = false;
	}

	return result;
}

ScopeList DotNode::FindSideEffectScope(const bool assign) {
	if (assign) {
		return node->GetSideEffectScope(assign);
	}

	return scope->AbsoluteName();
}

NodePtr DotNode::Optimize(OptimizeInfo& info) {
	if (NodePtr n = node->Optimize(info)) node = n;

	return nullptr;
}

StringBuilder DotNode::ToMelon(const UInt indent) const {
	StringBuilder sb = node->ToMelon(indent);
	sb += ".";
	sb += name.ToString();
	return sb;
}