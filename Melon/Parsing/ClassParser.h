#pragma once

#include "Parser.h"

#include "Melon/Nodes/Assignment.h"
#include "Melon/Nodes/ClassStatement.h"

///N Melon::Parsing
namespace Melon {
	namespace Nodes {
		class ClassStatement;
	}

	namespace Parsing {
		/// Parser for classes.
		class ClassParser {
		public:
			/// Parses a class.
			static Ptr<Nodes::ClassStatement> Parse(ParsingInfo& info);

		private:
			static Ptr<Nodes::ClassStatement> ParseName(ParsingInfo& info, const Boxx::UInt structLine);
		};
	}
}