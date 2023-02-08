#pragma once

#include "StackPtr.h"

#include "Kiwi/KiwiProgram.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		///H Structs

		/// Contains label info for a scope.
		struct LoopInfo {
			/// The continue label.
			Boxx::String continueLabel;

			/// The true label.
			Boxx::String trueLabel;

			/// The false label.
			Boxx::String falseLabel;

			/// The end label.
			Boxx::String endLabel;

			LoopInfo() {}

			LoopInfo(const Boxx::String& label) {
				continueLabel = label;
				trueLabel = label;
				falseLabel = label;
				endLabel = label;
			}

			LoopInfo(const Boxx::String& continueLabel, const Boxx::String& breakLabel) {
				this->continueLabel = continueLabel;
				trueLabel = breakLabel;
				falseLabel = breakLabel;
				endLabel = breakLabel;
			}

			LoopInfo(const Boxx::String& continueLabel, const Boxx::String& breakLabel, const Boxx::String& endLabel) {
				this->continueLabel = continueLabel;
				trueLabel = breakLabel;
				falseLabel = breakLabel;
				this->endLabel = endLabel;
			}

			LoopInfo(const Boxx::String& continueLabel, const Boxx::String& trueLabel, const Boxx::String& falseLabel, const Boxx::String& endLabel) {
				this->continueLabel = continueLabel;
				this->trueLabel = trueLabel;
				this->falseLabel = falseLabel;
				this->endLabel = endLabel;
			}

			~LoopInfo() {}
		};
	}
}
