#pragma once

#include "Parser.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for method calls.
		class MethodCallParser {
		public:
			/// Parses a method call.
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}
