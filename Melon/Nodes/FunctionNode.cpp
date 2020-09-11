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
	Symbol sym = Symbol::Find(ScopeList().Add(s.scope[0]), file);

	for (UInt i = 0; i < s.scope.Size(); i++) {
		if (i > 0) {
			sym = sym.Get(s.scope[i], file);
		}

		if (!sym.templateArgs.IsEmpty()) {
			for (const ScopeList& type : sym.templateArgs) {
				if (type.Size() == 1 && sym.Contains(type.Last())) {
					if (sym.Get(type.Last(), file).type == SymbolType::Template) {
						return true;
					}
				}
			}
		}
	}

	return false;
}

void FunctionNode::SetTemplateValues() const {
	Symbol::SetTemplateValues(func, file);
}

CompiledNode FunctionNode::Compile(CompileInfo& info) { //TODO: more accurate arg error lines
	if (IsNotSpecialized()) return CompiledNode();
	SetTemplateValues();

	CompiledNode c;

	Instruction func = Instruction::Function(Symbol::Find(this->func, file).size);
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

	for (Int i = s.args.Size() - 1; i >= 0; i--) {
		s.Get(argNames[i], file).stack = -size;

		if (s.Get(argNames[i], file).attributes.Contains(SymbolAttribute::Ref)) {
			size += info.stack.ptrSize;
		}
		else {
			size += Symbol::FindNearest(scope, s.args[i], file).size;
		}
	}

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

	Symbol::Find(this->func, file);
	info.Get().scopeInfo.type = ScopeInfo::ScopeType::Function;
	info.Get().scopeInfo.Reset();
	info.Get().file = file;

	if (func.Pop().Last() == Scope::Init) {
		info.Get().init = true;
		info.Get().symbol = Symbol::Find(func.Pop().Pop(), file);
		info.Get().symbol.ClearAssign();
	}

	Set<ScanType> scanSet = node->Scan(info);

	if (info.Get().init && !info.Get().symbol.IsAssigned()) {
		for (const Scope& var : info.Get().symbol.GetUnassignedVars()) {
			ErrorLog::Error(CompileError(CompileError::VarNotInitStart + var.ToString() + CompileError::VarNotInitEnd, file));
		}
	}

	for (const ScopeList& sl : s.args) {
		Symbol::FindNearest(scope, sl, file);
	}

	if (!info.Get().scopeInfo.hasReturned && !s.ret.IsEmpty()) {
		ErrorLog::Error(CompileError(CompileError::FuncNotReturn(s), file));
	}

	info.Pop();
	return scanSet;
}

Mango FunctionNode::ToMango() const {
	Mango m = node->ToMango();
	m.SetLabel(func.ToString());
	return m;
}