#pragma once

#include "StackPtr.h"
#include "LoopInfo.h"

#include "Kiwi/KiwiProgram.h"

#include "Boxx/Stack.h"

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

			/// The current code block.
			Weak<Kiwi::CodeBlock> currentCodeBlock;

			/// The current instruction block.
			Weak<Kiwi::InstructionBlock> currentBlock;

			/// The return registers for the current function.
			Boxx::List<Boxx::String> returnRegisters;

			/// The loop stack.
			Boxx::Stack<LoopInfo> loops;

			/// Returns the name of a new register.
			Boxx::String NewRegister() {
				return Boxx::String::ToString(regIndex++);
			}

			/// Clears all registers.
			void ClearRegisters() {
				regIndex = 0;
			}

			/// Returns the name of a new label.
			Boxx::String NewLabel() {
				return 'L' + Boxx::String::ToString(lblIndex++);
			}

			/// Creates a new instruction block.
			void NewInstructionBlock(const Boxx::String& label) {
				currentCodeBlock->AddInstructionBlock(new Kiwi::InstructionBlock(label));
				currentBlock = currentCodeBlock->blocks.Last();
			}

			/// Sets the current code block.
			void SetCodeBlock(Weak<Kiwi::CodeBlock> codeBlock) {
				currentCodeBlock = codeBlock;
				currentBlock = codeBlock->mainBlock;
			}

		private:
			Boxx::UInt regIndex = 0;
			Boxx::UInt lblIndex = 0;
		};
	}
}
