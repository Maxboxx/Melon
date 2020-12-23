#include "FunctionNode.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Optimizing/OptimizerInstruction.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Optimizing;

FunctionNode::FunctionNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

FunctionNode::~FunctionNode() {

}

bool FunctionNode::IsScope() const {
	return true;
}

bool FunctionNode::IsNotSpecialized() const {
	/* TODO: node
	Symbols sym = Symbols::Find(ScopeList().Add(s.scope[0]), file);

	for (UInt i = 0; i < s.scope.Size(); i++) {
		if (i > 0) {
			sym = sym.Get(s.scope[i], file);
		}

		if (!sym.templateArgs.IsEmpty()) {
			for (const ScopeList& type : sym.templateArgs) {
				if (Symbols::Contains(type)) {
					if (Symbols::Find(type, file).type == SymbolType::Template) {
						return true;
					}
				}
			}
		}
	}
	*/

	return false;
}

void FunctionNode::SetTemplateValues() const {
	/* TODO: node
	Symbols::SetTemplateValues(func, file);
	*/
}

CompiledNode FunctionNode::Compile(CompileInfo& info) { // TODO: more accurate arg error lines
	if (IsNotSpecialized()) return CompiledNode();
	SetTemplateValues();

	CompiledNode c;

	Instruction func = Instruction(InstructionType::Function);
	func.instructionName = this->func.ToString();
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
	if (IsNotSpecialized()) return;
	SetTemplateValues();

	node->IncludeScan(info);
}

Set<ScanType> FunctionNode::Scan(ScanInfoStack& info) {
	if (IsNotSpecialized()) return Set<ScanType>();
	SetTemplateValues();

	info.Push();

	/* TODO: node
	Symbols::Find(this->func, file);
	info.Get().scopeInfo.type = ScopeInfo::ScopeType::Function;
	info.Get().scopeInfo.Reset();
	info.Get().file = file;

	if (func.Pop().Last() == Scope::Init) {
		info.Get().init = true;
		info.Get().symbol = Symbols::Find(func.Pop().Pop(), file);
		info.Get().symbol.ClearAssign();
	}

	Set<ScanType> scanSet = node->Scan(info);

	if (info.Get().init && !info.Get().symbol.IsAssigned()) {
		for (const Scope& var : info.Get().symbol.GetUnassignedVars()) {
			ErrorLog::Error(CompileError(CompileError::VarNotInitStart + var.ToString() + CompileError::VarNotInitEnd, file));
		}
	}

	for (const ScopeList& sl : s.arguments) {
		Symbols s = Symbols::FindNearestInNamespace(scope, sl, file);

		if (s.type != SymbolType::None && s.attributes.Contains(SymbolAttribute::Ref)) {
			info.usedVariables.Add(s.scope);
		}
	}

	if (!info.Get().scopeInfo.hasReturned && !s.returnValues.IsEmpty()) {
		ErrorLog::Error(CompileError(CompileError::FuncNotReturn(s), file));
	}

	info.Pop();
	return scanSet;
	*/
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

	for (const SymbolAttribute attr : s.attributes) {
		switch (attr) {
			case SymbolAttribute::Debug:    sb += "debug "; break;
			case SymbolAttribute::Override: sb += "override "; break;
			case SymbolAttribute::Required: sb += "required "; break;
			case SymbolAttribute::Static:   sb += "static "; break;
		}
	}
	
	sb += "function ";

	const bool isOperator = s.scope.Last().name != Scope::Call.name;

	if (isOperator) {
		sb += "operator ";
	}

	for (UInt i = 0; i < s.returnValues.Size(); i++) {
		if (i > 0) sb += ", ";

		/* TODO: node
		Symbols sym = Symbols::FindNearestInNamespace(s.scope.Pop(), s.returnValues[i], file);
		sb += sym.scope.ToSimpleString();
		*/
	}

	if (!s.returnValues.IsEmpty()) {
		sb += ": ";
	}

	if (isOperator) {
		/* TODO: node
		Scope scope = func.Last();
		scope.variant = nullptr;
		sb += scope.ToSimpleString();
		*/
	}
	else {
		sb += func.Pop().Last().ToSimpleString();
	}

	sb += "(";

	const UInt start = (s.type == SymbolType::Method || (!argNames.IsEmpty() && argNames[0] == Scope::Self)) ? 1 : 0;

	for (UInt i = start; i < argNames.Size(); i++) {
		if (i > start) sb += ", ";

		/* TODO: node
		sb += s.Get(argNames[i], file).varType.ToSimpleString();
		sb += ": ";

		for (const SymbolAttribute attr : s.Get(argNames[i], file).attributes) {
			switch (attr) {
				case SymbolAttribute::Const: sb += "const "; break;
				case SymbolAttribute::Copy:  sb += "copy "; break;
				case SymbolAttribute::Ref:   sb += "ref "; break;
			}
		}
		*/

		sb += argNames[i].ToSimpleString();
	}

	sb += ")\n";
	sb += String('\t').Repeat(indent + 1);
	sb += node->ToMelon(indent + 1);
	sb += "\n";
	sb += String('\t').Repeat(indent);
	sb += "end";

	return sb;
}