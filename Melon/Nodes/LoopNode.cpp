#include "LoopNode.h"

#include "Melon/Parsing/Parser.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

LoopNode::LoopNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

LoopNode::~LoopNode() {

}

CompiledNode LoopNode::Compile(CompileInfo& info) {
	CompiledNode c;

	Array<UInt> segmentLabels{segments.Size() + 1};
	List<Pair<UInt, UInt>> jumps;
	Long stackTop = info.stack.top;

	for (UInt i = 0; i < segments.Size(); i++) {
		LoopSegment seg = segments[i];
		UInt nextTrue = i + 1;
		UInt nextFalse = i + 1;
		bool isLast = i >= segments.Size() - 1;

		if (!isLast) {
			if (!segments[i + 1].also) {
				while (nextTrue < segments.Size() && !segments[nextTrue].also) {
					nextTrue++;
				}

				while (nextTrue < segments.Size() && segments[nextTrue].also) {
					nextTrue++;
				}
			}
			else {
				while (nextFalse < segments.Size() && segments[nextFalse].also) {
					nextFalse++;
				}
			}
		}

		bool needsLabel = false;

		for (const Pair<UInt, UInt>& pair : jumps) {
			if (pair.value == i) {
				needsLabel = true;
				break;
			}
		}

		if (needsLabel) {
			Instruction lbl = Instruction::Label(info.label);
			segmentLabels[i] = info.label++;
			c.instructions.Add(lbl);
		}

		Long stack = 0;
		UInt loopEndLbl = 0;
		UInt loopEndJmp = 0;
		UInt loopLbl = 0;

		if (seg.type == LoopType::If) {
			CompiledNode cond = seg.condition->Compile(info);
			c.AddInstructions(cond.instructions);

			Instruction eq = Instruction(InstructionType::Eq, 1);
			eq.arguments.Add(cond.argument);
			eq.arguments.Add(Argument(0));
			jumps.Add(Pair<UInt, UInt>(c.instructions.Size(), nextFalse));
			c.instructions.Add(eq);
		}
		else if (seg.type == LoopType::While) {
			if (!isLast) {
				info.stack.Push(1);
				stack = info.stack.top;

				Instruction mov = Instruction(InstructionType::Mov, 1);
				mov.arguments.Add(Argument(MemoryLocation(info.stack.Offset(stack))));
				mov.arguments.Add(Argument(0));
				c.instructions.Add(mov);
			}

			Instruction lbl = Instruction::Label(info.label);
			loopLbl = info.label++;
			c.instructions.Add(lbl);

			CompiledNode cond = seg.condition->Compile(info);
			c.AddInstructions(cond.instructions);

			Instruction eq = Instruction(InstructionType::Eq, 1);
			eq.arguments.Add(cond.argument);
			eq.arguments.Add(Argument(0));

			if (!isLast)
				loopEndJmp = c.instructions.Size();
			else
				jumps.Add(Pair<UInt, UInt>(c.instructions.Size(), segments.Size()));

			c.instructions.Add(eq);

			if (!isLast) {
				Instruction mov = Instruction(InstructionType::Mov, 1);
				mov.arguments.Add(Argument(MemoryLocation(info.stack.Offset(stack))));
				mov.arguments.Add(Argument(1));
				c.instructions.Add(mov);
			}
		}

		if (seg.type == LoopType::While) {
			for (const OptimizerInstruction& in : seg.statements->Compile(info).instructions) {
				if (in.instruction.type == InstructionType::Custom) {
					const String type = in.instruction.instructionName;

					if (
						type == "abort" ||
						type == "breaktrue" ||
						type == "breakfalse" ||
						type == "continue"
						) {
						if (in.instruction.sizes[0] > 1) {
							OptimizerInstruction inst = in;
							inst.instruction.sizes[0]--;
							c.instructions.Add(inst);
						}
						else if (type == "abort") {
							Instruction jmp = Instruction(InstructionType::Jmp);
							jumps.Add(Pair<UInt, UInt>(c.instructions.Size(), segments.Size()));
							c.instructions.Add(jmp);
						}
						else if (type == "breaktrue") {
							Instruction jmp = Instruction(InstructionType::Jmp);
							jumps.Add(Pair<UInt, UInt>(c.instructions.Size(), isLast ? segments.Size() : nextTrue));
							c.instructions.Add(jmp);
						}
						else if (type == "breakfalse") {
							Instruction jmp = Instruction(InstructionType::Jmp);
							jumps.Add(Pair<UInt, UInt>(c.instructions.Size(), isLast ? segments.Size() : nextFalse));
							c.instructions.Add(jmp);
						}
						else if (type == "continue") {
							Instruction jmp = Instruction(InstructionType::Jmp);
							jmp.arguments.Add(Argument(ArgumentType::Label, loopLbl));
							c.instructions.Add(jmp);
						}
						else {
							c.instructions.Add(in);
						}
					}
					else {
						c.instructions.Add(in);
					}
				}
				else {
					c.instructions.Add(in);
				}
			}
		}
		else {
			c.AddInstructions(seg.statements->Compile(info).instructions);
		}

		if (seg.type == LoopType::While) {
			Instruction jmp = Instruction(InstructionType::Jmp);
			jmp.arguments.Add(Argument(ArgumentType::Label, loopLbl));
			c.instructions.Add(jmp);

			info.stack.Pop(1);

			if (!isLast) {
				Instruction lbl = Instruction::Label(info.label);
				loopEndLbl = info.label++;
				c.instructions[loopEndJmp].instruction.arguments.Add(Argument(ArgumentType::Label, loopEndLbl));
				c.instructions.Add(lbl);

				if (nextTrue == i + 1) {
					Instruction eq = Instruction(InstructionType::Eq, 1);
					eq.arguments.Add(Argument(MemoryLocation(info.stack.Offset(stack))));
					eq.arguments.Add(Argument(0));
					jumps.Add(Pair<UInt, UInt>(c.instructions.Size(), nextFalse));
					c.instructions.Add(eq);
				}
				else if (nextFalse == i + 1) {
					Instruction ne = Instruction(InstructionType::Ne, 1);
					ne.arguments.Add(Argument(MemoryLocation(info.stack.Offset(stack))));
					ne.arguments.Add(Argument(0));
					jumps.Add(Pair<UInt, UInt>(c.instructions.Size(), nextTrue));
					c.instructions.Add(ne);
				}
				else {
					Instruction ne = Instruction(InstructionType::Ne, 1);
					ne.arguments.Add(Argument(MemoryLocation(info.stack.Offset(stack))));
					ne.arguments.Add(Argument(0));
					jumps.Add(Pair<UInt, UInt>(c.instructions.Size(), nextTrue));
					c.instructions.Add(ne);

					Instruction jmp = Instruction(InstructionType::Jmp);
					jumps.Add(Pair<UInt, UInt>(c.instructions.Size(), nextFalse));
					c.instructions.Add(jmp);
				}
			}
		}
		else if (!isLast) {
			if (nextTrue != i + 1 && (seg.type == LoopType::If || seg.type == LoopType::None)) {
				Instruction jmp = Instruction(InstructionType::Jmp);
				jumps.Add(Pair<UInt, UInt>(c.instructions.Size(), nextTrue));
				c.instructions.Add(jmp);
			}
		}

		info.stack.top = stackTop;
	}

	Instruction endLbl = Instruction::Label(info.label);
	segmentLabels[segmentLabels.Size() - 1] = info.label++;
	c.instructions.Add(endLbl);

	for (const Pair<UInt, UInt>& pair : jumps) {
		c.instructions[pair.key].instruction.arguments.Add(Argument(ArgumentType::Label, segmentLabels[pair.value]));
	}

	return c;
}

void LoopNode::IncludeScan(ParsingInfo& info) {
	if (includeScanned) return;

	for (LoopSegment& segment : segments) {
		if (segment.type != LoopType::None) {
			segment.condition->IncludeScan(info);
		}

		segment.statements->IncludeScan(info);
	}

	includeScanned = true;
}

bool LoopNode::WillASegmentRun() const {
	bool hasPlainAlso = false;
	bool hasPlainElse = false;

	for (UInt i = 0; i < segments.Size(); i++) {
		if (segments[i].type == LoopType::None) {
			if (segments[i].also) {
				hasPlainAlso = true;
			}
			else {
				hasPlainElse = true;
			}
		}
		else if (i > 0 && (segments[i].type == LoopType::While || segments[i].type == LoopType::While)) {
			return false;
		}
	}

	return hasPlainElse;
}

LoopNode::LoopScanInfo LoopNode::ScanSetup(ScanInfo& info) const {
	LoopScanInfo loopInfo;
	loopInfo.init = info.init;

	if (loopInfo.init) {
		loopInfo.unassignedVarsStart = info.symbol.GetUnassignedVars();
		loopInfo.canBeAssigned = true;
	}

	return loopInfo;
}

void LoopNode::ScanFirstPostContents(LoopScanInfo& loopInfo, ScanInfo& info) const {
	if (loopInfo.init && loopInfo.canBeAssigned) {
		bool hasPlainAlso = false;

		for (UInt i = 1; i < segments.Size(); i++) {
			if (segments[i].also && segments[i].type == LoopType::None) {
				hasPlainAlso = true;
				break;
			}
		}

		if (segments[0].type == LoopType::For || segments[0].type == LoopType::While) {
			for (const Scope& var : loopInfo.unassignedVarsStart) {
				loopInfo.unassignedVarsAlso.Add(var);
			}
			
			if (!hasPlainAlso) {
				for (const Scope& var : info.symbol.GetUnassignedVars()) {
					loopInfo.unassignedVars.Add(var);
				}

				loopInfo.canBeAssigned = false;
			}
			else {
				loopInfo.checkAlso = true;
			}
		}
		else {
			for (const Scope& var : info.symbol.GetUnassignedVars()) {
				loopInfo.unassignedVarsAlso.Add(var);

				if (!hasPlainAlso) {
					loopInfo.unassignedVars.Add(var);
					loopInfo.canBeAssigned = false;
				}
				else {
					loopInfo.checkAlso = true;
				}
			}
		}

		if (loopInfo.canBeAssigned) {
			loopInfo.canBeAssigned = WillASegmentRun();
		}
	}
}

void LoopNode::ScanPreContents(LoopScanInfo& loopInfo, ScanInfo& info, const LoopSegment& segment) const {
	if (loopInfo.init && loopInfo.canBeAssigned && (!segment.also || loopInfo.checkAlso)) {
		info.init = true;

		for (const Scope& var : loopInfo.unassignedVarsStart) {
			info.symbol.Get(var, FileInfo()).sign = false;
		}
	}
}

void LoopNode::ScanPostContents(LoopScanInfo& loopInfo, ScanInfo& info, const LoopSegment& segment) const {
	if (loopInfo.init && loopInfo.canBeAssigned && (!segment.also || loopInfo.checkAlso)) {
		if (segment.also) {
			for (const Scope& var : info.symbol.GetUnassignedVars()) {
				if (loopInfo.unassignedVarsAlso.Contains(var)) {
					loopInfo.unassignedVars.Add(var);
				}
			}
		}
		else {
			for (const Scope& var : info.symbol.GetUnassignedVars()) {
				loopInfo.unassignedVars.Add(var);
			}
		}
	}
}

void LoopNode::ScanCleanup(LoopScanInfo& loopInfo, ScanInfo& info) const {
	if (loopInfo.init) {
		for (const Scope& var : loopInfo.unassignedVarsStart) {
			info.symbol.Get(var, FileInfo()).sign = loopInfo.canBeAssigned;
		}

		for (const Scope& var : loopInfo.unassignedVars) {
			info.symbol.Get(var, FileInfo()).sign = false;
		}

		if (!loopInfo.unassignedVars.IsEmpty()) {
			info.init = true;
		}
	}
}

Set<ScanType> LoopNode::Scan(ScanInfo& info) const {
	Set<ScanType> scanSet = Set<ScanType>();
	LoopScanInfo loopInfo = ScanSetup(info);

	for (UInt i = 0; i < segments.Size(); i++) {
		if (segments[i].type != LoopType::None) {
			for (const ScanType type : segments[i].condition->Scan(info)) {
				scanSet.Add(type);

				if (info.init && type == ScanType::Self && !info.symbol.IsAssigned()) {
					ErrorLog::Error(CompileError(CompileError::SelfInit, segments[i].condition->file));
				}
			}
		}

		ScanPreContents(loopInfo, info, segments[i]);

		for (const ScanType type : segments[i].statements->Scan(info)) {
			scanSet.Add(type);

			if (info.init && type == ScanType::Self && !info.symbol.IsAssigned()) {
				ErrorLog::Error(CompileError(CompileError::SelfInit, segments[i].statements->file));
			}
		}

		if (i == 0) {
			ScanFirstPostContents(loopInfo, info);
		}
		else {
			ScanPostContents(loopInfo, info, segments[i]);
		}
	}

	ScanCleanup(loopInfo, info);

	return scanSet;
}

Mango LoopNode::ToMango() const {
	Mango mango = Mango("loop", MangoType::List);

	for (UInt i = 0; i < segments.Size(); i++) {
		String name = "";

		if (segments[i].type == LoopType::If)
			name = "if";
		else if (segments[i].type == LoopType::While)
			name = "while";
		else if (segments[i].type == LoopType::For)
			name = "for";

		if (i > 0) {
			if (segments[i].also)
				name = "also" + name;
			else
				name = "else" + name;
		}

		Mango s = Mango(name, MangoType::Map);

		if (segments[i].type == LoopType::If || segments[i].type == LoopType::While) {
			s.Add("condition", segments[i].condition->ToMango());
		}

		s.Add("content", segments[i].statements->ToMango());

		mango.Add(s);
	}

	return mango;
}