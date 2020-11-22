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

DotNode::DotNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

DotNode::~DotNode() {

}

ScopeList DotNode::Type() const {
	const Symbol s = GetSymbol();

	if (s.type == SymbolType::Value || s.type == SymbolType::Variable || s.type == SymbolType::Template) {
		return s.varType;
	}
	else if (s.type != SymbolType::None) {
		return s.scope;
	}

	return ScopeList::undefined;
}

Symbol DotNode::GetSymbol() const {
	UInt errorCount = ErrorLog::ErrorCount();

	Symbol nodeSymbol = node->GetSymbol();

	if (errorCount < ErrorLog::ErrorCount()) {
		return Symbol();
	}

	if (nodeSymbol.type == SymbolType::Variable) {
		Symbol s = Symbol::Find(node->Type(), file);

		if (s.type == SymbolType::Struct) {
			if (s.type != SymbolType::None && s.Contains(name)) {
				const Symbol s2 = s.Get(name, file);

				if (s2.type != SymbolType::None) {
					return Symbol::FindNearest(s2.scope, s2.varType, file);
				}
			}
		}
	}
	else {
		return nodeSymbol.Get(name, file);
	}

	return Symbol();
}

CompiledNode DotNode::Compile(CompileInfo& info) {
	const ScopeList type = node->Type();
	Symbol s = Symbol::Find(type, file);
	CompiledNode c = node->Compile(info);

	if (s.type == SymbolType::Struct || s.type == SymbolType::Type) {
		Symbol var = s.Get(name, file);
		Symbol varType = var.GetType(file);

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
}

void DotNode::IncludeScan(ParsingInfo& info) {
	node->IncludeScan(info);
	
	Symbol s = node->GetSymbol();

	if (s.type == SymbolType::Namespace && !s.Contains(name)) {
		IncludeParser::ParseInclude(s.scope.Add(name), info);
	}

	if (name.types && !s.Contains(name)) {
		Symbol::TemplateSymbol ts;
		ts.type = s.scope.Add(name);
		ts.scope = scope;
		ts.file = file;
		Symbol::templateSymbols.Add(ts);
	}
}

Set<ScanType> DotNode::Scan(ScanInfoStack& info) {
	bool assign = info.Get().assign;
	info.Get().assign = false;

	UInt errorCount = ErrorLog::ErrorCount();

	Set<ScanType> scanSet = node->Scan(info);

	if (errorCount < ErrorLog::ErrorCount()) {
		return scanSet;
	}

	const ScopeList type = node->Type();
	Symbol var = Symbol::Find(type.Add(name), file);

	if (var.type == SymbolType::Variable && var.attributes.Contains(SymbolAttribute::Static)) {
		info.usedVariables.Add(var.scope);
	}

	info.Get().assign = assign;

	if (info.Get().assign) {
		if (const Pointer<NameNode>& nn = node.Cast<NameNode>()) {
			if (nn->name == Scope::Self) {
				const ScopeList scope = type.Add(name);
				Symbol& s = Symbol::Find(scope.Pop(), file).Get(scope.Last(), file);

				if (s.type == SymbolType::Variable) {
					if (info.Get().scopeInfo.WillContinue()) s.isAssigned = true;

					scanSet.Remove(ScanType::Self);

					if (Symbol::Find(scope.Pop(), file).IsAssigned()) {
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
				Symbol& s = Symbol::Find(scope.Pop(), file).Get(scope.Last(), file);

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

	return scanSet;
}

ScopeList DotNode::FindSideEffectScope(const bool assign) {
	if (assign) {
		return node->GetSideEffectScope(assign);
	}

	return scope;
}

NodePtr DotNode::Optimize(OptimizeInfo& info) {
	if (NodePtr n = node->Optimize(info)) node = n;

	return nullptr;
}

Mango DotNode::ToMango() const {
	Mango m = Mango(node->Type().Add(name).ToString(), MangoType::List);
	m.Add(node->ToMango());
	return m;
}

StringBuilder DotNode::ToMelon(const UInt indent) const {
	StringBuilder sb = node->ToMelon(indent);
	sb += ".";
	sb += name.ToString();
	return sb;
}