#include "KiwiOptimizer.h"

#include "OptimizerInstruction.h"

#include "Boxx/Math.h"
#include "Boxx/Set.h"
#include "Boxx/Map.h"
#include "Boxx/ReplacementMap.h"

using namespace Boxx;
using namespace KiwiOld;
using namespace Melon::Optimizing;

List<Instruction> KiwiOptimizer::Optimize(const List<OptimizerInstruction>& instructionList, const UInt loops) {
	// Split instructions in smaller segments
	List<Pair<List<OptimizerInstruction>, List<OptimizerInstruction>>> segments = Split(instructionList);

	// Perform optimization loops
	for (UInt i = 0; i < loops; i++) {
		bool optimized = false;

		// Optimize each segment
		for (Pair<List<OptimizerInstruction>, List<OptimizerInstruction>>& segment : segments) {
			// Copy instructions
			List<Instruction> copy{segment.value.Count()};

			for (const OptimizerInstruction& inst : segment.value) {
				copy.Add(inst.instruction.Copy());
			}

			// Optimize
			CombineComp(segment.value);
			RemoveComps(segment.value);

			CombinePushPop(segment.value);

			RemoveDuplicates(segment.value);
			CombineDuplicates(segment.value);
			ReduceMov(segment.value);

			RearrangeJumps(segment.value);
			RemoveUnusedLabels(segment.value);

			RenameRegisters(segment.value);

			// Check if the instructions have been changed
			if (copy.Count() != segment.value.Count()) {
				optimized = true;
			}
			else for (UInt i = 0; i < copy.Count(); i++) {
				if (copy[i] != segment.value[i].instruction) {
					optimized = true;
					break;
				}
			}
		}

		if (!optimized) {
			break;
		}
	}

	// Perform optimization cleanup
	if (loops > 0) {
		ReplacementMap<String> labels;
		UInt labelIndex = 0;

		for (Pair<List<OptimizerInstruction>, List<OptimizerInstruction>>& segment : segments) {
			CombineMov(segment.value);
			RenameLabels(segment.value, labels, labelIndex);
		}
	}

	// Convert optimized segments to list of instructions
	List<Instruction> instructions;

	for (const Pair<List<OptimizerInstruction>, List<OptimizerInstruction>>& segment : segments) {
		for (const OptimizerInstruction& inst : segment.key) {
			instructions.Add(inst.instruction);
		}

		for (const OptimizerInstruction& inst : segment.value) {
			instructions.Add(inst.instruction);
		}
	}

	return instructions;
}

List<Pair<List<OptimizerInstruction>, List<OptimizerInstruction>>> KiwiOptimizer::Split(const List<OptimizerInstruction>& instructions) {
	List<Pair<List<OptimizerInstruction>, List<OptimizerInstruction>>> segments;

	Pair<List<OptimizerInstruction>, List<OptimizerInstruction>> segment;

	// Split instructions
	for (UInt i = 0; i < instructions.Count(); i++) {
		bool start = false;
		OptimizerInstruction inst = instructions[i];

		// Split on new kiwi segment
		if (inst.instruction.type == InstructionType::Function || inst.instruction.type == InstructionType::Code || inst.instruction.type == InstructionType::Static) {
			start = true;
		}

		// Split on push/pop
		else if (inst.instruction.type == InstructionType::Sub && inst.instruction.arguments[0].type == ArgumentType::Register && inst.instruction.arguments[0].reg.type == RegisterType::Stack) {
			start = true;
		}
		else if (inst.instruction.type == InstructionType::Add && inst.instruction.arguments[0].type == ArgumentType::Register && inst.instruction.arguments[0].reg.type == RegisterType::Stack) {
			start = true;
		}

		// Add instruction to segment
		if (start) {
			if (!segment.value.IsEmpty()) {
				segments.Add(segment);
				segment = Pair<List<OptimizerInstruction>, List<OptimizerInstruction>>();
			}

			segment.key.Add(inst);
		}
		else {
			segment.value.Add(inst);
		}
	}

	segments.Add(segment);

	return segments;
}

void KiwiOptimizer::ReduceMov(List<OptimizerInstruction>& instructions) {
	for (UInt i = 0; i < instructions.Count(); i++) {
		if (instructions[i].IsStackBreak()) continue;

		const OptimizerInstruction inst = instructions[i];

		// Check if instruction has three arguments
		if (inst.instruction.arguments.Count() == 3) {
			ReduceMov3(instructions, i);
		}
		// Check if instruction has two arguments
		else if (inst.instruction.arguments.Count() == 2) {
			ReduceMov2(instructions, i);
		}
	}
}

void KiwiOptimizer::ReduceMov3(List<OptimizerInstruction>& instructions, UInt& i) {
	const OptimizerInstruction inst = instructions[i];

	// Simplify instruction
	if (inst.instruction.arguments[0] == inst.instruction.arguments[2] && inst.instruction.sizes[0] == inst.instruction.sizes[2]) {
		instructions[i].instruction.arguments.RemoveAt(2);
		i--;
	}
	// Check if number of moves can be reduced
	else for (UInt j = i; j < instructions.Count(); j++) {
		if (instructions[j].IsStackBreak()) break;
		if (inst.important) break;

		// Get next assignment of the argument
		UInt nextAssign = NextAssign(instructions, i, inst.instruction.arguments[2]);

		if (nextAssign > instructions.Count()) {
			nextAssign = instructions.Count();
		}

		// Break if the argument is used before next assignment
		if (UseCount(instructions, i, nextAssign, inst.instruction.arguments[2]) > 1) {
			break;
		}

		// Reduce moves
		for (UInt u = j + 1; u < nextAssign; u++) {
			if (instructions[u].IsStackBreak()) break;

			// Update inst argument and remove mov instruction
			if (instructions[u].instruction.type == InstructionType::Mov) {
				if (instructions[u].instruction.arguments[1] == inst.instruction.arguments[2]) {
					instructions[i].instruction.arguments[2] = instructions[u].instruction.arguments[0];
					instructions[i].instruction.sizes[2] = instructions[u].instruction.sizes[0];
					instructions[i].instruction.signs[2] = instructions[u].instruction.signs[0];
					instructions[i].important = instructions[u].important;

					instructions.RemoveAt(u);
					i--;
					j--;
					break;
				}
			}
		}
	}
}

void KiwiOptimizer::ReduceMov2(List<OptimizerInstruction>& instructions, UInt& i) {
	const OptimizerInstruction inst = instructions[i];

	// Simplify instruction
	if (inst.instruction.type == InstructionType::Mov && inst.instruction.arguments[0] == inst.instruction.arguments[1]) {
		instructions.RemoveAt(i);
		i--;
		return;
	}
	// Break if instruction is not assignment
	else if (!inst.IsAssignment()) {
		return;
	}

	// Check if moves can be reduced
	for (UInt j = i; j < instructions.Count(); j++) {
		if (instructions[j].IsStackBreak()) break;
		if (instructions[i].important) break;

		// Get the next assignment of the argument
		UInt nextAssign = NextAssign(instructions, i, instructions[i].instruction.arguments[0]);

		if (nextAssign > instructions.Count()) {
			nextAssign = instructions.Count();
		}

		// Break if the argument is used before the next assignment
		if (UseCount(instructions, i, nextAssign, instructions[i].instruction.arguments[0]) > 1) {
			break;
		}

		// Reduce moves
		for (UInt u = j + 1; u < nextAssign; u++) {
			if (instructions[u].IsStackBreak()) break;

			// Update inst argument and remove mov instruction
			if (instructions[u].instruction.type == InstructionType::Mov) {
				if (instructions[u].instruction.arguments[1] == instructions[i].instruction.arguments[0]) {
					instructions[i].instruction.arguments[0] = instructions[u].instruction.arguments[0];
					instructions[i].instruction.sizes[0] = instructions[u].instruction.sizes[0];
					instructions[i].instruction.signs[0] = instructions[u].instruction.signs[0];
					instructions[i].instruction.signs[0] = instructions[u].instruction.signs[0];
					instructions[i].important = instructions[u].important;

					instructions.RemoveAt(u);
					j--;
					break;
				}
			}
		}
	}
}

void KiwiOptimizer::CombineMov(List<OptimizerInstruction>& instructions) {
	for (UInt i = 0; i < instructions.Count(); i++) {
		const OptimizerInstruction inst = instructions[i];

		if (inst.IsStackBreak()) continue;
		if (inst.instruction.type != InstructionType::Mov) continue;
		if (inst.instruction.sizes[0] != inst.instruction.sizes[1]) continue;
		if (inst.instruction.sizes[0] == KiwiLang::maxSize) continue;

		// Combine memory mov
		if (inst.instruction.arguments[0].type == ArgumentType::Memory && inst.instruction.arguments[1].type == ArgumentType::Memory) {
			CombineMemoryMov(instructions, i);
		}
		// Combine number mov
		else if (inst.instruction.arguments[0].type == ArgumentType::Memory && inst.instruction.arguments[1].type == ArgumentType::Number) {
			CombineNumberMov(instructions, i);
		}
	}
}

void KiwiOptimizer::CombineMemoryMov(List<OptimizerInstruction>& instructions, UInt& i) {
	const OptimizerInstruction inst = instructions[i];

	// Prepare for combine
	const Either<Register, String> memptr1 = inst.instruction.arguments[0].mem.memptr;
	const Either<Register, String> memptr2 = inst.instruction.arguments[1].mem.memptr;

	Int startOffset1 = inst.instruction.arguments[0].mem.offset;
	Int startOffset2 = inst.instruction.arguments[1].mem.offset;
	Int offset1 = startOffset1;
	Int offset2 = startOffset2;
	Byte offsetDir = 0;

	List<OptimizerInstruction> insts;
	List<UByte> sizes;
	List<bool> important;
	UByte totalSize = inst.instruction.sizes[0];
	insts.Add(inst);
	sizes.Add(totalSize);

	// Check if mov can be combined
	for (UInt u = i + 1; u < instructions.Count(); u++) {
		if (instructions[u].IsStackBreak()) break;

		const OptimizerInstruction inst2 = instructions[u];

		// Check for valid mov instruction
		if (inst2.instruction.type != InstructionType::Mov) break;
		if (inst2.instruction.sizes[0] != inst2.instruction.sizes[1] || inst2.instruction.sizes[0] >= KiwiLang::maxSize) break;
		if (inst2.instruction.arguments[0].type != ArgumentType::Memory || inst2.instruction.arguments[1].type != ArgumentType::Memory) break;
		if (inst2.instruction.arguments[0].mem.memptr != memptr1 || inst2.instruction.arguments[1].mem.memptr != memptr2) break;
		if (inst2.instruction.sizes[0] + totalSize > KiwiLang::maxSize) break;

		// Combine negative offset direction
		if (
			offsetDir < 1 &&
			inst2.instruction.arguments[0].mem.offset == offset1 - inst2.instruction.sizes[0] &&
			inst2.instruction.arguments[1].mem.offset == offset2 - inst2.instruction.sizes[0]
		) {
			insts.Add(inst2);
			sizes.Add(inst2.instruction.sizes[0]);
			important.Add(inst2.important);
			totalSize += sizes.Last();
			offset1 -= sizes.Last();
			offset2 -= sizes.Last();
			offsetDir = -1;
		}
		// Combine positive offset direction
		else if (
			offsetDir > -1 &&
			inst2.instruction.arguments[0].mem.offset == offset1 + inst2.instruction.sizes[0] &&
			inst2.instruction.arguments[1].mem.offset == offset2 + inst2.instruction.sizes[0]
		) {
			insts.Add(inst2);
			sizes.Add(inst2.instruction.sizes[0]);
			important.Add(inst2.important);
			totalSize += sizes.Last();
			offset1 += sizes.Last();
			offset2 += sizes.Last();
			offsetDir = 1;
		}
		// Break if instruction can no be combined
		else {
			break;
		}
	}

	// Remove movs if combined size is too large
	while (!KiwiLang::IsValidSize(totalSize)) {
		totalSize -= sizes.Last();

		if (offsetDir < 1) {
			offset1 += sizes.Last();
			offset2 += sizes.Last();
		}

		sizes.RemoveAt(sizes.Count() - 1);
		important.RemoveAt(important.Count() - 1);
		insts.RemoveAt(insts.Count() - 1);
	}

	// Combine movs
	if (insts.Count() != 1) {
		instructions[i].instruction.sizes[0] = totalSize;
		instructions[i].instruction.sizes[1] = totalSize;
		instructions[i].instruction.arguments[0].mem.offset = offsetDir < 1 ? offset1 : startOffset1;
		instructions[i].instruction.arguments[1].mem.offset = offsetDir < 1 ? offset2 : startOffset2;
		instructions[i].important = important.Contains(true);

		for (UInt u = i + 1; u < i + insts.Count(); u++) {
			instructions.RemoveAt(i + 1);
		}
	}
}

void KiwiOptimizer::CombineNumberMov(List<OptimizerInstruction>& instructions, UInt& i) {
	const OptimizerInstruction inst = instructions[i];

	// Prepare combine
	const Either<Register, String> memptr = inst.instruction.arguments[0].mem.memptr;
	Int startOffset = inst.instruction.arguments[0].mem.offset;
	Int offset = startOffset;
	Byte offsetDir = 0;
	Long num = inst.instruction.arguments[1].number;

	List<OptimizerInstruction> insts;
	List<UByte> sizes;
	List<Long> numbers;
	List<bool> important;
	UByte totalSize = inst.instruction.sizes[0];
	insts.Add(inst);
	sizes.Add(totalSize);
	numbers.Add(num);

	// Check if mov can be combined
	for (UInt u = i + 1; u < instructions.Count(); u++) {
		if (instructions[u].IsStackBreak()) break;

		const OptimizerInstruction inst2 = instructions[u];

		// Check for valid mov
		if (inst2.instruction.type != InstructionType::Mov) break;
		if (inst2.instruction.sizes[0] != inst2.instruction.sizes[1] || inst2.instruction.sizes[0] >= KiwiLang::maxSize) break;
		if (inst2.instruction.arguments[0].type != ArgumentType::Memory || inst2.instruction.arguments[1].type != ArgumentType::Number) break;
		if (inst2.instruction.arguments[0].mem.memptr != memptr) break;
		if (inst2.instruction.sizes[0] + totalSize > KiwiLang::maxSize) break;

		// Combine negative offset direction
		if (offsetDir < 1 && inst2.instruction.arguments[0].mem.offset == offset - inst2.instruction.sizes[0]) {
			insts.Add(inst2);
			sizes.Add(inst2.instruction.sizes[0]);
			numbers.Add(inst2.instruction.arguments[1].number);
			important.Add(inst2.important);
			totalSize += sizes.Last();
			offset -= sizes.Last();
			offsetDir = -1;
		}
		// Combine positive offset direction
		else if (offsetDir > -1 && inst2.instruction.arguments[0].mem.offset == offset + inst2.instruction.sizes[0]) {
			insts.Add(inst2);
			sizes.Add(inst2.instruction.sizes[0]);
			numbers.Add(inst2.instruction.arguments[1].number);
			important.Add(inst2.important);
			totalSize += sizes.Last();
			offset += sizes.Last();
			offsetDir = 1;
		}
		// Break if instruction can not be combined
		else {
			break;
		}
	}

	// Remove movs if combined size is too large
	while (!KiwiLang::IsValidSize(totalSize)) {
		totalSize -= sizes.Last();
		sizes.RemoveAt(sizes.Count() - 1);
		insts.RemoveAt(insts.Count() - 1);
		numbers.RemoveAt(numbers.Count() - 1);
		important.RemoveAt(important.Count() - 1);
	}

	// Combine movs
	if (insts.Count() != 1) {
		instructions[i].instruction.sizes[0] = totalSize;
		instructions[i].instruction.sizes[1] = totalSize;
		instructions[i].instruction.arguments[0].mem.offset -= offsetDir < 1 ? totalSize - sizes[0] : 0;
		instructions[i].instruction.arguments[1].number = 0;
		instructions[i].important = important.Contains(true);

		UByte size = totalSize;

		for (UInt u = i; u < i + insts.Count(); u++) {
			size -= sizes[0];

			ULong numMask = 0;

			switch (sizes[0]) {
				case 8: numMask = Math::ULongMax();  break;
				case 4: numMask = Math::UIntMax();   break;
				case 2: numMask = Math::UShortMax(); break;
				case 1: numMask = Math::UByteMax();  break;
			}

			sizes.RemoveAt(0);

			if (offsetDir < 1) {
				instructions[i].instruction.arguments[1].number |= (numbers[0] & numMask) << size * 8;
			}
			else {
				instructions[i].instruction.arguments[1].number |= (numbers[0] & numMask) << (totalSize - size - 1) * 8;
			}

			numbers.RemoveAt(0);

			if (u != i) instructions.RemoveAt(i + 1);
		}
	}
}

void KiwiOptimizer::RemoveDuplicates(List<OptimizerInstruction>& instructions) {
	for (UInt i = 0; i < instructions.Count(); i++) {
		if (instructions[i].IsStackBreak()) continue;

		const OptimizerInstruction inst = instructions[i];

		// Check for mov instructions
		if (inst.instruction.type == InstructionType::Mov && inst.instruction.arguments.Count() == 2) {
			const Argument arg1 = inst.instruction.arguments[0];
			const Argument arg2 = inst.instruction.arguments[1];

			UInt index = NextAssign(instructions, i, arg1);

			// Check for duplicate movs
			while (index < instructions.Count()) {
				if (instructions[index].IsStackBreak()) break;

				const OptimizerInstruction in = instructions[index];

				// Remove duplicate
				if (in.instruction.type == InstructionType::Mov && in.instruction.arguments.Count() == 2) {
					if (in.instruction.arguments[0] == arg1 && in.instruction.arguments[1] == arg2) {
						if (index < NextAssign(instructions, i, arg2)) {
							instructions.RemoveAt(index);
						}
					}
				}

				index = NextAssign(instructions, index, arg1);
			}
		}
	}
}

void KiwiOptimizer::CombineDuplicates(List<OptimizerInstruction>& instructions) {
	for (UInt i = 0; i < instructions.Count(); i++) {
		if (instructions[i].IsStackBreak()) continue;

		CombineInfo info;
		info.inst = instructions[i];

		// Break if instruction is not a mov with register
		if (info.inst.instruction.type != InstructionType::Mov) continue;
		if (!IsRegister(info.inst.instruction.arguments[0])) continue;

		// Get mov arguments
		info.arg1 = info.inst.instruction.arguments[0];
		info.arg2 = info.inst.instruction.arguments[1];

		// Get next assignment of arguments
		info.nextAssign1 = NextAssign(instructions, i, info.arg1);
		info.nextAssign2 = NextAssign(instructions, i, info.arg2);

		// Combine duplicate assigns
		for (UInt u = i + 1; u < info.nextAssign1; u++) {
			if (instructions[u].IsStackBreak()) break;

			info.nextReplacement = nullptr;
			info.remove = nullptr;

			UpdateCombineInfo(instructions, info, i, u);

			ReplaceInstructionRegisters(instructions[u].instruction, info.replacement);

			if (info.remove) {
				info.replacement.Remove(*info.remove);
			}

			if (info.nextReplacement) {
				info.replacement.Add(info.nextReplacement->value1, info.nextReplacement->value2);
			}
		}
	}
}

void KiwiOptimizer::UpdateCombineInfo(List<OptimizerInstruction>& instructions, CombineInfo& info, const UInt initialIndex, UInt& index) {
	bool isRegAndArg = IsRegister(instructions[index].instruction.arguments[0]) && instructions[index].instruction.arguments[1] == info.arg2;
	bool isMov       = instructions[index].instruction.type == InstructionType::Mov;

	// Get assign info
	if (!info.inst.important && index < info.nextAssign2 && isMov && isRegAndArg) {
		const Argument arg = instructions[index].instruction.arguments[0];
		const UInt assign = NextAssign(instructions, index, arg);

		// Find duplicate
		if (info.nextAssign1 < assign && NextRegisterAccess(instructions, initialIndex, arg.reg) >= assign) {
			info.nextAssign1 = assign;

			for (UInt p = initialIndex + 1; p < index; p++) {
				for (Argument& argument : instructions[p].instruction.arguments) {
					if (argument == info.arg1) {
						argument = arg;
					}
					else if (argument.type == ArgumentType::Memory && argument.mem.memptr == info.arg1.reg) {
						argument.mem.memptr = arg.reg;
					}
				}
			}

			info.replacement.Add(info.arg1.reg, arg.reg);
			info.arg1 = info.replacement.GetChain(info.arg1.reg);
			instructions[initialIndex].instruction.arguments[0] = info.arg1;
			instructions.RemoveAt(index);
			index--;
		}
		// Get next replacement
		else {
			info.nextReplacement = Tuple<Register, Register>(arg.reg, info.arg1.reg);
			instructions.RemoveAt(index);
			index--;
		}
	}
	// Remove register if it is assigned a different value
	else if (instructions[index].IsRegisterAssignment()) {
		info.remove = instructions[index].GetAssignedRegister();
	}
}

void KiwiOptimizer::ReplaceInstructionRegisters(Instruction& instruction, const ReplacementMap<Register>& replacement) {
	// Replaces registers in a two argument instruction
	if (instruction.arguments.Count() == 2 && instruction.type != InstructionType::Adr) {
		ReplaceArgumentMemoryRegister(instruction.arguments[0], replacement);
		ReplaceArgumentRegister(instruction.arguments[1], replacement);
	}
	// Replaces registers in a three argument instruction
	else if (instruction.arguments.Count() == 3) {
		ReplaceArgumentRegister(instruction.arguments[0], replacement);
		ReplaceArgumentRegister(instruction.arguments[1], replacement);
		ReplaceArgumentMemoryRegister(instruction.arguments[2], replacement);
	}
}

void KiwiOptimizer::ReplaceArgumentRegister(Argument& argument, const ReplacementMap<Register>& replacement) {
	// Replaces a register argument
	if (argument.type == ArgumentType::Register) {
		argument.reg = replacement.GetChain(argument.reg);
	}
	// Replaces the register of a memory argument
	else if (argument.type == ArgumentType::Memory) {
		if (argument.mem.memptr.IsLeft()) {
			argument.mem.memptr = replacement.GetChain(argument.mem.memptr.GetLeft());
		}
	}
}

void KiwiOptimizer::ReplaceArgumentMemoryRegister(Argument& argument, const ReplacementMap<Register>& replacement) {
	if (argument.type == ArgumentType::Memory) {
		if (argument.mem.memptr.IsLeft()) {
			argument.mem.memptr = replacement.GetChain(argument.mem.memptr.GetLeft());
		}
	}
}

void KiwiOptimizer::RemoveUnused(List<OptimizerInstruction>& instructions) {
	for (UInt i = 0; i < instructions.Count(); i++) {
		if (instructions[i].IsStackBreak()) continue;

		const OptimizerInstruction inst = instructions[i];
	}
}

bool KiwiOptimizer::IsRegister(const Argument& arg) {
	return arg.type == ArgumentType::Register && arg.reg.type == RegisterType::Register;
}

void KiwiOptimizer::CombineComp(List<OptimizerInstruction>& instructions) {
	for (UInt i = 0; i < instructions.Count(); i++) {
		if (instructions[i].IsStackBreak()) continue;

		const OptimizerInstruction inst = instructions[i];

		// Check if instruction is a comp instruction with three arguments
		if (!Instruction::IsComp(inst.instruction.type)) continue;
		if (inst.important) continue;
		if (inst.instruction.arguments.Count() != 3) continue;
		if (i + 1 >= instructions.Count()) continue;

		const OptimizerInstruction inst2 = instructions[i + 1];

		// Check if the next instruction is a comp instruction that uses the result from the previous instruction
		if (!Instruction::IsComp(inst2.instruction.type)) continue;
		if (inst2.instruction.arguments[0] != inst.instruction.arguments[2]) continue;
		if (inst2.instruction.arguments[1].type != ArgumentType::Number) continue;

		// Combine eq instruction
		if (inst2.instruction.type == InstructionType::Eq) {
			if (inst2.instruction.arguments[1].number == 0) {
				instructions[i].instruction.type = OppositeComp(inst.instruction.type);
			}

			instructions[i].instruction.arguments[2] = inst2.instruction.arguments.Count() == 3 ? inst2.instruction.arguments[2] : inst2.instruction.arguments[0];
			instructions.RemoveAt(i + 1);
		}
		// Combine ne instruction
		else if (inst2.instruction.type == InstructionType::Ne) {
			if (inst2.instruction.arguments[1].number == 1) {
				instructions[i].instruction.type = OppositeComp(inst.instruction.type);
			}

			instructions[i].instruction.arguments[2] = inst2.instruction.arguments.Count() == 3 ? inst2.instruction.arguments[2] : inst2.instruction.arguments[0];
			instructions.RemoveAt(i + 1);
		}
	}
}

void KiwiOptimizer::RemoveComps(List<OptimizerInstruction>& instructions) {
	for (UInt i = 0; i < instructions.Count(); i++) {
		if (instructions[i].IsStackBreak()) continue;

		const OptimizerInstruction inst = instructions[i];

		// Check if comp instruction has two number arguments
		if (!Instruction::IsComp(inst.instruction.type)) continue;
		if (inst.important) continue;
		if (inst.instruction.arguments.Count() != 3) continue;
		if (inst.instruction.arguments[0].type != ArgumentType::Number) continue;
		if (inst.instruction.arguments[1].type != ArgumentType::Number) continue;

		// Calculate comp output
		bool output = false;

		switch (inst.instruction.type) {
			case InstructionType::Eq: output = inst.instruction.arguments[0].number == inst.instruction.arguments[1].number; break;
			case InstructionType::Ne: output = inst.instruction.arguments[0].number != inst.instruction.arguments[1].number; break;
			case InstructionType::Lt: output = inst.instruction.arguments[0].number  < inst.instruction.arguments[1].number; break;
			case InstructionType::Gt: output = inst.instruction.arguments[0].number  > inst.instruction.arguments[1].number; break;
			case InstructionType::Le: output = inst.instruction.arguments[0].number <= inst.instruction.arguments[1].number; break;
			case InstructionType::Ge: output = inst.instruction.arguments[0].number >= inst.instruction.arguments[1].number; break;
		}

		// Replace comp with jump
		if (inst.instruction.arguments[2].type == ArgumentType::Label) {
			if (output) {
				Instruction jmp = Instruction(InstructionType::Jmp);
				jmp.arguments.Add(inst.instruction.arguments[2]);
				instructions[i].instruction = jmp;
			}
			else {
				instructions.RemoveAt(i);
				i--;
			}
		}
		// Replace comp with mov
		else {
			Instruction mov = Instruction(InstructionType::Mov, inst.instruction.sizes[2]);
			mov.arguments.Add(inst.instruction.arguments[2]);
			mov.arguments.Add(Argument(output ? 1 : 0));
			instructions[i].instruction = mov;
		}
	}
}

void KiwiOptimizer::InsertEmptyPushPop(List<OptimizerInstruction>& instructions) {
	instructions.Insert(0, Instruction(InstructionType::Push, 0));

	for (UInt i = 0; i < instructions.Count(); i++) {
		if (instructions[i].IsLabelOrCall() || instructions[i].IsJumpOrRet() || instructions[i].CanJump()) {
			instructions.Insert(i + 1, Instruction(InstructionType::Push, 0));
			instructions.Insert(i, Instruction(InstructionType::Pop, 0));
			i++;
		}
	}

	instructions.Add(Instruction(InstructionType::Pop, 0));
}

void KiwiOptimizer::CombinePushPop(List<OptimizerInstruction>& instructions) {
	// Insert empty push and pop instructions
	InsertEmptyPushPop(instructions);

	for (UInt i = 0; i < instructions.Count(); i++) {
		const OptimizerInstruction inst = instructions[i];
		
		// Check if instruction is push or pop
		if (!inst.IsPushPop()) continue;
		if (inst.instruction.arguments.Count() != 0) continue;

		// Get push number
		Long num = inst.instruction.type == InstructionType::Push ? inst.instruction.sizes[0] : -inst.instruction.sizes[0];
		bool found = false;

		// Find next push or pop
		for (UInt u = i + 1; u < instructions.Count(); u++) {
			if (instructions[u].IsLabelOrCall()) break;
			if (instructions[u].IsJumpOrRet())   break;
			if (instructions[u].CanJump())       break;

			const OptimizerInstruction inst2 = instructions[u];

			// Check if instruction is push or pop
			if (!inst2.IsPushPop()) continue;
			if (inst2.instruction.arguments.Count() != 0) continue;

			// Get push number
			Long num2 = inst2.instruction.type == InstructionType::Push ? inst2.instruction.sizes[0] : -inst2.instruction.sizes[0];
			Long combinedNum = num + num2;
			found = true;

			// Push/pop == 0
			if (combinedNum == 0) {
				OffsetStackPointer(instructions, i + 1, u - 1, (UInt)num2);
				instructions.RemoveAt(u);
				instructions.RemoveAt(i);
				i--;
			}
			// Push
			else if (combinedNum > 0) {
				OffsetStackPointer(instructions, i + 1, u - 1, (UInt)num2);
				instructions[i].instruction.type = InstructionType::Push;
				instructions[i].instruction.sizes[0] = (UByte)combinedNum;
				instructions.RemoveAt(u);
				i--;
			}
			// Pop
			else if (combinedNum < 0) {
				OffsetStackPointer(instructions, i + 1, u - 1, (UInt)-num);
				instructions[u].instruction.type = InstructionType::Pop;
				instructions[u].instruction.sizes[0] = (UByte)-combinedNum;
				instructions.RemoveAt(i);
				i--;
			}

			break;
		}

		// Remove push/pop if 0
		if (!found && num == 0) {
			instructions.RemoveAt(i);
			i--;
			continue;
		}
	}
}

void KiwiOptimizer::RearrangeJumps(List<OptimizerInstruction>& instructions) {
	for (UInt i = 0; i < instructions.Count(); i++) {
		if (!instructions[i].CanJump()) continue;

		// Rearrange comp jump
		if (Instruction::IsComp(instructions[i].instruction.type)) {
			instructions[i].instruction.arguments[2] = LastLabelInChain(instructions, instructions[i].instruction.arguments[2]);

			if (i + 2 < instructions.Count()) {
				if (instructions[i + 1].instruction.type == InstructionType::Jmp) {
					if (instructions[i + 2].instruction.type == InstructionType::Label) {
						if (instructions[i + 2].instruction.instructionName == instructions[i].GetJump()) {
							instructions[i].instruction.type = OppositeComp(instructions[i].instruction.type);
							instructions[i].instruction.arguments[2].label = instructions[i + 1].instruction.arguments[0].label;
							instructions.RemoveAt(i + 1);
						}
					}
				}
			}
		}
		// Rearrange jmp jump
		else if (instructions[i].instruction.type == InstructionType::Jmp) {
			instructions[i].instruction.arguments[0] = LastLabelInChain(instructions, instructions[i].instruction.arguments[0]);

			UInt u = i + 1;

			while (u < instructions.Count()) {
				if (instructions[u].instruction.type == InstructionType::Label) break;
				instructions.RemoveAt(u);
			}
		}
		
		// Remove extra labels
		for (UInt u = i + 1; u < instructions.Count(); u++) {
			if (instructions[u].instruction.type != InstructionType::Label) break;
				
			if (instructions[u].instruction.instructionName == instructions[i].GetJump()) {
				instructions.RemoveAt(i);
				i--;
				break;
			}
		}
	}
}

void KiwiOptimizer::RemoveUnusedLabels(List<OptimizerInstruction>& instructions) {
	Set<String> usedLabels;

	// Find used labels
	for (UInt i = 0; i < instructions.Count(); i++) {
		if (instructions[i].CanJump()) {
			usedLabels.Add(instructions[i].GetJump());
		}
	}

	// Remove unused labels
	for (UInt i = 0; i < instructions.Count(); i++) {
		if (instructions[i].instruction.type == InstructionType::Label) {
			if (!usedLabels.Contains(instructions[i].instruction.instructionName)) {
				instructions.RemoveAt(i);
				i--;
			}
		}
	}
}

void KiwiOptimizer::RenameLabels(List<OptimizerInstruction>& instructions, ReplacementMap<String>& labels, UInt& labelIndex) {
	// Rename label name
	for (UInt i = 0; i < instructions.Count(); i++) {
		if (instructions[i].instruction.type == InstructionType::Label) {
			labels.Add(instructions[i].instruction.instructionName, Argument(ArgumentType::Label, labelIndex++).label);
			instructions[i].instruction.instructionName = labels.GetValue(instructions[i].instruction.instructionName);
		}
	}

	// Rename label argument name
	for (UInt i = 0; i < instructions.Count(); i++) {
		for (Argument& arg : instructions[i].instruction.arguments) {
			if (arg.type == ArgumentType::Label) {
				arg.label = labels.GetValue(arg.label);
			}
		}
	}
}

void KiwiOptimizer::RenameRegisters(List<OptimizerInstruction>& instructions) {
	Set<Register> usedRegisters;
	ReplacementMap<Register> replacement;

	// Check initial register use
	CheckAssignedRegisters(instructions, usedRegisters, 0);

	for (UInt i = 0; i < instructions.Count(); i++) {
		// Reset register use
		if (instructions[i].IsLabelOrCall()) {
			usedRegisters = Set<Register>();
			replacement = ReplacementMap<Register>();

			CheckAssignedRegisters(instructions, usedRegisters, i + 1);
			continue;
		}

		const UInt size = instructions[i].instruction.arguments.Count();
		const bool isAssignment = instructions[i].IsRegisterAssignment();

		// Replace registers
		for (UInt u = 0; u < size; u++) {
			if (size == 2 && u == 0 && isAssignment) continue;
			if (size == 3 && u == 2 && isAssignment) continue;
			
			ReplaceArgumentRegister(instructions[i].instruction.arguments[u], replacement);
		}

		// Check register use
		CheckRegisterUse(instructions[i], usedRegisters, replacement);
	}
}

void KiwiOptimizer::CheckAssignedRegisters(List<OptimizerInstruction>& instructions, Set<Register>& usedRegisters, const UInt start) {
	for (UInt i = start; i < instructions.Count(); i++) {
		if (instructions[i].IsLabelOrCall()) break;

		if (instructions[i].important && instructions[i].IsRegisterAssignment()) {
			usedRegisters.Add(instructions[i].GetAssignedRegister());
		}
	}
}

void KiwiOptimizer::CheckRegisterUse(OptimizerInstruction& instruction, Set<Register>& usedRegisters, ReplacementMap<Register>& replacement) {
	const bool isRegAssign = instruction.IsRegisterAssignment();
	const UInt assignIndex = instruction.GetAssignedIndex();

	// Check all arguments
	for (UInt argIndex = 0; argIndex < instruction.instruction.arguments.Count(); argIndex++) {
		// Check if argument is register
		if (instruction.IsRegister(argIndex)) {
			// Handle assignment to the register
			if (isRegAssign && assignIndex == argIndex && !instruction.important) {
				Register reg = instruction.GetRegister(argIndex);
				UInt regIndex = 0;

				while (usedRegisters.Contains(Register(regIndex))) {
					regIndex++;
				}

				instruction.instruction.arguments[argIndex] = Register(regIndex);
				usedRegisters.Add(Register(regIndex));

				if (replacement.Contains(reg)) {
					replacement[reg] = Register(regIndex);
				}
				else {
					replacement.Add(reg, Register(regIndex));
				}
			}
			// Add to used registers
			else {
				usedRegisters.Add(instruction.GetRegister(argIndex));
			}
		}
	}
}

void KiwiOptimizer::OffsetStackPointer(List<OptimizerInstruction>& instructions, const UInt start, const UInt end, const UInt offset) {
	for (UInt i = start; i < end + 1; i++) {
		if (instructions[i].instruction.arguments.IsEmpty()) continue;

		for (Argument& arg : instructions[i].instruction.arguments) {
			if (arg.type == ArgumentType::Memory && arg.mem.memptr.IsLeft() && arg.mem.memptr.GetLeft().type == RegisterType::Stack) {
				arg.mem.offset += offset;
			}
		}
	}
}

InstructionType KiwiOptimizer::OppositeComp(const InstructionType type) {
	switch (type) {
		case InstructionType::Eq: return InstructionType::Ne;
		case InstructionType::Ne: return InstructionType::Eq;
		case InstructionType::Lt: return InstructionType::Ge;
		case InstructionType::Gt: return InstructionType::Le;
		case InstructionType::Le: return InstructionType::Gt;
		case InstructionType::Ge: return InstructionType::Lt;
	}

	return InstructionType::Nop;
}

UInt KiwiOptimizer::NextAssign(List<OptimizerInstruction>& instructions, UInt index, const Argument& arg) {
	for (index++; index < instructions.Count(); index++) {
		if (instructions[index].IsLabelOrCall()) break;
		const OptimizerInstruction inst = instructions[index];

		switch (inst.instruction.arguments.Count()) {
			case 1: {
				if (inst.instruction.arguments[0] == arg) {
					return index;
				}

				break;
			}

			case 2: {
				if (inst.instruction.arguments[0] == arg) {
					return index;
				}

				break;
			}

			case 3: {
				if (inst.instruction.arguments[2] == arg) {
					return index;
				}

				break;
			}
		}
	}

	return instructions.Count();
}

UInt KiwiOptimizer::NextAccess(List<OptimizerInstruction>& instructions, UInt index, const Argument& arg) {
	UInt nextAssign = NextAssign(instructions, index, arg);

	for (index++; index < nextAssign; index++) {
		if (instructions[index].IsLabelOrCall()) break;
		const OptimizerInstruction inst = instructions[index];

		for (const Argument& a : inst.instruction.arguments) {
			if (a == arg) {
				return index;
			}
		}
	}

	return instructions.Count();
}

UInt KiwiOptimizer::NextUse(List<OptimizerInstruction>& instructions, UInt index, const Argument& arg) {
	for (index++; index < instructions.Count(); index++) {
		if (instructions[index].IsLabelOrCall()) break;
		const OptimizerInstruction inst = instructions[index];

		for (const Argument& a : inst.instruction.arguments) {
			if (a == arg) {
				return index;
			}
		}
	}

	return instructions.Count();
}

UInt KiwiOptimizer::UseCount(List<OptimizerInstruction>& instructions, UInt start, UInt end, const Argument& arg) {
	UInt count = 0;

	for (start++; start < end; start++) {
		if (instructions[start].IsLabelOrCall()) break;
		const OptimizerInstruction inst = instructions[start];

		for (const Argument& a : inst.instruction.arguments) {
			if (a == arg) {
				count++;
			}
		}
	}

	return count;
}

UInt KiwiOptimizer::NextRegisterAccess(List<OptimizerInstruction>& instructions, UInt index, const Register& arg) {
	UInt nextAssign = NextAssign(instructions, index, arg);

	for (index++; index < nextAssign; index++) {
		if (instructions[index].IsLabelOrCall()) break;
		const OptimizerInstruction inst = instructions[index];

		for (const Argument& a : inst.instruction.arguments) {
			if (a.type == ArgumentType::Register) {
				if (a.reg == arg) {
					return index;
				}
			}
			else if (a.type == ArgumentType::Memory) {
				if (a.mem.memptr == arg) {
					return index;
				}
			}
		}
	}

	return instructions.Count();
}

Argument KiwiOptimizer::LastLabelInChain(const List<OptimizerInstruction>& instructions, const Argument& label, const Optional<Argument>& firstLabel) {
	Argument first = firstLabel.ValueOr(label);

	for (UInt i = 0; i < instructions.Count(); i++) {
		if (instructions[i].instruction.type != InstructionType::Label) continue;
		if (instructions[i].instruction.instructionName != label.label) continue;

		Argument arg = label;

		for (UInt u = i + 1; u < instructions.Count(); u++) {
			if (instructions[u].instruction.type == InstructionType::Jmp) {
				if (instructions[u].instruction.arguments[0] == first) {
					return first;
				}

				return LastLabelInChain(instructions, instructions[u].instruction.arguments[0], first);
			}
			else if (instructions[u].instruction.type == InstructionType::Label) {
				arg.label = instructions[u].instruction.instructionName;
			}
			else {
				return arg;
			}
		}

		return arg;
	}

	return label;
}

