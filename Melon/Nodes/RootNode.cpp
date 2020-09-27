#include "RootNode.h"

#include "StructNode.h"
#include "StatementsNode.h"

#include "Melon/Parsing/Parser.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

RootNode::RootNode() : Node(ScopeList(), FileInfo()) {

}

RootNode::~RootNode() {

}

CompiledNode RootNode::Compile(CompileInfo& info) {
	CompiledNode cn;

	Pointer<StatementsNode> statements = new StatementsNode(ScopeList(), FileInfo());
	statements->statements = nodes;
	UInt size = statements->GetSize();

	if (size > 0) {
		OptimizerInstruction push = Instruction(InstructionType::Push, size);
		push.important = true;
		cn.instructions.Add(push);
	}

	info.stack.PushFrame(size);

	for (const NodePtr& node : nodes) {
		for (const OptimizerInstruction& instruction : node->Compile(info).instructions) {
			cn.instructions.Add(instruction);
		}
	}

	if (size > 0) {
		OptimizerInstruction pop = Instruction(InstructionType::Pop, size);
		pop.important = true;
		cn.instructions.Add(pop);
	}

	info.stack.PopFrame(size);

	Instruction in = Instruction(InstructionType::Exit, info.stack.ptrSize);
	in.arguments.Add(Argument(0));
	cn.instructions.Add(in);

	for (const NodePtr& node : funcs) {
		for (const OptimizerInstruction& instruction : node->Compile(info).instructions) {
			cn.instructions.Add(instruction);
		}
	}

	return cn;
}

List<OptimizerInstruction> RootNode::Compile() {
	UByte index = 0;
	CompileInfo info;
	return Compile(info).instructions;
}

void RootNode::IncludeScan(ParsingInfo& info) {
	UInt nodeIndex = 0;
	UInt funcIndex = 0;
	UInt templateIndex = 0;

	Collection<UInt> failedNodes;
	Collection<UInt> failedFuncs;

	do {
		for (UInt i = 0; i < failedNodes.Size();) {
			try {
				nodes[failedNodes[i]]->IncludeScan(info);
				failedNodes.RemoveAt(i);
			}
			catch (IncludeScanError& e) {
				i++;
			}
		}

		for (UInt i = 0; i < failedFuncs.Size();) {
			try {
				funcs[failedFuncs[i]]->IncludeScan(info);
				failedFuncs.RemoveAt(i);
			}
			catch (IncludeScanError& e) {
				i++;
			}
		}

		for (; nodeIndex < nodes.Size(); nodeIndex++) {
			try {
				nodes[nodeIndex]->IncludeScan(info);
			}
			catch (IncludeScanError& e) {
				failedNodes.Add(nodeIndex);
			}
		}

		for (; funcIndex < funcs.Size(); funcIndex++) {
			try {
				funcs[funcIndex]->IncludeScan(info);
			}
			catch (IncludeScanError& e) {
				failedFuncs.Add(nodeIndex);
			}
		}

		for (; templateIndex < Symbol::templateSymbols.Size(); templateIndex++) {
			Tuple<Symbol, List<ScopeList>> templateInfo = Symbol::FindTemplateArgs(Symbol::templateSymbols[templateIndex]);
			Scope templateScope = templateInfo.value1.scope.Last().Copy();
			templateScope.types = templateInfo.value2;
			templateScope.variant = nullptr;

			if (Symbol::Contains(templateInfo.value1.scope.Pop().Add(templateScope))) continue;

			Scope last = templateInfo.value1.scope.Last();
			last.variant = nullptr;
			last.types   = nullptr;

			Symbol templateSym = Symbol::Find(templateInfo.value1.scope.Pop().Add(last), file);
			templateSym.templateVariants.Add(Symbol(templateInfo.value1.type));

			Symbol& s = templateSym.templateVariants.Last();

			templateInfo.value1.SpecializeTemplate(s, templateInfo.value2, info);

			if (s.type == SymbolType::Struct) {
				Pointer<StructNode> sn = new StructNode(Symbol::templateSymbols[templateIndex].scope, Symbol::templateSymbols[templateIndex].file);
				sn->name = s.scope.Last();
				sn->symbol = s;
				
				for (const ScopeList& var : s.arguments) {
					sn->vars.Add(var.Last());
				}

				nodes.Add(sn);
			}
		}
	}
	while (
		nodeIndex < nodes.Size() ||
		funcIndex < funcs.Size() ||
		!failedNodes.IsEmpty() ||
		!failedFuncs.IsEmpty() ||
		templateIndex < Symbol::templateSymbols.Size()
	);
}

Set<ScanType> RootNode::Scan(ScanInfoStack& info) {
	Set<ScanType> scanSet = Set<ScanType>();

	for (const NodePtr& node : nodes) {
		for (const ScanType type : node->Scan(info)) {
			scanSet.Add(type);
		}
	}

	for (const NodePtr& node : funcs) {
		for (const ScanType type : node->Scan(info)) {
			scanSet.Add(type);
		}
	}

	return scanSet;
}

void RootNode::Scan() {
	ScanInfoStack info;
	info.Get().scopeInfo.type = ScopeInfo::ScopeType::Main;
	Scan(info);
}

Mango RootNode::ToMango() const {
	Mango mango = Mango("ast", MangoType::List);

	for (const NodePtr& node : nodes) {
		mango.Add(node->ToMango());
	}

	return mango;
}

String RootNode::ToString() const {
	return Mango::Encode(ToMango(), true);
}