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
		};
	}
}