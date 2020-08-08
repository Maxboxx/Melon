#include "FunctionNode.h"

#include "Melon/Parsing/Parser.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;

FunctionNode::FunctionNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

FunctionNode::~FunctionNode() {

}

CompiledNode FunctionNode::Compile(CompileInfo& info) { //TODO: more accurate arg error lines
	CompiledNode c;

	Instruction func = Instruction::Function(Symbol::Find(this->func, file).size);
	c.instructions.Add(func);

	StackPtr stack = info.stack;
	info.stack.base = 0;
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
		c.instructions.Add(Instruction(InstructionType::Ret));
	}

	return c;
}

void FunctionNode::IncludeScan(ParsingInfo& info) {
	if (includeScanned) return;

	node->IncludeScan(info);

	includeScanned = true;
}

Set<ScanType> FunctionNode::Scan(ScanInfoStack& info) const {
	info.Push();

	Symbol::Find(this->func, file);
	info.Get().ret = false;
	info.Get().hasRet = false;
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

	if (!info.Get().ret && !s.ret.IsEmpty()) {
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