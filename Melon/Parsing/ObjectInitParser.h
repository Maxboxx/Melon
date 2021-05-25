#pragma once

#include "Parser.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for object initializers.
		class ObjectInitParser {
		public:
			/// Parses an object initializer.
			static Nodes::NodePtr Parse(ParsingInfo& info);
		};
	}
}