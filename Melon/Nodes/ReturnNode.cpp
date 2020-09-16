#include "ReturnNode.h"

#include "StackNode.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

ReturnNode::ReturnNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

ReturnNode::~ReturnNode() {

}

List<Symbol> ReturnNode::GetTypes() const {
	List<Symbol> types;

	Symbol s = Symbol::Find(Symbol::ReplaceTemplates(func, file), file);
	
	for (ScopeList type : s.ret) {
		Symbol sym = Symbol::FindNearestInNamespace(Symbol::ReplaceTemplates(s.scope.Pop(), file), type, file);

		if (sym.type == SymbolType::Template) {
			types.Add(Symbol::Find(sym.varType, file));
		}
		else {
			types.Add(sym);
		}
	}

	return types;
}

CompiledNode ReturnNode::Compile(CompileInfo& info) {
	Symbol s = Symbol::Find(Symbol::ReplaceTemplates(func, file), file);

	UInt stackOffset = info.stack.ptrSize + info.stack.frame;
	List<Symbol> types = GetTypes();

	for (const Symbol& sym : types) {
		stackOffset += sym.size;
	}

	for (UInt i = 0; i < s.args.Size(); i++) {
		Symbol sym = Symbol::FindNearestInNamespace(s.scope.Pop(), s.args[i], file);

		if (Symbol::Find(s.scope.Add(s.names[i]), file).attributes.Contains(SymbolAttribute::Ref)) {
			stackOffset += info.stack.ptrSize;
		}
		else {
			stackOffset += sym.size;
		}
	}

	CompiledNode c;

	for (UInt i = 0; i < nodes.Size(); i++) {
		List<ScopeList> args;
		args.Add(nodes[i]->Type());

		Symbol assign = Symbol::FindFunction(types[i].scope.Add(Scope::Assign), args, nodes[i]->file);		

		stackOffset -= types[i].size;

		List<NodePtr> assignArgs;
		Pointer<StackNode> sn = new StackNode(stackOffset);
		sn->type = types[i].scope;
		assignArgs.Add(sn);
		assignArgs.Add(nodes[i]);

		info.important = true;
		c.AddInstructions(assign.symbolNode->Compile(assignArgs, info).instructions);
		info.important = false;
	}

	return c;
}

void ReturnNode::IncludeScan(ParsingInfo& info) {
	for (NodePtr node : nodes) {
		node->IncludeScan(info);
	}
}

Set<ScanType> ReturnNode::Scan(ScanInfoStack& info) {
	Set<ScanType> scanSet = Set<ScanType>();

	if (info.Get().scopeInfo.CanContinue()) {
		info.Get().scopeInfo.hasReturned = true;
	}

	info.Get().scopeInfo.willNotReturn = false;

	for (const NodePtr& node : nodes) {
		for (const ScanType type : node->Scan(info)) {
			scanSet.Add(type);
		}
	}

	Symbol s = Symbol::Find(Symbol::ReplaceTemplates(func, file), file);

	if (s.ret.Size() != nodes.Size()) {
		ErrorLog::Error(CompileError(CompileError::Return(s.ret.Size(), nodes.Size()), file));
	}

	List<Symbol> types = GetTypes();

	for (UInt i = 0; i < s.args.Size(); i++) {
		Symbol::FindNearestInNamespace(s.scope.Pop(), s.args[i], file);
		Symbol::Find(s.scope.Add(s.names[i]), file);
	}

	for (UInt i = 0; i < nodes.Size(); i++) {
		List<ScopeList> args;
		args.Add(types[i].scope);
		Symbol::FindFunction(types[i].scope.Add(Scope::Assign), args, nodes[i]->file);
	}

	return scanSet;
}

Mango ReturnNode::ToMango() const {
	Mango m = Mango("return", MangoType::List);

	for (NodePtr node : nodes)
		m.Add(node->ToMango());

	return m;
}