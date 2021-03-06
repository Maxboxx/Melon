#include "FunctionNode.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Optimizing/OptimizerInstruction.h"

#include "Melon/Symbols/TemplateSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Optimizing;

FunctionNode::FunctionNode(Symbol* const scope, const FileInfo& file) : Node(scope, file) {

}

FunctionNode::~FunctionNode() {

}

bool FunctionNode::IsScope() const {
	return true;
}

CompiledNode FunctionNode::Compile(CompileInfo& info) { // TODO: more accurate arg error lines
	if (sym->IsNotSpecialized()) return CompiledNode();
	scope->SetTemplateValues(sym);

	CompiledNode c;

	// Function instruction
	Instruction func = Instruction(InstructionType::Function);
	func.instructionName = sym->AbsoluteName().ToString();
	c.instructions.Add(func);

	UInt funcSize = node->GetSize();

	// Add push if the function needs memory
	if (funcSize > 0) {
		OptimizerInstruction push = Instruction(InstructionType::Push, funcSize);
		push.important = true;
		c.instructions.Add(push);
	}

	StackPtr stack = info.stack;
	info.stack.frame = funcSize;
	info.stack.top = 0;

	Long size = info.stack.ptrSize;

	// Set stack index of arguments
	for (Long i = sym->arguments.Size() - 1; i >= 0; i--) {
		VariableSymbol* const arg = sym->Argument(i);
		arg->stackIndex = -size;

		if (arg->HasAttribute(VariableAttributes::Ref)) {
			size += info.stack.ptrSize;
		}
		else {
			size += arg->Type()->Size();
		}
	}

	// Compile function body
	c.AddInstructions(node->Compile(info).instructions);

	info.stack = stack;

	// Add return and pop if needed
	if (c.instructions.Last().instruction.type != InstructionType::Ret) {
		if (funcSize > 0) {
			OptimizerInstruction pop = Instruction(InstructionType::Pop, funcSize);
			pop.important = true;
			c.instructions.Add(pop);
		}

		c.instructions.Add(Instruction(InstructionType::Ret));
	}

	return c;
}

void FunctionNode::IncludeScan(ParsingInfo& info) {
	for (UInt i = 0; i < sym->arguments.Size(); i++) {
		if (VariableSymbol* const arg = sym->Argument(i)) {
			if (!arg->Type()) {
				Node::root->AddTemplateSpecialization(arg->type, arg->Parent()->AbsoluteName(), arg->File());
			}
		}
	}

	if (sym->IsNotSpecialized()) return;
	scope->SetTemplateValues(sym);

	node->IncludeScan(info);
}

ScanResult FunctionNode::Scan(ScanInfoStack& info) {
	if (sym->IsNotSpecialized()) return ScanResult();
	scope->SetTemplateValues(sym);

	// Setup scan info
	info.Push();
	
	info->scopeInfo.type = ScopeInfo::ScopeType::Function;
	info->scopeInfo.Reset();
	info->file = file;

	if (sym->AbsoluteName().Pop().Last() == Scope::Init) {
		info->init = true;
		info->type = sym->Parent()->Parent<TypeSymbol>();
		info->type->PrepareInit();
	}

	// Scan function body
	ScanResult result = node->Scan(info);

	// Check if member variables are initialized
	if (info->init && !info->type->IsInitialized()) {
		for (const Scope& var : info->type->UnassignedMembers()) {
			ErrorLog::Error(CompileError(CompileError::VarNotInitStart + var.ToString() + CompileError::VarNotInitEnd, file));
		}
	}

	// Check if arguments are used
	for (UInt i = 0; i < sym->arguments.Size(); i++) {
		VariableSymbol* const arg = sym->Argument(i);

		if (arg && (arg->attributes & VariableAttributes::Ref) != VariableAttributes::None) {
			info.usedVariables.Add(arg);
		}
	}

	// Check if the function has not returned if it needs to
	if (!info->scopeInfo.hasReturned && !sym->returnValues.IsEmpty()) {
		ErrorLog::Error(CompileError(CompileError::FuncNotReturn(sym), file));
	}

	info.Pop();

	result.selfUsed = false;
	return result;
}

NodePtr FunctionNode::Optimize(OptimizeInfo& info) {
	if (NodePtr n = node->Optimize(info)) node = n;

	return nullptr;
}

StringBuilder FunctionNode::ToMelon(const UInt indent) const {
	StringBuilder sb = "";

	// Get attribute names

	if ((sym->attributes & FunctionAttributes::Debug) != FunctionAttributes::None) {
		sb += "debug ";
	}

	if ((sym->attributes & FunctionAttributes::Static) != FunctionAttributes::None) {
		sb += "static ";
	}

	if ((sym->attributes & FunctionAttributes::Override) != FunctionAttributes::None) {
		sb += "override ";
	}

	if ((sym->attributes & FunctionAttributes::Partial) != FunctionAttributes::None) {
		sb += "partial ";
	}

	if ((sym->attributes & FunctionAttributes::Required) != FunctionAttributes::None) {
		sb += "required ";
	}
	
	// Create functon head
	sb += "function ";

	if (sym->isOperator) {
		sb += "operator ";
	}

	// Add return types
	for (UInt i = 0; i < sym->returnValues.Size(); i++) {
		if (i > 0) sb += ", ";
		sb += sym->returnValues[i].ToSimpleString();
	}

	if (!sym->returnValues.IsEmpty()) {
		sb += ": ";
	}

	// Get name of function with template arguments
	sb += sym->Parent()->Name().ToSimpleString();

	if (!sym->templateArguments.IsEmpty()) {
		sb += "<";

		for (UInt i = 0; i < sym->templateArguments.Size(); i++) {
			if (i > 0) sb += ", ";
			sb += sym->templateArguments[i].ToSimpleString();
		}

		sb += ">";
	}

	// Create argument list
	sb += "(";

	const UInt start = (!sym->arguments.IsEmpty() && sym->arguments[0][0] == Scope::Self) ? 1 : 0;

	for (UInt i = start; i < sym->arguments.Size(); i++) {
		if (i > start) sb += ", ";

		VariableSymbol* const arg = sym->Argument(i);

		sb += arg->type.ToSimpleString();
		sb += ": ";

		if ((arg->attributes & VariableAttributes::Const) != VariableAttributes::None) {
			sb += "const ";
		}

		if ((arg->attributes & VariableAttributes::Ref) != VariableAttributes::None) {
			sb += "ref ";
		}

		sb += sym->arguments[i].ToSimpleString();
	}

	sb += ")\n";

	// Add function body
	sb += String('\t').Repeat(indent + 1);
	sb += node->ToMelon(indent + 1);
	sb += "\n";
	sb += String('\t').Repeat(indent);
	sb += "end";

	return sb;
}
