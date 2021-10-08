#include "ReturnNode.h"

#include "MemoryNode.h"
#include "TypeNode.h"

#include "Kiwi/Kiwi.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/FunctionSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

ReturnNode::ReturnNode(Symbol* const scope, const FileInfo& file) : StatementNode(scope, file) {

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
	for (UInt i = 0; i < values.Size(); i++) {
		stackOffset -= types[i]->Size();

		Pointer<MemoryNode> sn = new MemoryNode(stackOffset);
		sn->type = types[i]->AbsoluteName();

		info.important = true;
		c.AddInstructions(CompileAssignment(sn, values[i], info, values[i]->File()).instructions);
		info.important = false;
	}

	// Add return instruction
	info.stack.PopExpr(0, c);
	c.instructions.Add(Instruction(InstructionType::Ret));

	return c;
}

void ReturnNode::IncludeScan(ParsingInfo& info) {
	for (Expression& value : values) {
		value->IncludeScan(info);
	}
}

ScanResult ReturnNode::Scan(ScanInfoStack& info) {
	if (info->scopeInfo.CanContinue()) {
		info->scopeInfo.hasReturned = true;
	}

	info->scopeInfo.willNotReturn = false;

	ScanResult result;

	// Scan nodes
	for (const Expression& value : values) {
		ScanResult r = value->Scan(info);
		r.SelfUseCheck(info, value->File());
		result |= r;
	}

	FunctionSymbol* const f = GetFunc();

	if (f == nullptr) return result;

	// Check for correct number of return values
	if (f->returnValues.Size() > values.Size()) {
		ErrorLog::Error(LogMessage("error.scan.return.many"), file);
	}
	else if (f->returnValues.Size() < values.Size()) {
		ErrorLog::Error(LogMessage("error.scan.return.few"), file);
	}

	List<TypeSymbol*> types = GetTypes();

	// Scan assignment to return values
	for (UInt i = 0; i < values.Size(); i++) {
		if (i >= types.Size()) break;

		if (types[i]) {
			ScanAssignment(new TypeNode(types[i]->AbsoluteName()), values[i], info, values[i]->File());
		}
	}

	return result;
}

NameList ReturnNode::FindSideEffectScope(const bool assign) {
	NameList list = values.IsEmpty() ? scope->AbsoluteName() : scope->Parent()->AbsoluteName();

	for (Expression& value : values) {
		list = CombineSideEffects(list, value->GetSideEffectScope(assign));
	}

	return list;
}

Statement ReturnNode::Optimize(OptimizeInfo& info) {
	for (Expression& value : values) {
		Node::Optimize(value, info);
	}

	return nullptr;
}

StringBuilder ReturnNode::ToMelon(const UInt indent) const {
	StringBuilder sb = "return";

	for (UInt i = 0; i < values.Size(); i++) {
		if (i > 0) sb += ", ";
		else sb += " ";
		sb += values[i]->ToMelon(indent);
	}

	return sb;
}