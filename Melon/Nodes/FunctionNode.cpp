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

	Instruction func = Instruction(InstructionType::Function);
	func.instructionName = sym->AbsoluteName().ToString();
	c.instructions.Add(func);

	UInt funcSize = node->GetSize();

	if (funcSize > 0) {
		OptimizerInstruction push = Instruction(InstructionType::Push, funcSize);
		push.important = true;
		c.instructions.Add(push);
	}

	StackPtr stack = info.stack;
	info.stack.frame = funcSize;
	info.stack.top = 0;

	Long size = info.stack.ptrSize;

	/* TODO: node
	for (Int i = s.arguments.Size() - 1; i >= 0; i--) {
		s.Get(argNames[i], file).stackIndex = -size;

		if (s.Get(argNames[i], file).attributes.Contains(SymbolAttribute::Ref)) {
			size += info.stack.ptrSize;
		}
		else {
			size += Symbols::FindNearestInNamespace(scope, s.arguments[i], file).size;
		}
	}
	*/

	c.AddInstructions(node->Compile(info).instructions);

	info.stack = stack;

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

	info.Push();
	
	info.ScopeInfo().type = ScopeInfo::ScopeType::Function;
	info.ScopeInfo().Reset();
	info.File(file);

	if (sym->AbsoluteName().Pop().Last() == Scope::Init) {
		info.Init(true);
		info.Type(sym->Parent()->Parent<TypeSymbol>());
		info.Type()->PrepareInit();
	}

	ScanResult result = node->Scan(info);

	if (info.Init() && !info.Type()->IsInitialized()) {
		for (const Scope& var : info.Type()->UnassignedMembers()) {
			ErrorLog::Error(CompileError(CompileError::VarNotInitStart + var.ToString() + CompileError::VarNotInitEnd, file));
		}
	}

	for (UInt i = 0; i < sym->arguments.Size(); i++) {
		VariableSymbol* const arg = sym->Argument(i);

		if (arg && (arg->attributes & VariableAttributes::Ref) != VariableAttributes::None) {
			info.usedVariables.Add(arg);
		}
	}

	if (!info.ScopeInfo().hasReturned && !sym->returnValues.IsEmpty()) {
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
	
	sb += "function ";

	if (sym->isOperator) {
		sb += "operator ";
	}

	for (UInt i = 0; i < sym->returnValues.Size(); i++) {
		if (i > 0) sb += ", ";
		sb += sym->returnValues[i].ToSimpleString();
	}

	if (!sym->returnValues.IsEmpty()) {
		sb += ": ";
	}

	sb += sym->Parent()->Name().ToSimpleString();

	if (!sym->templateArguments.IsEmpty()) {
		sb += "<";

		for (UInt i = 0; i < sym->templateArguments.Size(); i++) {
			if (i > 0) sb += ", ";
			sb += sym->templateArguments[i].ToSimpleString();
		}

		sb += ">";
	}

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
	sb += String('\t').Repeat(indent + 1);
	sb += node->ToMelon(indent + 1);
	sb += "\n";
	sb += String('\t').Repeat(indent);
	sb += "end";

	return sb;
}
