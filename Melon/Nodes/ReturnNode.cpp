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
	FunctionSymbol* const f = GetFunc();
	if (!f) return CompiledNode();

	UInt stackOffset = info.stack.ptrSize + info.stack.frame;
	List<TypeSymbol*> types = GetTypes();

	// Calculate stack offset for return values
	for (TypeSymbol* const type : types) {
		stackOffset += type->Size();
	}

	// Calculate stack offset for arguments
	for (UInt i = 0; i < f->arguments.Size(); i++) {
		VariableSymbol* const var = f->Argument(i);

		if (var->HasAttribute(VariableAttributes::Ref)) {
			stackOffset += info.stack.ptrSize;
		}
		else {
			stackOffset += var->Type()->Size();
		}
	}

	CompiledNode c;

	// Compile return values
	for (UInt i = 0; i < nodes.Size(); i++) {
		stackOffset -= types[i]->Size();

		Pointer<MemoryNode> sn = new MemoryNode(stackOffset);
		sn->type = types[i]->AbsoluteName();

		info.important = true;
		c.AddInstructions(CompileAssignment(sn, nodes[i], info, nodes[i]->file).instructions);
		info.important = false;
	}

	// Add return instruction
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
	if (info->scopeInfo.CanContinue()) {
		info->scopeInfo.hasReturned = true;
	}

	info->scopeInfo.willNotReturn = false;

	ScanResult result;

	// Scan nodes
	for (const NodePtr& node : nodes) {
		ScanResult r = node->Scan(info);
		r.SelfUseCheck(info, node->file);
		result |= r;
	}

	FunctionSymbol* const f = GetFunc();

	if (f == nullptr) return result;

	// Check for correct number of return values
	if (f->returnValues.Size() != nodes.Size()) {
		ErrorLog::Error(CompileError(CompileError::Return(f->returnValues.Size(), nodes.Size()), file));
	}

	List<TypeSymbol*> types = GetTypes();

	// Scan assignment to return values
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