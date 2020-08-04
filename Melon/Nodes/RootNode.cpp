#include "RootNode.h"

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

	for (const NodePtr& node : nodes) {
		for (const OptimizerInstruction& instruction : node->Compile(info).instructions) {
			cn.instructions.Add(instruction);
		}
	}

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

}

Set<ScanType> RootNode::Scan(ScanInfo& info) const {
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

void RootNode::Scan() const {
	ScanInfo info;
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