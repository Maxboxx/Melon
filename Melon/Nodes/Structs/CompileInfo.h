#pragma once

#include "StackPtr.h"

#include "Kiwi/KiwiProgram.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		///H Structs

		/// Contains info about the compiliation.
		struct [[deprecated("Use the new compile info instead")]] OldCompileInfo {
			/// The next free label index.
			Boxx::UInt label = 0;

			/// The current stack and frame positions.
			StackPtr stack;

			/// The next free register index.
			Boxx::UByte index = 0;

			/// {true} if assignments to memory values are important.
			bool important = false;
		};

		/// Contains compile info.
		struct CompileInfo {
			/// The current kiwi program.
			Weak<Kiwi::KiwiProgram> program;

			/// The current instruction block.
			Weak<Kiwi::InstructionBlock> currentBlock;

			/// Creates a new register and returns the name of it.
			Boxx::String NewRegister() {
				return Boxx::String::ToString(regIndex++);
			}

			/// Clears all registers.
			void ClearRegisters() {
				regIndex = 0;
			}

		private:
			Boxx::UInt regIndex = 0;
		};
	}
}
