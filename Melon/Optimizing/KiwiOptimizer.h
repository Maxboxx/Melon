#pragma once

#include "Boxx/List.h"

#include "Kiwi/Kiwi.h"

namespace Melon {
	namespace Optimizing {
		struct OptimizerInstruction;

		class KiwiOptimizer {
		public:
			static Boxx::List<Kiwi::Instruction> Optimize(const Boxx::List<OptimizerInstruction>& instructionList, const Boxx::UByte loops = 1);

		private:
			static Boxx::List<Boxx::Pair<Boxx::List<OptimizerInstruction>, Boxx::List<OptimizerInstruction>>> Split(const Boxx::List<OptimizerInstruction>& instructions);

			static void ReduceMov(Boxx::List<OptimizerInstruction>& instructions);
			static void CombineMov(Boxx::List<OptimizerInstruction>& instructions);
			static void RemoveDuplicates(Boxx::List<OptimizerInstruction>& instructions);
			static void RemoveUnused(Boxx::List<OptimizerInstruction>& instructions);

			static void CombineComp(Boxx::List<OptimizerInstruction>& instructions);
			static void RemoveComps(Boxx::List<OptimizerInstruction>& instructions);

			static void CombinePushPop(Boxx::List<OptimizerInstruction>& instructions);

			static void RearrangeJumps(Boxx::List<OptimizerInstruction>& instructions);
			static void UpdateLabels(Boxx::List<OptimizerInstruction>& instructions);

			static void OffsetStackPointer(Boxx::List<OptimizerInstruction>& instructions, const Boxx::UInt start, const Boxx::UInt end, const Boxx::UInt offset);
			static Kiwi::InstructionType OppositeComp(const Kiwi::InstructionType type);

			static Boxx::UInt NextAssign(Boxx::List<OptimizerInstruction>& instructions, Boxx::UInt index, const Kiwi::Argument& arg);
			static Boxx::UInt NextGet(Boxx::List<OptimizerInstruction>& instructions, Boxx::UInt index, const Kiwi::Argument& arg);
			static Boxx::UInt NextUse(Boxx::List<OptimizerInstruction>& instructions, Boxx::UInt index, const Kiwi::Argument& arg);
			static Boxx::UInt UseCount(Boxx::List<OptimizerInstruction>& instructions, Boxx::UInt start, Boxx::UInt end, const Kiwi::Argument& arg);
		};
	}
}