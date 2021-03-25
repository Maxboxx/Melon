#pragma once

#include "Kiwi/Kiwi.h"

///N Melon::Optimizing
namespace Melon {
	namespace Optimizing {
		/// A kiwi instruction used for optimization.
		struct OptimizerInstruction {
			/// The kiwi unstruction.
			Kiwi::Instruction instruction;

			/// {true} if the instruction is important.
			bool important = false;

			OptimizerInstruction () {}

			OptimizerInstruction(const Kiwi::Instruction& inst) {
				instruction = inst;
			}

			~OptimizerInstruction() {}

			/// {true} if the instruction can jump to a label.
			bool CanJump() const {
				if (Kiwi::Instruction::IsComp(instruction.type)) {
					return 
						instruction.arguments.Size() == 3 &&
						instruction.arguments[2].type == Kiwi::ArgumentType::Label;
				}

				return instruction.type == Kiwi::InstructionType::Jmp;
			}

			/// {true} if the instruction will jump to a label.
			bool WillJump() const {
				return instruction.type == Kiwi::InstructionType::Jmp;
			}

			/// Gets the label name of the jump.
			Boxx::String GetJump() const {
				if (!CanJump()) return "";

				if (Kiwi::Instruction::IsComp(instruction.type)) {
					return instruction.arguments[2].label;
				}

				return instruction.arguments[0].label;
			}

			/// {true} if the instruction is a label.
			bool IsLabel() const {
				return instruction.type == Kiwi::InstructionType::Label;
			}

			/// {true} if the instruction is a call.
			bool IsCall() const {
				return instruction.type == Kiwi::InstructionType::Call;
			}

			/// {true} if the instruction is a label or call.
			bool IsLabelOrCall() const {
				return IsLabel() || IsCall();
			}

			/// {true} if the instruction is a push or pop instruction.
			bool IsPushPop() const {
				return instruction.type == Kiwi::InstructionType::Push || instruction.type == Kiwi::InstructionType::Pop;
			}

			/// {true} if the instruction is a jump or ret instruction.
			bool IsJumpOrRet() const {
				return instruction.type == Kiwi::InstructionType::Jmp || instruction.type == Kiwi::InstructionType::Ret || instruction.type == Kiwi::InstructionType::Exit;
			}

			/// {true} if the instruction is a label, call, push, pop, jump, or ret.
			bool IsStackBreak() const {
				return IsLabelOrCall() || IsPushPop() || IsJumpOrRet();
			}

			/// {true} if the argument at the specified index is a register.
			bool IsRegister(const Boxx::UInt index) const {
				if (instruction.arguments[index].type == Kiwi::ArgumentType::Register) {
					return instruction.arguments[index].reg.type == Kiwi::RegisterType::Register;
				}
				else if (instruction.arguments[index].type == Kiwi::ArgumentType::Memory) {
					return instruction.arguments[index].mem.memptr.IsLeft() && instruction.arguments[index].mem.memptr.GetLeft().type == Kiwi::RegisterType::Register;
				}

				return false;
			}

			/// Gets the register of the argument at the specified index.
			Kiwi::Register GetRegister(const Boxx::UInt index) const {
				if (instruction.arguments[index].type == Kiwi::ArgumentType::Register) {
					return instruction.arguments[index].reg;
				}
				else if (instruction.arguments[index].type == Kiwi::ArgumentType::Memory) {
					return instruction.arguments[index].mem.memptr.GetLeft();
				}

				return Kiwi::Register();
			}

			/// {true} if the instruction assigns a value to a register or memory location.
			bool IsAssignment() const {
				if (instruction.arguments.Size() == 2) {
					return instruction.type == Kiwi::InstructionType::Mov || instruction.type == Kiwi::InstructionType::Adr;
				}
				else {
					return instruction.arguments.Size() == 3;
				}
			}

			/// {true} if the instruction assigns a value to a register.
			bool IsRegisterAssignment() const {
				if (!IsAssignment()) return false;

				if (instruction.arguments.Size() == 2) {
					return instruction.arguments[0].type == Kiwi::ArgumentType::Register;
				}
				else {
					return instruction.arguments[2].type == Kiwi::ArgumentType::Register;
				}
			}

			/// Gets the register that a value is assigned to.
			Kiwi::Register GetAssignedRegister() const {
				if (IsRegisterAssignment()) {
					return instruction.arguments.Size() == 2 ? GetRegister(0) : GetRegister(2);
				}

				return Kiwi::Register();
			}

			/// Gets the index of the assigned register or memory location.
			Boxx::UInt GetAssignedIndex() const {
				if (IsAssignment()) {
					return instruction.arguments.Size() == 2 ? 0 : 2;
				}

				return 0;
			}
		};
	}
}