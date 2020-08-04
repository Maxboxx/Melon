#include "DotNode.h"

#include "NameNode.h"

#include "Melon/Parsing/Parser.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

DotNode::DotNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

DotNode::~DotNode() {

}

ScopeList DotNode::Type() const {
	const Symbol s = GetSymbol();

	if (s.type != SymbolType::None) {
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
			Symbol varType = Symbol::FindNearest(s.scope, s.varType, file);

			if (varType.type != SymbolType::None && varType.Contains(name)) {
				const Symbol s2 = varType.Get(name, file);

				if (s2.type != SymbolType::None) {
					return Symbol::FindNearest(s2.scope, s2.varType, file);
				}
			}
		}
	}
	else {
		Symbol s = Symbol::Find(nodeSymbol.scope, file);
		return s.Get(name, file);
	}

	return Symbol();
}

CompiledNode DotNode::Compile(CompileInfo& info) {
	const ScopeList type = node->Type();
	Symbol s = Symbol::Find(type, file);
	CompiledNode c = node->Compile(info);

	if (s.type == SymbolType::Struct) {
		UInt offset = s.size;

		for (UInt i = 0; i < s.args.Size(); i++) {
			Symbol var = Symbol::FindNearest(type, s.args[i], file);
			Symbol varType = Symbol::FindNearest(type, var.varType, file);
			offset -= varType.size;

			if (s.args[i] == ScopeList().Add(name)) {
				c.argument.mem.offset += offset;
				c.size = varType.size;
				break;	
			}
		}
	}

	return c;
}

void DotNode::IncludeScan(ParsingInfo& info) {
	
}

Set<ScanType> DotNode::Scan(ScanInfo& info) const {
	bool assign = info.assign;
	info.assign = false;

	UInt errorCount = ErrorLog::ErrorCount();

	Set<ScanType> scanSet = node->Scan(info);

	if (errorCount < ErrorLog::ErrorCount()) {
		return scanSet;
	}

	const ScopeList type = node->Type();
	Symbol::Find(type.Add(name), file);

	info.assign = assign;

	if (info.assign) {
		if (const Pointer<NameNode>& nn = node.Cast<NameNode>()) {
			if (nn->name == Scope::Self) {
				const ScopeList scope = node->Type().Add(name);
				Symbol& s = Symbol::Find(scope.Pop(), file).Get(scope.Last(), file);

				if (s.type == SymbolType::Variable) {
					s.sign = true;
					scanSet.Remove(ScanType::Self);

					if (Symbol::Find(scope.Pop(), file).IsAssigned()) {
						info.init = false;
					}
				}

				scanSet.Remove(ScanType::Self);
			}
		}
	}
	else if (info.init) {
		if (const Pointer<NameNode>& nn = node.Cast<NameNode>()) {
			if (nn->name == Scope::Self) {
				const ScopeList scope = node->Type().Add(name);
				Symbol& s = Symbol::Find(scope.Pop(), file).Get(scope.Last(), file);

				if (s.type == SymbolType::Variable) {
					if (!s.sign) {
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

Mango DotNode::ToMango() const {
	Mango m = Mango(node->Type().Add(name).ToString(), MangoType::List);
	m.Add(node->ToMango());
	return m;
}