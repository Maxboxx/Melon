#pragma once

#include "CompiledNode.h"

namespace Melon {
	namespace Nodes {
		///B StackPtr
		/// Contains information about the stack pointer
		struct StackPtr {
			///H Members

			///T Stack frame
			Boxx::UInt frame = 0;

			///T Stack top
			Boxx::Long top = 0;

			///T Pointer size
			static const Boxx::UByte ptrSize = 8;

			///H Methods

			///T Push
			///A bytes: The number of bytes to push on the stack
			void Push(const Boxx::UInt bytes) {
				top += bytes;
			}

			///T Pop
			///A bytes: The number of bytes to pop from stack
			void Pop(const Boxx::UInt bytes) {
				top -= bytes;
			}

			///T Push frame
			///A bytes: The number of bytes to push on the stack frame
			void PushFrame(const Boxx::UInt bytes) {
				frame += bytes;
			}

			///T Pop frame
			///A bytes: The number of bytes to pop from the stack frame
			void PopFrame(const Boxx::UInt bytes) {
				frame -= bytes;
			}

			///T Push Expression
			/// Pushes the top of the stack and also pushes the frame if it runs out of space
			void PushExpr(const Boxx::UInt bytes, CompiledNode& cn) {
				top += bytes;

				if (top > frame) {
					cn.instructions.Add(Kiwi::Instruction(Kiwi::InstructionType::Push, top - frame));
					frame = top;
				}
			}

			///T Pop Expression
			/// Pops the frame back to a previous position
			void PopExpr(const Boxx::UInt frame, CompiledNode& cn) {
				if (this->frame > frame) {
					cn.instructions.Add(Kiwi::Instruction(Kiwi::InstructionType::Pop, this->frame - frame));
				}

				this->frame = frame;
			}

			///T Offset
			/// Gets the offset between the top and frame
			Boxx::Long Offset() const {
				return frame - top;
			}

			///T Offset
			/// Gets the offset between <code>index</code> and frame
			Boxx::Long Offset(const Boxx::Long index) const {
				return frame - index;
			}
		};
	}
}
