#pragma once

#include "Parser.h"

#include "Melon/Nodes/Debug.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for debug statements.
		class DebugParser {
		public:
			/// Parses a debug statement.
			static Ptr<Nodes::Debug> Parse(ParsingInfo& info);
		};
	}
}