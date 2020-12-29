#include "SafeUnwrapEndNode.h"

#include "MemoryNode.h"

#include "Kiwi/Kiwi.h"

#include "Melon/Optimizing/OptimizerInstruction.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

String SafeUnwrapEndNode::jumpInstName = "?jmp";

SafeUnwrapEndNode::SafeUnwrapEndNode(Symbol* const scope, const FileInfo& file) : Node(scope, file) {

}

SafeUnwrapEndNode::~SafeUnwrapEndNode() {

}

TypeSymbol* SafeUnwrapEndNode::Type() const  {
	Scope scope = Scope::Optional;
	scope.types = List<ScopeList>();
	scope.types.Get().Add(node->Type()->AbsoluteName());

	/* TODO: node
	return Symbols::Find(ScopeList().Add(scope), file).scope;
	*/

	return nullptr;
}

CompiledNode SafeUnwrapEndNode::Compile(CompileInfo& info)  {
	CompiledNode cn = node->Compile(info);

	info.stack.PushExpr(cn.size + 1, cn);
	Argument arg = Argument(MemoryLocation(info.stack.Offset()));

	Instruction mov1 = Instruction(InstructionType::Mov, 1);
	mov1.arguments.Add(arg);
	mov1.arguments.Add(Argument(1));
	cn.instructions.Add(mov1);

	Pointer<MemoryNode> sn1 = new MemoryNode(arg.mem.offset + 1);
	sn1->type = node->Type()->AbsoluteName();

	Pointer<MemoryNode> sn2 = new MemoryNode(cn.argument.mem);
	sn2->type = node->Type()->AbsoluteName();

	cn.AddInstructions(CompileAssignment(sn1, sn2, info, file).instructions);

	UInt jmpIndex = cn.instructions.Size();
	cn.instructions.Add(Instruction(InstructionType::Jmp));

	for (OptimizerInstruction& inst : cn.instructions) {
		if (inst.instruction.type == InstructionType::Custom) {
			if (inst.instruction.instructionName == jumpInstName) {
				inst.instruction.type = InstructionType::Eq;
				inst.instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
			}
		}
	}

	cn.instructions.Add(Instruction::Label(info.label++));

	Instruction mov2 = Instruction(InstructionType::Mov, 1);
	mov2.arguments.Add(arg);
	mov2.arguments.Add(Argument(0));
	cn.instructions.Add(mov2);

	cn.instructions[jmpIndex].instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
	cn.instructions.Add(Instruction::Label(info.label++));

	cn.size++;
	cn.argument = arg;
	info.stack.Pop(cn.size);

	return cn;
}

void SafeUnwrapEndNode::IncludeScan(ParsingInfo& info)  {
	node->IncludeScan(info);

	const ScopeList nodeType = node->Type()->AbsoluteName();

	if (nodeType == ScopeList::undefined) {
		throw IncludeScanError();
	}

	Scope type = Scope::Optional;
	type.types = List<ScopeList>();
	type.types.Get().Add(nodeType);

	/* TODO: node
	Symbols::TemplateSymbol ts;
	ts.type = ScopeList().Add(type);
	ts.scope = scope;
	ts.file = file;
	Symbols::templateSymbols.Add(ts);
	*/
}

Set<ScanType> SafeUnwrapEndNode::Scan(ScanInfoStack& info)  {
	return node->Scan(info);
}

ScopeList SafeUnwrapEndNode::FindSideEffectScope(const bool assign) {
	return node->GetSideEffectScope(assign);
}

NodePtr SafeUnwrapEndNode::Optimize(OptimizeInfo& info) {
	if (NodePtr n = node->Optimize(info)) node = n;

	return nullptr;
}

StringBuilder SafeUnwrapEndNode::ToMelon(const UInt indent) const  {
	return StringBuilder();
}
