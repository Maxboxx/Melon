#include "FunctionNode.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Optimizing/OptimizerInstruction.h"

#include "Melon/Symbols/TemplateSymbol.h"

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

bool FunctionNode::IsNotSpecialized() const {
	for (UInt i = 0; i < sym->templateArguments.Size(); i++) {
		if (sym->TemplateArgument(i)->Is<TemplateSymbol>()) {
			return true;
		}
	}

	return false;
}

void FunctionNode::SetTemplateValues() const {
	if (FunctionSymbol* const f = scope->Cast<FunctionSymbol>()) {
		for (UInt i = 0; i < f->templateArguments.Size(); i++) {
			if (TypeSymbol* const type = f->TemplateArgument(i)) {
				if (TemplateSymbol* const arg = type->Cast<TemplateSymbol>()) {
					if (TypeSymbol* const t = sym->TemplateArgument(i)) {
						arg->type = t->AbsoluteName();
					}
				}
			}
		}
	}
}

CompiledNode FunctionNode::Compile(CompileInfo& info) { // TODO: more accurate arg error lines
	if (IsNotSpecialized()) return CompiledNode();
	SetTemplateValues();

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

	if (IsNotSpecialized()) return;
	SetTemplateValues();

	node->IncludeScan(info);
}

ScanResult FunctionNode::Scan(ScanInfoStack& info) {
	if (IsNotSpecialized()) return ScanResult();
	SetTemplateValues();

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

	/* TODO: node
	for (const ScopeList& arg : Symbols::Find(s.scope, file).templateArgs) {
		if (Symbols::Find(arg, file).type == SymbolType::Template) {
			return "";
		}
	}
	*/

	/*for (const SymbolAttribute attr : sym.attributes) {
		switch (attr) {
			case SymbolAttribute::Debug:    sb += "debug "; break;
			case SymbolAttribute::Override: sb += "override "; break;
			case SymbolAttribute::Required: sb += "required "; break;
			case SymbolAttribute::Static:   sb += "static "; break;
		}
	}
	
	sb += "function ";

	const bool isOperator = sym.scope.Last().name != Scope::Call.name;

	if (isOperator) {
		sb += "operator ";
	}

	for (UInt i = 0; i < sym.returnValues.Size(); i++) {
		if (i > 0) sb += ", ";

		Symbols sym = Symbols::FindNearestInNamespace(s.scope.Pop(), s.returnValues[i], file);
		sb += sym.scope.ToSimpleString();
	}

	if (!sym.returnValues.IsEmpty()) {
		sb += ": ";
	}

	if (isOperator) {
		Scope scope = func.Last();
		scope.variant = nullptr;
		sb += scope.ToSimpleString();
	}
	else {
		sb += func.Pop().Last().ToSimpleString();
	}

	sb += "(";

	const UInt start = (sym.type == SymbolType::Method || (!argNames.IsEmpty() && argNames[0] == Scope::Self)) ? 1 : 0;

	for (UInt i = start; i < argNames.Size(); i++) {
		if (i > start) sb += ", ";

		sb += s.Get(argNames[i], file).varType.ToSimpleString();
		sb += ": ";

		for (const SymbolAttribute attr : s.Get(argNames[i], file).attributes) {
			switch (attr) {
				case SymbolAttribute::Const: sb += "const "; break;
				case SymbolAttribute::Copy:  sb += "copy "; break;
				case SymbolAttribute::Ref:   sb += "ref "; break;
			}
		}

		sb += argNames[i].ToSimpleString();
	}

	sb += ")\n";
	sb += String('\t').Repeat(indent + 1);
	sb += node->ToMelon(indent + 1);
	sb += "\n";
	sb += String('\t').Repeat(indent);
	sb += "end";
	*/

	return sb;
}