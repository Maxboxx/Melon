#include "DoStatement.h"

#include "Kiwi/Old/Kiwi.h"

#include "Melon/Optimizing/OptimizerInstruction.h"

#include "BreakStatement.h"
#include "EmptyStatement.h"
#include "Statements.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

DoStatement::DoStatement(Symbols::Symbol* const scope, const FileInfo& file) : Statement(scope, file) {

}

DoStatement::~DoStatement() {

}

UInt DoStatement::GetSize() const {
	return statements->GetSize();
}

bool DoStatement::IsScope() const {
	return true;
}

CompiledNode DoStatement::Compile(OldCompileInfo& info) {
	CompiledNode compiled;

	List<UInt> jumps;

	UInt top = info.stack.top;

	// Compile content
	for (const OptimizerInstruction& in : statements->Compile(info).instructions) {
		// Check for custom instructions
		if (in.instruction.type != InstructionType::Custom) {
			compiled.instructions.Add(in);
			continue;
		}

		const String type = in.instruction.instructionName;

		// Check for scope wise break
		if (type != BreakStatement::scopeBreakInstName) {
			compiled.instructions.Add(in);
			continue;
		}

		// Handle scope wise break
		if (in.instruction.sizes[0] > 1) {
			OptimizerInstruction inst = in;
			inst.instruction.sizes[0]--;
			compiled.instructions.Add(inst);
		}
		else {
			Instruction jmp = Instruction(InstructionType::Jmp);
			jumps.Add(compiled.instructions.Size());
			compiled.instructions.Add(jmp);
		}
	}

	// Add labels for scope breaks
	if (!jumps.IsEmpty()) {
		Instruction lbl = Instruction::Label(info.label);
		compiled.instructions.Add(lbl);

		for (const UInt jump : jumps) {
			compiled.instructions[jump].instruction.arguments.Add(Argument(ArgumentType::Label, info.label));
		}

		info.label++;
	}

	info.stack.top = top;
	return compiled;
}

void DoStatement::IncludeScan(ParsingInfo& info) {
	statements->IncludeScan(info);
}

ScanResult DoStatement::Scan(ScanInfoStack& info) {
	info->scopeInfo.EnterScope(ScopeInfo::ScopeType::Scope);
	ScanResult result = statements->Scan(info);
	info->scopeInfo.ExitScope();
	return result;
}

NameList DoStatement::FindSideEffectScope(const bool assign) {
	return statements->GetSideEffectScope(assign);
}

Ptr<Statement> DoStatement::Optimize(OptimizeInfo& info) {
	Node::Optimize(statements, info);

	if (IsEmpty(statements) || !statements->HasSideEffects()) {
		info.optimized = true;
		return new EmptyStatement();
	}

	return nullptr;
}

StringBuilder DoStatement::ToMelon(const UInt indent) const {
	StringBuilder sb = "do\n";
	sb += String('\t').Repeat(indent + 1);
	sb += statements->ToMelon(indent + 1);
	sb += "\n";
	sb += String('\t').Repeat(indent);
	sb += "end";
	return sb;
}