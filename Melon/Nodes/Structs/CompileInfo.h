#pragma once

#include "LoopInfo.h"

#include "Kiwi/KiwiProgram.h"

#include "Boxx/Stack.h"

#include "Melon/Symbols/TypeSymbol.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		///H Structs

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

			/// The optional chain stack.
			Boxx::Stack<Boxx::String> optionalChains;

			~CompileInfo() {}

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

			/// Adds an instruction to the current block.
			void AddInstruction(Ptr<Kiwi::Instruction> instruction) {
				currentBlock->AddInstruction(instruction);
			}

			/// Pushes a loop.
			void PushLoop(const LoopInfo& loop) {
				loops.Add(loop);
				scopes.Add(loop);
			}

			/// Pushes a scope.
			void PushScope(const LoopInfo& scope) {
				scopes.Add(scope);
			}

			/// Pops a loop.
			LoopInfo PopLoop() {
				scopes.RemoveLast();

				LoopInfo loop = loops.Last();
				loops.RemoveLast();
				return loop;
			}

			/// Pops a scope.
			LoopInfo PopScope() {
				LoopInfo scope = scopes.Last();
				scopes.RemoveLast();
				return scope;
			}

			/// Peeks the current loop.
			LoopInfo PeekLoop() {
				return loops.Last();
			}

			/// Peeks the current scope.
			LoopInfo PeekScope() {
				return scopes.Last();
			}

			/// Peeks a loop.
			LoopInfo PeekLoop(int depth) {
				return loops.Last(depth);
			}

			/// Peeks a scope.
			LoopInfo PeekScope(int depth) {
				return scopes.Last(depth);
			}

			/// Pushes an expected type.
			void PushExpectedType(Symbols::TypeSymbol* type) {
				expectedTypes.Push(type);
			}

			/// Pops an expected type.
			void PopExpectedType() {
				expectedTypes.Pop();
			}

			/// Peeks an expected type.
			Symbols::TypeSymbol* PeekExpectedType() {
				if (expectedTypes.Count() <= 0) return nullptr;
				return expectedTypes.Peek();
			}

		private:
			Boxx::UInt regIndex = 0;
			Boxx::UInt lblIndex = 0;
			
			Boxx::List<LoopInfo> loops;
			Boxx::List<LoopInfo> scopes;

			Boxx::Stack<Symbols::TypeSymbol*> expectedTypes;
		};
	}
}
