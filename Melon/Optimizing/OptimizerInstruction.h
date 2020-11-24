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

			bool IsPushPop() const {
				return instruction.type == Kiwi::InstructionType::Push || instruction.type == Kiwi::InstructionType::Pop;
			}

			bool IsJumpOrRet() const {
				return instruction.type == Kiwi::InstructionType::Jmp || instruction.type == Kiwi::InstructionType::Ret || instruction.type == Kiwi::InstructionType::Exit;
			}

			bool IsStackBreak() const {
				return IsLabelOrCall() || IsPushPop() || IsJumpOrRet();
			}

			bool IsRegister(const Boxx::UInt index) const {
				if (instruction.arguments[index].type == Kiwi::ArgumentType::Register) {
					return instruction.arguments[index].reg.type == Kiwi::RegisterType::Register;
				}
				else if (instruction.arguments[index].type == Kiwi::ArgumentType::Memory) {
					return instruction.arguments[index].mem.memptr.IsLeft() && instruction.arguments[index].mem.memptr.GetLeft().type == Kiwi::RegisterType::Register;
				}

				return false;
			}

			Kiwi::Register GetRegister(const Boxx::UInt index) const {
				if (instruction.arguments[index].type == Kiwi::ArgumentType::Register) {
					return instruction.arguments[index].reg;
				}
				else if (instruction.arguments[index].type == Kiwi::ArgumentType::Memory) {
					return instruction.arguments[index].mem.memptr.GetLeft();
				}

				return Kiwi::Register();
			}

			bool IsAssignment() const {
				if (instruction.arguments.Size() == 2) {
					return instruction.type == Kiwi::InstructionType::Mov || instruction.type == Kiwi::InstructionType::Adr;
				}
				else {
					return instruction.arguments.Size() == 3;
				}
			}

			bool IsAssignmentRegister() const {
				if (!IsAssignment()) return false;

				if (instruction.arguments.Size() == 2) {
					return instruction.arguments[0].type == Kiwi::ArgumentType::Register;
				}
				else {
					return instruction.arguments[2].type == Kiwi::ArgumentType::Register;
				}
			}
		};
	}
}