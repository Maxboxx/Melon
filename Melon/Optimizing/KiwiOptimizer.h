#pragma once

#include "Boxx/List.h"
#include "Boxx/ReplacementMap.h"

#include "Kiwi/Old/Kiwi.h"

#include "OptimizerInstruction.h"

///N Melon::Optimizing
namespace Melon {
	namespace Optimizing {
		/// Class for optimizing kiwi instructions
		class KiwiOptimizer {
		public:
			/// Optimizes a list of kiwi instructions.
			///A loops: The number of optimization loops to perform.
			static Boxx::List<KiwiOld::Instruction> Optimize(const Boxx::List<OptimizerInstruction>& instructionList, const Boxx::UInt loops = 1);

		private:
			static Boxx::List<Boxx::Pair<Boxx::List<OptimizerInstruction>, Boxx::List<OptimizerInstruction>>> Split(const Boxx::List<OptimizerInstruction>& instructions);

			// Reduces the chain of mov instructions
			static void ReduceMov(Boxx::List<OptimizerInstruction>& instructions);
			static void ReduceMov2(Boxx::List<OptimizerInstruction>& instructions, Boxx::UInt& i);
			static void ReduceMov3(Boxx::List<OptimizerInstruction>& instructions, Boxx::UInt& i);

			// Combines smaller mov instructions into one big mov instruction
			static void CombineMov(Boxx::List<OptimizerInstruction>& instructions);
			static void CombineMemoryMov(Boxx::List<OptimizerInstruction>& instructions, Boxx::UInt& i);
			static void CombineNumberMov(Boxx::List<OptimizerInstruction>& instructions, Boxx::UInt& i);

			// Removes duplicate mov instructions
			static void RemoveDuplicates(Boxx::List<OptimizerInstruction>& instructions);

			// Combines mov instructions that assigns the same value to different places
			static void CombineDuplicates(Boxx::List<OptimizerInstruction>& instructions);

			struct CombineInfo {
				OptimizerInstruction inst;
				KiwiOld::Argument arg1, arg2;
				Boxx::UInt nextAssign1{}, nextAssign2{};

				Boxx::ReplacementMap<KiwiOld::Register> replacement;
				Boxx::Optional<Boxx::Tuple<KiwiOld::Register, KiwiOld::Register>> nextReplacement;
				Boxx::Optional<KiwiOld::Register> remove;
			};

			static void UpdateCombineInfo(Boxx::List<OptimizerInstruction>& instructions, CombineInfo& info, const Boxx::UInt initialIndex, Boxx::UInt& index);
			static void ReplaceInstructionRegisters(KiwiOld::Instruction& instruction, const Boxx::ReplacementMap<KiwiOld::Register>& replacement);
			static void ReplaceArgumentRegister(KiwiOld::Argument& argument, const Boxx::ReplacementMap<KiwiOld::Register>& replacement);
			static void ReplaceArgumentMemoryRegister(KiwiOld::Argument& argument, const Boxx::ReplacementMap<KiwiOld::Register>& replacement);
			
			// Removes unused instructions
			static void RemoveUnused(Boxx::List<OptimizerInstruction>& instructions);

			static bool IsRegister(const KiwiOld::Argument& arg);

			// Combine comparison instructions
			static void CombineComp(Boxx::List<OptimizerInstruction>& instructions);

			// Remove comparison instructions
			static void RemoveComps(Boxx::List<OptimizerInstruction>& instructions);

			// Combine push and pop instructions
			static void CombinePushPop(Boxx::List<OptimizerInstruction>& instructions);
			static void InsertEmptyPushPop(Boxx::List<OptimizerInstruction>& instructions);

			// Rearrange jumps
			static void RearrangeJumps(Boxx::List<OptimizerInstruction>& instructions);

			// Remove unused labels
			static void RemoveUnusedLabels(Boxx::List<OptimizerInstruction>& instructions);

			// Rename labels
			static void RenameLabels(Boxx::List<OptimizerInstruction>& instructions, Boxx::ReplacementMap<Boxx::String>& labels, Boxx::UInt& labelIndex);

			// Rename registers
			static void RenameRegisters(Boxx::List<OptimizerInstruction>& instructions);
			static void CheckAssignedRegisters(Boxx::List<OptimizerInstruction>& instructions, Boxx::Set<KiwiOld::Register>& usedRegisters, const Boxx::UInt start);
			static void CheckRegisterUse(OptimizerInstruction& instruction, Boxx::Set<KiwiOld::Register>& usedRegisters, Boxx::ReplacementMap<KiwiOld::Register>& replacement);

			static void OffsetStackPointer(Boxx::List<OptimizerInstruction>& instructions, const Boxx::UInt start, const Boxx::UInt end, const Boxx::UInt offset);
			static KiwiOld::InstructionType OppositeComp(const KiwiOld::InstructionType type);

			// Get next assignment to the specified argument
			static Boxx::UInt NextAssign(Boxx::List<OptimizerInstruction>& instructions, Boxx::UInt index, const KiwiOld::Argument& arg);

			// Get next access to the specified argument
			static Boxx::UInt NextAccess(Boxx::List<OptimizerInstruction>& instructions, Boxx::UInt index, const KiwiOld::Argument& arg);

			// Get next use of the specified argument
			static Boxx::UInt NextUse(Boxx::List<OptimizerInstruction>& instructions, Boxx::UInt index, const KiwiOld::Argument& arg);

			// Get the use count of the specified argument
			static Boxx::UInt UseCount(Boxx::List<OptimizerInstruction>& instructions, Boxx::UInt start, Boxx::UInt end, const KiwiOld::Argument& arg);

			// Get next access to the specified register
			static Boxx::UInt NextRegisterAccess(Boxx::List<OptimizerInstruction>& instructions, Boxx::UInt index, const KiwiOld::Register& reg);

			// Get last label in jump chain
			static KiwiOld::Argument LastLabelInChain(const Boxx::List<OptimizerInstruction>& instructions, const KiwiOld::Argument& label, const Boxx::Optional<KiwiOld::Argument>& firstLabel = nullptr);
		};
	}
}