#include "DotNode.h"

#include "NameNode.h"

#include "Melon/Parsing/Parser.h"
#include "Melon/Parsing/IncludeParser.h"

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
	/* TODO: node
	const Symbols s = GetSymbol();

	if (s.type == SymbolType::Value || s.type == SymbolType::Variable || s.type == SymbolType::Template) {
		return s.varType;
	}
	else if (s.type != SymbolType::None) {
		return s.scope;
	}
	*/

	return nullptr;
}

Symbol* DotNode::GetSymbol() const {
	UInt errorCount = ErrorLog::ErrorCount();

	/* TODO: node
	Symbols nodeSymbol = node->GetSymbol();

	if (errorCount < ErrorLog::ErrorCount()) {
		return Symbols();
	}

	if (nodeSymbol.type == SymbolType::Variable) {
		Symbols s = Symbols::Find(node->Type(), file);

		if (s.type == SymbolType::Struct) {
			if (s.type != SymbolType::None && s.Contains(name)) {
				const Symbols s2 = s.Get(name, file);

				if (s2.type != SymbolType::None) {
					return Symbols::FindNearest(s2.scope, s2.varType, file);
				}
			}
		}
	}
	else {
		if (nodeSymbol.type != SymbolType::None && name.types && !nodeSymbol.Contains(name)) {
			Symbols::TemplateSymbol ts;
			ts.type = nodeSymbol.scope.Add(name);
			ts.scope = scope;
			ts.file = file;
			Node::root->AddTemplateSpecialization(ts);
		}

		return nodeSymbol.Get(name, file);
	}
	*/

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
	
	/* TODO: node
	Symbols s = node->GetSymbol();

	if (s.type == SymbolType::Namespace && !s.Contains(name)) {
		IncludeParser::ParseInclude(s.scope.Add(name), info);
	}
	*/
}

Set<ScanType> DotNode::Scan(ScanInfoStack& info) {
	bool assign = info.Get().assign;
	info.Get().assign = false;

	UInt errorCount = ErrorLog::ErrorCount();

	Set<ScanType> scanSet = node->Scan(info);

	if (errorCount < ErrorLog::ErrorCount()) {
		return scanSet;
	}

	Symbol* const type = node->Type();

	/* TODO: node
	Symbols var = Symbols::Find(type.Add(name), file);

	if (var.type == SymbolType::Variable && var.attributes.Contains(SymbolAttribute::Static)) {
		info.usedVariables.Add(var.scope);
	}

	info.Get().assign = assign;

	if (info.Get().assign) {
		if (const Pointer<NameNode>& nn = node.Cast<NameNode>()) {
			if (nn->name == Scope::Self) {
				const ScopeList scope = type.Add(name);
				Symbols& s = Symbols::Find(scope.Pop(), file).Get(scope.Last(), file);

				if (s.type == SymbolType::Variable) {
					if (info.Get().scopeInfo.WillContinue()) s.isAssigned = true;

					scanSet.Remove(ScanType::Self);

					if (Symbols::Find(scope.Pop(), file).IsAssigned()) {
						info.Get().init = false;
					}
				}

				scanSet.Remove(ScanType::Self);
			}
		}
	}
	else if (info.Get().init) {
		if (const Pointer<NameNode>& nn = node.Cast<NameNode>()) {
			if (nn->name == Scope::Self) {
				const ScopeList scope = type.Add(name);
				Symbols& s = Symbols::Find(scope.Pop(), file).Get(scope.Last(), file);

				if (s.type == SymbolType::Variable) {
					if (!s.isAssigned) {
						ErrorLog::Error(CompileError(CompileError::SelfVarUseStart + name.ToString() + CompileError::SelfVarUseEnd, file));
					}
				}

				scanSet.Remove(ScanType::Self);
			}
		}
	}
	else {
		scanSet.Remove(ScanType::Self);
	}
	*/

	return scanSet;
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