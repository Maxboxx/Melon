#pragma once

#include "StackPtr.h"

#include "Kiwi/KiwiProgram.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		///H Structs

		/// Contains label info.
		struct LoopInfo {
			/// The continue label.
			Boxx::String continueLabel;

			/// The true label.
			Boxx::String trueLabel;

			/// The false label.
			Boxx::String falseLabel;

			/// The end label.
			Boxx::String endLabel;

			~LoopInfo() {}
		};
	}
}
