#include "ReturnNode.h"

#include "MemoryNode.h"
#include "TypeNode.h"

#include "Kiwi/Kiwi.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/FunctionSymbol.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

ReturnNode::ReturnNode(Symbol* const scope, const FileInfo& file) : Node(scope, file) {

}

ReturnNode::~ReturnNode() {

}

FunctionSymbol* ReturnNode::GetFunc() const {
	return SymbolTable::Find<FunctionSymbol>(func, scope->AbsoluteName(), file, SymbolTable::SearchOptions::ReplaceTemplates);
}

List<TypeSymbol*> ReturnNode::GetTypes() const {
	List<TypeSymbol*> types;

	if (FunctionSymbol* const f = GetFunc()) {
		for (UInt i = 0; i < f->returnValues.Size(); i++) {
			types.Add(f->ReturnType(i));
		}
	}

	return types;
}

CompiledNode ReturnNode::Compile(CompileInfo& info) {
	/* TODO: node
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
	*/

	CompiledNode c;

	for (UInt i = 0; i < nodes.Size(); i++) {
		/* TODO: node
		stackOffset -= types[i].size;

		Pointer<MemoryNode> sn = new MemoryNode(stackOffset);
		sn->type = types[i].scope;

		info.important = true;
		c.AddInstructions(CompileAssignment(sn, nodes[i], info, nodes[i]->file).instructions);
		info.important = false;
		*/
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

ScanResult ReturnNode::Scan(ScanInfoStack& info) {
	if (info.ScopeInfo().CanContinue()) {
		info.ScopeInfo().hasReturned = true;
	}

	info.ScopeInfo().willNotReturn = false;

	ScanResult result;

	for (const NodePtr& node : nodes) {
		ScanResult r = node->Scan(info);
		r.SelfUseCheck(info, node->file);
		result |= r;
	}

	FunctionSymbol* const f = GetFunc();

	if (f == nullptr) return result;

	if (f->returnValues.Size() != nodes.Size()) {
		ErrorLog::Error(CompileError(CompileError::Return(f->returnValues.Size(), nodes.Size()), file));
	}

	List<TypeSymbol*> types = GetTypes();

	for (UInt i = 0; i < nodes.Size(); i++) {
		if (i >= types.Size()) break;

		if (types[i]) {
			ScanAssignment(new TypeNode(types[i]->AbsoluteName()), nodes[i], info, nodes[i]->file);
		}
	}

	return result;
}

ScopeList ReturnNode::FindSideEffectScope(const bool assign) {
	ScopeList list = nodes.IsEmpty() ? scope->AbsoluteName() : scope->Parent()->AbsoluteName();

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

StringBuilder ReturnNode::ToMelon(const UInt indent) const {
	StringBuilder sb = "return";

	for (UInt i = 0; i < nodes.Size(); i++) {
		if (i > 0) sb += ", ";
		else sb += " ";
		sb += nodes[i]->ToMelon(indent);
	}

	return sb;
}