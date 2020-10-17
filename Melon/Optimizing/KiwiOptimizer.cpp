#include "KiwiOptimizer.h"

#include "OptimizerInstruction.h"

#include "Boxx/Math.h"
#include "Boxx/Set.h"
#include "Boxx/Map.h"

using namespace Boxx;
using namespace Kiwi;
using namespace Melon::Optimizing;

List<Instruction> KiwiOptimizer::Optimize(const List<OptimizerInstruction>& instructionList, const UByte loops) {
	List<Pair<List<OptimizerInstruction>, List<OptimizerInstruction>>> segments = Split(instructionList);

	for (UInt i = 0; i < loops; i++) {
		for (Pair<List<OptimizerInstruction>, List<OptimizerInstruction>>& segment : segments) {
			CombinePushPop(segment.value);

			CombineComp(segment.value);
			RemoveComps(segment.value);

			RemoveDuplicates(segment.value);
			CombineMov(segment.value);
			CombineDuplicates(segment.value);
			ReduceMov(segment.value);

			RearrangeJumps(segment.value);
			UpdateLabels(segment.value);
		}
	}

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

	for (UInt i = 0; i < instructions.Size(); i++) {
		bool start = false;
		OptimizerInstruction inst = instructions[i];

		if (inst.instruction.type == InstructionType::Function) {
			start = true;
		}
		else if (inst.instruction.type == InstructionType::Sub && inst.instruction.arguments[0].type == ArgumentType::Register && inst.instruction.arguments[0].reg.type == RegisterType::Stack) {
			start = true;
		}
		else if (inst.instruction.type == InstructionType::Add && inst.instruction.arguments[0].type == ArgumentType::Register && inst.instruction.arguments[0].reg.type == RegisterType::Stack) {
			start = true;
		}

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
	for (UInt i = 0; i < instructions.Size(); i++) {
		if (instructions[i].IsLabelOrCall()) continue;

		const OptimizerInstruction inst = instructions[i];

		if (inst.instruction.arguments.Size() == 3) {
			if (inst.instruction.arguments[0] == inst.instruction.arguments[2] && inst.instruction.sizes[0] == inst.instruction.sizes[2]) {
				instructions[i].instruction.arguments.RemoveAt(2);
				i--;
			}
			else for (UInt j = i; j < instructions.Size(); j++) {
				if (inst.important) break;
				UInt nextAssign = NextAssign(instructions, i, inst.instruction.arguments[2]);

				if (nextAssign > instructions.Size()) {
					nextAssign = instructions.Size();
				}

				if (UseCount(instructions, i, nextAssign, inst.instruction.arguments[2]) > 1) {
					break;
				}

				for (UInt u = j + 1; u < nextAssign; u++) {
					if (instructions[u].IsLabelOrCall()) break;

					if (instructions[u].instruction.type == InstructionType::Mov) {
						if (instructions[u].instruction.arguments[1] == inst.instruction.arguments[2]) {
							instructions[i].instruction.arguments[2] = instructions[u].instruction.arguments[0];
							instructions[i].instruction.sizes[2] = instructions[u].instruction.sizes[0];
							instructions[i].instruction.signs[2] = instructions[u].instruction.signs[0];
							instructions.RemoveAt(u);
							i--;
							j--;
							break;
						}
					}
				}
			}
		}
		else if (inst.instruction.arguments.Size() == 2) {
			if (inst.instruction.type == InstructionType::Mov && inst.instruction.arguments[0] == inst.instruction.arguments[1]) {
				instructions.RemoveAt(i);
				i--;
				continue;
			}

			for (UInt j = i; j < instructions.Size(); j++) {
				if (instructions[i].important) break;
				UInt nextAssign = NextAssign(instructions, i, instructions[i].instruction.arguments[0]);

				if (nextAssign > instructions.Size()) {
					nextAssign = instructions.Size();
				}

				if (UseCount(instructions, i, nextAssign, instructions[i].instruction.arguments[0]) > 1) {
					break;
				}

				for (UInt u = j + 1; u < nextAssign; u++) {
					if (instructions[u].IsLabelOrCall()) break;

					if (instructions[u].instruction.type == InstructionType::Mov) {
						if (instructions[u].instruction.arguments[1] == instructions[i].instruction.arguments[0]) {
							instructions[i].instruction.arguments[0] = instructions[u].instruction.arguments[0];
							instructions[i].instruction.sizes[0] = instructions[u].instruction.sizes[0];
							instructions[i].instruction.signs[0] = instructions[u].instruction.signs[0];
							instructions.RemoveAt(u);
							j--;
							break;
						}
					}
				}
			}
		}
	}
}

void KiwiOptimizer::CombineMov(List<OptimizerInstruction>& instructions) {
	for (UInt i = 0; i < instructions.Size(); i++) {
		if (instructions[i].IsLabelOrCall()) continue;

		const OptimizerInstruction inst = instructions[i];

		if (inst.instruction.type == InstructionType::Mov) {
			if (inst.instruction.sizes[0] == inst.instruction.sizes[1] && inst.instruction.sizes[0] < KiwiLang::maxSize) {
				if (inst.instruction.arguments[0].type == ArgumentType::Memory && inst.instruction.arguments[1].type == ArgumentType::Memory) {
					const Register reg1 = inst.instruction.arguments[0].mem.reg;
					const Register reg2 = inst.instruction.arguments[1].mem.reg;
					Int offset1 = inst.instruction.arguments[0].mem.offset;
					Int offset2 = inst.instruction.arguments[1].mem.offset;

					List<OptimizerInstruction> insts;
					List<UByte> sizes;
					UByte totalSize = inst.instruction.sizes[0];
					insts.Add(inst);
					sizes.Add(totalSize);

					for (UInt u = i + 1; u < instructions.Size(); u++) {
						if (instructions[u].IsLabelOrCall()) break;

						const OptimizerInstruction inst2 = instructions[u];

						if (inst2.instruction.type != InstructionType::Mov) {
							break;
						}

						if (inst2.instruction.sizes[0] != inst2.instruction.sizes[1] || inst2.instruction.sizes[0] >= KiwiLang::maxSize) {
							break;
						}

						if (
							inst2.instruction.arguments[0].type != ArgumentType::Memory ||
							inst2.instruction.arguments[1].type != ArgumentType::Memory
						) {
							break;
						}

						if (inst2.instruction.arguments[0].mem.reg != reg1 || inst2.instruction.arguments[1].mem.reg != reg2) {
							break;
						}

						if (inst2.instruction.sizes[0] + totalSize > KiwiLang::maxSize) {
							break;
						}

						if (
							inst2.instruction.arguments[0].mem.offset == offset1 - inst2.instruction.sizes[0] &&
							inst2.instruction.arguments[1].mem.offset == offset2 - inst2.instruction.sizes[0]
						) {
							insts.Add(inst2);
							sizes.Add(inst2.instruction.sizes[0]);
							totalSize += sizes.Last();
							offset1 -= sizes.Last();
							offset2 -= sizes.Last();
						}
						else {
							break;
						}
					}

					while (!KiwiLang::IsValidSize(totalSize)) {
						totalSize -= sizes.Last();
						offset1 += sizes.Last();
						offset2 += sizes.Last();
						sizes.RemoveAt(sizes.Size() - 1);
						insts.RemoveAt(insts.Size() - 1);
					}

					if (insts.Size() == 1) break;

					instructions[i].instruction.sizes[0] = totalSize;
					instructions[i].instruction.sizes[1] = totalSize;
					instructions[i].instruction.arguments[0].mem.offset = offset1;
					instructions[i].instruction.arguments[1].mem.offset = offset2;

					for (UInt u = i + 1; u < i + insts.Size(); u++) {
						instructions.RemoveAt(i + 1);
					}
				}
				else if (inst.instruction.arguments[0].type == ArgumentType::Memory && inst.instruction.arguments[1].type == ArgumentType::Number) {
					const Register reg = inst.instruction.arguments[0].mem.reg;
					Int offset = inst.instruction.arguments[0].mem.offset;
					Long num = inst.instruction.arguments[1].number;

					List<OptimizerInstruction> insts;
					List<UByte> sizes;
					List<Long> numbers;
					UByte totalSize = inst.instruction.sizes[0];
					insts.Add(inst);
					sizes.Add(totalSize);
					numbers.Add(num);

					for (UInt u = i + 1; u < instructions.Size(); u++) {
						if (instructions[u].IsLabelOrCall()) break;

						const OptimizerInstruction inst2 = instructions[u];

						if (inst2.instruction.type != InstructionType::Mov) {
							break;
						}

						if (inst2.instruction.sizes[0] != inst2.instruction.sizes[1] || inst2.instruction.sizes[0] >= KiwiLang::maxSize) {
							break;
						}

						if (
							inst2.instruction.arguments[0].type != ArgumentType::Memory ||
							inst2.instruction.arguments[1].type != ArgumentType::Number
						) {
							break;
						}

						if (inst2.instruction.arguments[0].mem.reg != reg) {
							break;
						}

						if (inst2.instruction.sizes[0] + totalSize > KiwiLang::maxSize) {
							break;
						}

						if (inst2.instruction.arguments[0].mem.offset == offset - inst2.instruction.sizes[0]) {
							insts.Add(inst2);
							sizes.Add(inst2.instruction.sizes[0]);
							numbers.Add(inst2.instruction.arguments[1].number);
							totalSize += sizes.Last();
							offset -= sizes.Last();
						}
						else {
							break;
						}
					}

					while (!KiwiLang::IsValidSize(totalSize)) {
						totalSize -= sizes.Last();
						sizes.RemoveAt(sizes.Size() - 1);
						insts.RemoveAt(insts.Size() - 1);
						numbers.RemoveAt(numbers.Size() - 1);
					}

					if (insts.Size() == 1) continue;

					instructions[i].instruction.sizes[0] = totalSize;
					instructions[i].instruction.sizes[1] = totalSize;
					instructions[i].instruction.arguments[0].mem.offset -= totalSize - sizes[0];
					instructions[i].instruction.arguments[1].number = 0;

					for (UInt u = i; u < i + insts.Size(); u++) {
						totalSize -= sizes[0];

						ULong numMask = 0;

						switch (sizes[0]) {
							case 8: numMask = Math::ULongMax();  break;
							case 4: numMask = Math::UIntMax();   break;
							case 2: numMask = Math::UShortMax(); break;
							case 1: numMask = Math::UByteMax();  break;
						}

						sizes.RemoveAt(0);
						instructions[i].instruction.arguments[1].number |= (numbers[0] & numMask) << totalSize * 8;
						numbers.RemoveAt(0);

						if (u != i) instructions.RemoveAt(i + 1);
					}
				}
			}
		}
	}
}

void KiwiOptimizer::RemoveDuplicates(List<OptimizerInstruction>& instructions) {
	for (UInt i = 0; i < instructions.Size(); i++) {
		if (instructions[i].IsLabelOrCall()) continue;

		const OptimizerInstruction inst = instructions[i];

		if (inst.instruction.type == InstructionType::Mov && inst.instruction.arguments.Size() == 2) {
			const Argument arg1 = inst.instruction.arguments[0];
			const Argument arg2 = inst.instruction.arguments[1];

			UInt index = NextAssign(instructions, i, arg1);

			while (index < instructions.Size()) {
				if (instructions[index].IsLabelOrCall()) break;

				const OptimizerInstruction in = instructions[index];

				if (in.instruction.type == InstructionType::Mov && in.instruction.arguments.Size() == 2) {
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
	for (UInt i = 0; i < instructions.Size(); i++) {
		if (instructions[i].IsLabelOrCall()) continue;

		const OptimizerInstruction inst = instructions[i];

		if (inst.instruction.type == InstructionType::Mov && IsRegister(inst.instruction.arguments[0])) {
			Argument arg1 = inst.instruction.arguments[0];
			Argument arg2 = inst.instruction.arguments[1];

			UInt nextAssign1 = NextAssign(instructions, i, arg1);
			UInt nextAssign2 = NextAssign(instructions, i, arg2);

			Map<Register, Register> replacement;

			for (UInt u = i + 1; u < nextAssign1; u++) {
				if (instructions[u].IsLabelOrCall()) break;

				Optional<Tuple<Register, Register>> nextReplacement = nullptr;

				if (u < nextAssign2 && instructions[u].instruction.type == InstructionType::Mov && IsRegister(instructions[u].instruction.arguments[0]) && instructions[u].instruction.arguments[1] == arg2) {
					const Argument arg = instructions[u].instruction.arguments[0];
					const UInt assign = NextAssign(instructions, u, arg);

					if (nextAssign1 < assign && NextRegisterGet(instructions, i, arg.reg) > assign) {
						nextAssign1 = assign;

						for (UInt p = i + 1; p < u; p++) {
							for (Argument& argument : instructions[p].instruction.arguments) {
								if (argument == arg1) {
									argument = arg;
								}
								else if (argument.type == ArgumentType::Memory && argument.mem.reg == arg1.reg) {
									argument.mem.reg = arg.reg;
								}
							}
						}

						replacement.Add(arg1.reg, arg.reg);
						arg1 = arg;
						instructions[i].instruction.arguments[0] = arg;
						instructions.RemoveAt(u);
						u--;
					}
					else {
						nextReplacement = Tuple<Register, Register>(arg.reg, arg1.reg);
						instructions.RemoveAt(u);
						u--;
					}
				}

				if (instructions[u].instruction.arguments.Size() == 2 && instructions[u].instruction.type != InstructionType::Adr) {
					Register arg;

					if (instructions[u].instruction.arguments[0].type == ArgumentType::Memory) {
						if (replacement.Contains(instructions[u].instruction.arguments[0].mem.reg, arg)) {
							instructions[u].instruction.arguments[0].mem.reg = arg;
						}
					}

					if (instructions[u].instruction.arguments[1].type == ArgumentType::Register) {
						if (replacement.Contains(instructions[u].instruction.arguments[1].reg, arg)) {
							instructions[u].instruction.arguments[1].reg = arg;
						}
					}
					else if (instructions[u].instruction.arguments[1].type == ArgumentType::Memory) {
						if (replacement.Contains(instructions[u].instruction.arguments[1].mem.reg, arg)) {
							instructions[u].instruction.arguments[1].mem.reg = arg;
						}
					}
				}
				else if (instructions[u].instruction.arguments.Size() == 3) {
					Register arg;

					if (instructions[u].instruction.arguments[0].type == ArgumentType::Register) {
						if (replacement.Contains(instructions[u].instruction.arguments[0].reg, arg)) {
							instructions[u].instruction.arguments[0].reg = arg;
						}
					}
					else if (instructions[u].instruction.arguments[0].type == ArgumentType::Memory) {
						if (replacement.Contains(instructions[u].instruction.arguments[0].mem.reg, arg)) {
							instructions[u].instruction.arguments[0].mem.reg = arg;
						}
					}

					if (instructions[u].instruction.arguments[1].type == ArgumentType::Register) {
						if (replacement.Contains(instructions[u].instruction.arguments[1].reg, arg)) {
							instructions[u].instruction.arguments[1].reg = arg;
						}
					}
					else if (instructions[u].instruction.arguments[1].type == ArgumentType::Memory) {
						if (replacement.Contains(instructions[u].instruction.arguments[1].mem.reg, arg)) {
							instructions[u].instruction.arguments[1].mem.reg = arg;
						}
					}

					if (instructions[u].instruction.arguments[2].type == ArgumentType::Memory) {
						if (replacement.Contains(instructions[u].instruction.arguments[2].mem.reg, arg)) {
							instructions[u].instruction.arguments[2].mem.reg = arg;
						}
					}
				}

				if (instructions[u].instruction.type == InstructionType::Mov || instructions[u].instruction.type == InstructionType::Adr) {
					if (instructions[u].instruction.arguments[0].type == ArgumentType::Register) {
						if (replacement.Contains(instructions[u].instruction.arguments[0].reg)) {
							replacement.Remove(instructions[u].instruction.arguments[0].reg);
						}
					}
				}

				if (nextReplacement) {
					replacement.Add(nextReplacement.Get().value1, nextReplacement.Get().value2);
				}
			}
		}
	}
}

void KiwiOptimizer::RemoveUnused(List<OptimizerInstruction>& instructions) {
	for (UInt i = 0; i < instructions.Size(); i++) {
		if (instructions[i].IsLabelOrCall()) continue;

		const OptimizerInstruction inst = instructions[i];
	}
}

bool KiwiOptimizer::IsRegister(const Argument& arg) {
	return arg.type == ArgumentType::Register && arg.reg.type == RegisterType::Register;
}

void KiwiOptimizer::CombineComp(List<OptimizerInstruction>& instructions) {
	for (UInt i = 0; i < instructions.Size(); i++) {
		if (instructions[i].IsLabelOrCall()) continue;

		const OptimizerInstruction inst = instructions[i];

		if (Instruction::IsComp(inst.instruction.type) && !inst.important && inst.instruction.arguments.Size() == 3) {
			if (i + 1 < instructions.Size()) {
				const OptimizerInstruction inst2 = instructions[i + 1];

				if (Instruction::IsComp(inst2.instruction.type)) continue;

				if (inst2.instruction.arguments[0] == inst.instruction.arguments[2] && inst2.instruction.arguments[1].type == ArgumentType::Number) {
					if (inst2.instruction.type == InstructionType::Eq) {
						if (inst2.instruction.arguments[1].number == 0) {
							instructions[i].instruction.type = OppositeComp(inst.instruction.type);
						}

						instructions[i].instruction.arguments[2] = inst2.instruction.arguments.Size() == 3 ? inst2.instruction.arguments[2] : inst2.instruction.arguments[0];
						instructions.RemoveAt(i + 1);
					}
					else if (inst2.instruction.type == InstructionType::Ne) {
						if (inst2.instruction.arguments[1].number == 1) {
							instructions[i].instruction.type = OppositeComp(inst.instruction.type);
						}

						instructions[i].instruction.arguments[2] = inst2.instruction.arguments.Size() == 3 ? inst2.instruction.arguments[2] : inst2.instruction.arguments[0];
						instructions.RemoveAt(i + 1);
					}
				}
			}
		}
	}
}

void KiwiOptimizer::RemoveComps(List<OptimizerInstruction>& instructions) {
	for (UInt i = 0; i < instructions.Size(); i++) {
		if (instructions[i].IsLabelOrCall()) continue;

		const OptimizerInstruction inst = instructions[i];

		if (Instruction::IsComp(inst.instruction.type) && !inst.important && inst.instruction.arguments.Size() == 3) {
			if (inst.instruction.arguments[0].type == ArgumentType::Number && inst.instruction.arguments[1].type == ArgumentType::Number) {
				bool output = false;

				switch (inst.instruction.type) {
					case InstructionType::Eq: output = inst.instruction.arguments[0].number == inst.instruction.arguments[1].number; break;
					case InstructionType::Ne: output = inst.instruction.arguments[0].number != inst.instruction.arguments[1].number; break;
					case InstructionType::Lt: output = inst.instruction.arguments[0].number  < inst.instruction.arguments[1].number; break;
					case InstructionType::Gt: output = inst.instruction.arguments[0].number  > inst.instruction.arguments[1].number; break;
					case InstructionType::Le: output = inst.instruction.arguments[0].number <= inst.instruction.arguments[1].number; break;
					case InstructionType::Ge: output = inst.instruction.arguments[0].number >= inst.instruction.arguments[1].number; break;
				}

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
				else {
					Instruction mov = Instruction(InstructionType::Mov, inst.instruction.sizes[2]);
					mov.arguments.Add(inst.instruction.arguments[2]);
					mov.arguments.Add(Argument(output ? 1 : 0));
					instructions[i].instruction = mov;
				}
			}
		}
	}
}

void KiwiOptimizer::CombinePushPop(List<OptimizerInstruction>& instructions) {
	for (UInt i = 0; i < instructions.Size(); i++) {
		if (instructions[i].IsLabelOrCall()) continue;

		const OptimizerInstruction inst = instructions[i];

		if ((inst.instruction.type == InstructionType::Push || inst.instruction.type == InstructionType::Pop) && inst.instruction.arguments.Size() == 0) {
			Long num = inst.instruction.type == InstructionType::Push ? inst.instruction.sizes[0] : -inst.instruction.sizes[0];

			for (UInt u = i + 1; u < instructions.Size(); u++) {
				if (instructions[u].IsLabelOrCall()) break;
				const OptimizerInstruction inst2 = instructions[u];

				if ((inst2.instruction.type == InstructionType::Push || inst2.instruction.type == InstructionType::Pop) && inst2.instruction.arguments.Size() == 0) {
					Long num2 = inst2.instruction.type == InstructionType::Push ? inst2.instruction.sizes[0] : -inst2.instruction.sizes[0];
					Long combinedNum = num + num2;

					if (inst.important && Math::Abs(num) > Math::Abs(combinedNum)) {
						break;
					}

					if (combinedNum == 0) {
						OffsetStackPointer(instructions, i + 1, u - 1, num2);
						instructions.RemoveAt(u);
						instructions.RemoveAt(i);
						i--;
					}
					else if (combinedNum > 0) {
						OffsetStackPointer(instructions, i + 1, u - 1, num2);
						instructions[i].instruction.type = InstructionType::Push;
						instructions[i].instruction.sizes[0] = combinedNum;
						instructions.RemoveAt(u);
						i--;
					}
					else if (combinedNum < 0) {
						OffsetStackPointer(instructions, i + 1, u - 1, -num);
						instructions[u].instruction.type = InstructionType::Pop;
						instructions[u].instruction.sizes[0] = -combinedNum;
						instructions.RemoveAt(i);
						i--;
					}

					break;
				}
			}
		}
	}
}

void KiwiOptimizer::RearrangeJumps(List<OptimizerInstruction>& instructions) {
	for (UInt i = 0; i < instructions.Size(); i++) {
		if (!instructions[i].CanJump()) continue;

		if (Instruction::IsComp(instructions[i].instruction.type)) {
			if (i + 2 < instructions.Size()) {
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
		else if (instructions[i].instruction.type == InstructionType::Jmp) {
			UInt u = i + 1;

			while (u < instructions.Size()) {
				if (instructions[u].instruction.type == InstructionType::Label) break;
				instructions.RemoveAt(u);
			}
		}
		
		for (UInt u = i + 1; u < instructions.Size(); u++) {
			if (instructions[u].instruction.type != InstructionType::Label) break;
				
			if (instructions[u].instruction.instructionName == instructions[i].GetJump()) {
				instructions.RemoveAt(i);
				i--;
				break;
			}
		}
	}
}

void KiwiOptimizer::UpdateLabels(List<OptimizerInstruction>& instructions) {
	Set<String> usedLabels;

	for (UInt i = 0; i < instructions.Size(); i++) {
		if (instructions[i].CanJump()) {
			usedLabels.Add(instructions[i].GetJump());
		}
	}

	for (UInt i = 0; i < instructions.Size(); i++) {
		if (instructions[i].instruction.type == InstructionType::Label) {
			if (!usedLabels.Contains(instructions[i].instruction.instructionName)) {
				instructions.RemoveAt(i);
				i--;
			}
		}
	}
}

void KiwiOptimizer::OffsetStackPointer(List<OptimizerInstruction>& instructions, const UInt start, const UInt end, const UInt offset) {
	for (UInt i = start; i < end + 1; i++) {
		if (!instructions[i].instruction.arguments.IsEmpty()) {
			for (Argument& arg : instructions[i].instruction.arguments) {
				if (arg.type == ArgumentType::Memory && arg.mem.reg == RegisterType::Stack) {
					arg.mem.offset += offset;
				}
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
	for (index++; index < instructions.Size(); index++) {
		if (instructions[index].IsLabelOrCall()) break;
		const OptimizerInstruction inst = instructions[index];

		switch (inst.instruction.arguments.Size()) {
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

	return instructions.Size();
}

UInt KiwiOptimizer::NextGet(List<OptimizerInstruction>& instructions, UInt index, const Argument& arg) {
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

	return instructions.Size();
}

UInt KiwiOptimizer::NextUse(List<OptimizerInstruction>& instructions, UInt index, const Argument& arg) {
	for (index++; index < instructions.Size(); index++) {
		if (instructions[index].IsLabelOrCall()) break;
		const OptimizerInstruction inst = instructions[index];

		for (const Argument& a : inst.instruction.arguments) {
			if (a == arg) {
				return index;
			}
		}
	}

	return instructions.Size();
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

UInt KiwiOptimizer::NextRegisterGet(List<OptimizerInstruction>& instructions, UInt index, const Register& arg) {
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
				if (a.mem.reg == arg) {
					return index;
				}
			}
		}
	}

	return instructions.Size();
}