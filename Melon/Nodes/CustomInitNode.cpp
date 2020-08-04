#include "CustomInitNode.h"

#include "StackNode.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

CustomInitNode::CustomInitNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

CustomInitNode::~CustomInitNode() {

}

ScopeList CustomInitNode::Type() const {
	return node->Type();
}

CompiledNode CustomInitNode::Compile(CompileInfo& info) {
	CompiledNode c = node->Compile(info);

	Symbol s = Symbol::Find(Type(), file);

	UInt offset = s.size - 1;

	info.stack.Push(s.size);

	UInt index = info.index;

	for (UInt i = 0; i < vars.Size(); i++) {
		Symbol v = s.Get(vars[i], file);
		Symbol varType = v.GetType(file);
		const ScopeList type = varType.scope;

		List<ScopeList> argTypes;
		argTypes.Add(expressions[i]->Type());
		Symbol a = Symbol::FindFunction(type.Add(Scope::Assign), argTypes, expressions[i]->file);

		if (s.type == SymbolType::Struct) {
			Pointer<StackNode> sn = new StackNode(info.stack.Offset() + offset - v.stack);
			sn->type = type;

			List<NodePtr> args;
			args.Add(sn);
			args.Add(expressions[i]);
			c.AddInstructions(a.node->Compile(args, info).instructions);
		}

		info.index = index;
	}

	c.argument = Argument(MemoryLocation(info.stack.Offset()));
	info.stack.Pop(s.size);

	return c;
}

void CustomInitNode::IncludeScan(ParsingInfo& info) {
	
}

Set<ScanType> CustomInitNode::Scan(ScanInfo& info) const {
	Set<ScanType> scanSet = node->Scan(info);

	Symbol s = Symbol::Find(Type(), file);

	if (!s.IsAllRequiredVars(vars)) {
		ErrorLog::Error(CompileError(CompileError::VarNotCustomInit, file));
	}

	for (UInt i = 0; i < vars.Size(); i++) {
		for (UInt u = i + 1; u < vars.Size(); u++) {
			if (vars[i] == vars[u]) {
				ErrorLog::Error(CompileError(CompileError::MultipleInitStart + vars[i].name + CompileError::MultipleInitEnd, file));
			}
		}

		Symbol v = s.Get(vars[i], file);
		Symbol varType = v.GetType(file);
		const ScopeList type = varType.scope;

		List<ScopeList> argTypes;
		argTypes.Add(expressions[i]->Type());
		Symbol a = Symbol::FindFunction(type.Add(Scope::Assign), argTypes, expressions[i]->file);

		if (s.type != SymbolType::Struct) {
			ErrorLog::Error(CompileError(CompileError::InvalidCustomInit, file));
		}
	}

	if (info.init && scanSet.Contains(ScanType::Self) && !info.symbol.IsAssigned()) {
		ErrorLog::Error(CompileError(CompileError::SelfInit, node->file));
	}

	for (const NodePtr& node : expressions) {
		for (const ScanType type : node->Scan(info)) {
			scanSet.Add(type);

			if (info.init && type == ScanType::Self && !info.symbol.IsAssigned()) {
				ErrorLog::Error(CompileError(CompileError::SelfInit, node->file));
			}
		}
	}

	return scanSet;
}

Mango CustomInitNode::ToMango() const {
	Mango m = Mango("init", MangoType::List);

	m.Add(node->ToMango());

	Symbol s = Symbol::Find(Type(), file);

	for (UInt i = 0; i < vars.Size(); i++) {
		Symbol v = s.Get(vars[i], file);
		const ScopeList type = Symbol::FindNearest(Type(), v.varType, file).scope;

		List<ScopeList> argTypes;
		argTypes.Add(expressions[i]->Type());
		Mango a = Mango(Symbol::FindFunction(type.Add(Scope::Assign), argTypes, expressions[i]->file).scope.ToString(), MangoType::List);
		a.Add(Mango("var", vars[i].ToString()));
		a.Add(expressions[i]->ToMango());
		m.Add(a);
	}

	return m;
}