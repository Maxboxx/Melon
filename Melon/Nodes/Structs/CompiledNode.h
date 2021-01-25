#pragma once

#include "Melon/Optimizing/OptimizerInstruction.h"

namespace Melon {
	namespace Nodes {
		///B CompiledNode
		/// Contains info about a compiled node
		struct CompiledNode {
			///T Instructions
			/// All instructions for the node
			Boxx::List<Optimizing::OptimizerInstruction> instructions;

			///T Argument
			/// The argument to use if the node returns something
			Kiwi::Argument argument;

			///T Size
			/// The size in bytes of the returned value;
			Boxx::UByte size;

			///T Add instruction
			/// Adds a list of instructions to the instruction list
			void AddInstructions(const Boxx::List<Optimizing::OptimizerInstruction>& instructions) {
				for (const Optimizing::OptimizerInstruction& inst : instructions) {
					this->instructions.Add(inst);
				}
			}
		};
	}
}
