#include "FunctionBody.h"

#include "Statements.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Optimizing/OptimizerInstruction.h"

#include "Melon/Symbols/TemplateSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Optimizing;

FunctionBody::FunctionBody(Symbol* const scope, const FileInfo& file) : Statement(scope, file) {

}

FunctionBody::~FunctionBody() {

}

bool FunctionBody::IsScope() const {
	return true;
}

Ptr<Kiwi::Value> FunctionBody::Compile(CompileInfo& info) { // TODO: more accurate arg error lines
	if (sym->IsNotSpecialized()) return nullptr;
	scope->SetTemplateValues(sym);

	Ptr<Kiwi::Function> func = new Kiwi::Function(sym->KiwiName());

	Weak<Kiwi::InstructionBlock> prevBlock = info.currentBlock;
	info.SetCodeBlock(func->block);
	info.returnRegisters.Clear();

	for (UInt i = 0; i < sym->arguments.Count(); i++) {
		VariableSymbol* arg = sym->Argument(i);
		func->AddArgument(arg->KiwiType(), arg->KiwiName());
	}

	for (UInt i = 0; i < sym->returnValues.Count(); i++) {
		TypeSymbol* ret = sym->ReturnType(i);
		String reg = info.NewRegister();
		info.returnRegisters.Add(reg);
		func->AddReturnValue(ret->KiwiType(), reg);
	}

	statements->Compile(info);

	info.currentBlock = prevBlock;

	info.program->AddFunction(func);
	return nullptr;
}

void FunctionBody::IncludeScan(ParsingInfo& info) {
	for (UInt i = 0; i < sym->arguments.Count(); i++) {
		if (VariableSymbol* const arg = sym->Argument(i)) {
			if (!arg->Type()) {
				Root()->AddTemplateSpecialization(arg->type, arg->Parent()->AbsoluteName(), arg->File());
			}
		}
	}

	if (sym->IsNotSpecialized()) return;
	scope->SetTemplateValues(sym);

	statements->IncludeScan(info);
}

ScanResult FunctionBody::Scan(ScanInfoStack& info) {
	if (sym->IsNotSpecialized()) return ScanResult();
	scope->SetTemplateValues(sym);

	// Setup scan info
	info.Push();
	
	info->scopeInfo.type = ScopeInfo::ScopeType::Function;
	info->scopeInfo.Reset();
	info->file = file;

	if (sym->AbsoluteName().Pop().Last() == Name::Init) {
		info->init = true;
		info->type = sym->Parent()->Parent<TypeSymbol>();
		info->type->PrepareInit();
	}

	// Scan function body
	ScanResult result = statements->Scan(info);

	// Check if member variables are initialized
	if (info->init && !info->type->IsInitialized()) {
		for (const Name& var : info->type->UnassignedMembers()) {
			ErrorLog::Error(LogMessage("error.scan.init.member", var.ToSimpleString()), file);
		}
	}

	// Check if arguments are used
	for (UInt i = 0; i < sym->arguments.Count(); i++) {
		VariableSymbol* const arg = sym->Argument(i);

		if (arg && (arg->modifiers & VariableModifiers::Ref) != VariableModifiers::None) {
			info.usedVariables.Add(arg);
		}
	}

	// Check if the function has not returned if it needs to
	if (!info->scopeInfo.hasReturned && !sym->returnValues.IsEmpty()) {
		ErrorLog::Error(LogMessage(sym->returnValues.Count() == 1 ? "error.scan.return.value" : "error.scan.return.values", sym->ToString()), file);
	}

	info.Pop();

	result.selfUsed = false;
	return result;
}

Ptr<Statement> FunctionBody::Optimize(OptimizeInfo& info) {
	Node::Optimize(statements, info);
	return nullptr;
}

StringBuilder FunctionBody::ToMelon(const UInt indent) const {
	StringBuilder sb = "";

	if ((sym->modifiers & FunctionModifiers::Static) != FunctionModifiers::None) {
		sb += "static ";
	}

	if ((sym->modifiers & FunctionModifiers::Override) != FunctionModifiers::None) {
		sb += "override ";
	}

	if ((sym->modifiers & FunctionModifiers::Partial) != FunctionModifiers::None) {
		sb += "partial ";
	}

	if ((sym->modifiers & FunctionModifiers::Required) != FunctionModifiers::None) {
		sb += "required ";
	}
	
	// Create functon head
	sb += "function ";

	if (sym->isOperator) {
		sb += "operator ";
	}

	// Add return types
	for (UInt i = 0; i < sym->returnValues.Count(); i++) {
		if (i > 0) sb += ", ";
		sb += sym->returnValues[i].ToSimpleString();
	}

	if (!sym->returnValues.IsEmpty()) {
		sb += ": ";
	}

	if ((sym->attributes & FunctionAttributes::Throw) != FunctionAttributes::None) {
		sb += "throw ";
	}

	// Get name of function with template arguments
	sb += sym->Parent()->Name().ToSimpleString();

	if (!sym->templateArguments.IsEmpty()) {
		sb += "<";

		for (UInt i = 0; i < sym->templateArguments.Count(); i++) {
			if (i > 0) sb += ", ";
			sb += sym->templateArguments[i].ToSimpleString();
		}

		sb += ">";
	}

	// Create argument list
	sb += "(";

	const UInt start = (!sym->arguments.IsEmpty() && sym->arguments[0][0] == Name::Self) ? 1 : 0;

	for (UInt i = start; i < sym->arguments.Count(); i++) {
		if (i > start) sb += ", ";

		VariableSymbol* const arg = sym->Argument(i);

		sb += arg->type.ToSimpleString();
		sb += ": ";

		if ((arg->modifiers & VariableModifiers::Const) != VariableModifiers::None) {
			sb += "const ";
		}

		if ((arg->modifiers & VariableModifiers::Ref) != VariableModifiers::None) {
			sb += "ref ";
		}

		sb += sym->arguments[i].ToSimpleString();
	}

	sb += ")\n";

	// Add function body
	sb += String('\t').Repeat(indent + 1);
	sb += statements->ToMelon(indent + 1);
	sb += "\n";
	sb += String('\t').Repeat(indent);
	sb += "end";

	return sb;
}
