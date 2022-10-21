#pragma once

#include "Melon/Optimizing/OptimizerInstruction.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		///H Structs

		/// Contains info about a compiled node.
		struct CompiledNode {
			/// All instructions for the node.
			Boxx::List<Optimizing::OptimizerInstruction> instructions;

			/// The argument to use if the node returns something.
			KiwiOld::Argument argument;

			/// The size in bytes of the returned value.
			Boxx::UByte size;

			/// Adds a list of instructions to the instruction list.
			void AddInstructions(const Boxx::List<Optimizing::OptimizerInstruction>& instructions) {
				for (const Optimizing::OptimizerInstruction& inst : instructions) {
					this->instructions.Add(inst);
				}
			}
		};
	}
}
