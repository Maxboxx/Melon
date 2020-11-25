#include "ReturnNode.h"

#include "MemoryNode.h"
#include "TypeNode.h"

#include "Kiwi/Kiwi.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

ReturnNode::ReturnNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

ReturnNode::~ReturnNode() {

}

List<Symbols> ReturnNode::GetTypes() const {
	List<Symbols> types;

	Symbols s = Symbols::Find(Symbols::ReplaceTemplates(func, file), file);
	
	for (ScopeList type : s.returnValues) {
		Symbols sym = Symbols::FindNearestInNamespace(Symbols::ReplaceTemplates(s.scope.Pop(), file), type, file);

		if (sym.type == SymbolType::Template) {
			types.Add(Symbols::Find(sym.varType, file));
		}
		else {
			types.Add(sym);
		}
	}

	return types;
}

CompiledNode ReturnNode::Compile(CompileInfo& info) {
	Symbols s = Symbols::Find(Symbols::ReplaceTemplates(func, file), file);

	UInt stackOffset = info.stack.ptrSize + info.stack.frame;
	List<Symbols> types = GetTypes();

	for (const Symbols& sym : types) {
		stackOffset += sym.size;
	}

	for (UInt i = 0; i < s.arguments.Size(); i++) {
		Symbols sym = Symbols::FindNearestInNamespace(s.scope.Pop(), s.arguments[i], file);

		if (Symbols::Find(s.scope.Add(s.names[i]), file).attributes.Contains(SymbolAttribute::Ref)) {
			stackOffset += info.stack.ptrSize;
		}
		else {
			stackOffset += sym.size;
		}
	}

	CompiledNode c;

	for (UInt i = 0; i < nodes.Size(); i++) {
		stackOffset -= types[i].size;

		Pointer<MemoryNode> sn = new MemoryNode(stackOffset);
		sn->type = types[i].scope;

		info.important = true;
		c.AddInstructions(CompileAssignment(sn, nodes[i], info, nodes[i]->file).instructions);
		info.important = false;
	}

	info.stack.PopExpr(0, c);
	c.instructions.Add(Instruction(InstructionType::Ret));

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

	Symbols s = Symbols::Find(Symbols::ReplaceTemplates(func, file), file);

	if (s.returnValues.Size() != nodes.Size()) {
		ErrorLog::Error(CompileError(CompileError::Return(s.returnValues.Size(), nodes.Size()), file));
	}

	List<Symbols> types = GetTypes();

	for (UInt i = 0; i < s.arguments.Size(); i++) {
		Symbols::FindNearestInNamespace(s.scope.Pop(), s.arguments[i], file);
		Symbols::Find(s.scope.Add(s.names[i]), file);
	}

	for (UInt i = 0; i < nodes.Size(); i++) {
		if (i >= types.Size()) break;
		ScanAssignment(new TypeNode(types[i].scope), nodes[i], info, nodes[i]->file);
	}

	return scanSet;
}

ScopeList ReturnNode::FindSideEffectScope(const bool assign) {
	ScopeList list = nodes.IsEmpty() ? scope : scope.Pop();

	for (NodePtr& node : nodes) {
		list = CombineSideEffects(list, node->GetSideEffectScope(assign));
	}

	return list;
}

NodePtr ReturnNode::Optimize(OptimizeInfo& info) {
	for (NodePtr& node : nodes) {
		if (NodePtr n = node->Optimize(info)) n = node;
	}

	return nullptr;
}

Mango ReturnNode::ToMango() const {
	Mango m = Mango("return", MangoType::List);

	for (NodePtr node : nodes)
		m.Add(node->ToMango());

	return m;
}

StringBuilder ReturnNode::ToMelon(const UInt indent) const {
	StringBuilder sb = "return";

	for (UInt i = 0; i < nodes.Size(); i++) {
		if (i > 0) sb += ", ";
		else sb += " ";
		sb += nodes[i]->ToMelon(indent);
	}

	return sb;
}