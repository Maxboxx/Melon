#pragma once

#include "CompiledNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		///H Structs

		/// Contains information about the stack.
		struct StackPtr {
			/// The stack frame position.
			Boxx::UInt frame = 0;

			/// The position of the top of the stack.
			Boxx::Long top = 0;

			/// The byte size of a pointer.
			static const Boxx::UByte ptrSize = 8;

			///A bytes: The number of bytes to push on the stack.
			void Push(const Boxx::UInt bytes) {
				top += bytes;
			}

			///A bytes: The number of bytes to pop from stack.
			void Pop(const Boxx::UInt bytes) {
				top -= bytes;
			}

			///A bytes: The number of bytes to push on the stack frame.
			void PushFrame(const Boxx::UInt bytes) {
				frame += bytes;
			}

			///A bytes: The number of bytes to pop from the stack frame.
			void PopFrame(const Boxx::UInt bytes) {
				frame -= bytes;
			}

			/// Pushes the top of the stack and also pushes the frame if it runs out of space.
			void PushExpr(const Boxx::UInt bytes, CompiledNode& cn) {
				top += bytes;

				if (top > frame) {
					cn.instructions.Add(KiwiOld::Instruction(KiwiOld::InstructionType::Push, top - frame));
					frame = top;
				}
			}

			/// Pops the frame back to a previous position.
			void PopExpr(const Boxx::UInt frame, CompiledNode& cn) {
				if (this->frame > frame) {
					cn.instructions.Add(KiwiOld::Instruction(KiwiOld::InstructionType::Pop, this->frame - frame));
				}

				this->frame = frame;
			}

			/// Gets the offset between the top and frame.
			Boxx::Long Offset() const {
				return frame - top;
			}

			/// Gets the offset between {index} and frame.
			Boxx::Long Offset(const Boxx::Long index) const {
				return frame - index;
			}
		};
	}
}
