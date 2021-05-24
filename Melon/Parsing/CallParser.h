#pragma once

#include "Parser.h"

#include "Melon/Nodes/CallNode.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for function calls.
		class CallParser {
		public:
			/// Parses a function call.
			static Nodes::NodePtr Parse(ParsingInfo& info);

		private:
			static Nodes::CallNode::ArgAttributes ParseArgumentAttributes(ParsingInfo& info);
		};
	}
}
