#pragma once

#include "Kiwi/Kiwi.h"

namespace Melon {
	namespace Optimizing {

		struct OptimizerInstruction {
			Kiwi::Instruction instruction;
			bool important = false;

			OptimizerInstruction () {}

			OptimizerInstruction(const Kiwi::Instruction& inst) {
				instruction = inst;
			}

			~OptimizerInstruction() {}

			bool CanJump() const {
				if (Kiwi::Instruction::IsComp(instruction.type)) {
					return 
						instruction.arguments.Size() == 3 &&
						instruction.arguments[2].type == Kiwi::ArgumentType::Label;
				}

				return instruction.type == Kiwi::InstructionType::Jmp;
			}

			bool WillJump() const {
				return instruction.type == Kiwi::InstructionType::Jmp;
			}

			Boxx::String GetJump() const {
				if (!CanJump()) return "";

				if (Kiwi::Instruction::IsComp(instruction.type)) {
					return instruction.arguments[2].label;
				}

				return instruction.arguments[0].label;
			}

			bool IsLabel() const {
				return instruction.type == Kiwi::InstructionType::Label;
			}

			bool IsCall() const {
				return instruction.type == Kiwi::InstructionType::Call;
			}

			bool IsLabelOrCall() const {
				return IsLabel() || IsCall();
			}
		};
	}
}