#pragma once

#include "Parser.h"

///N Melon::Parsing
namespace Melon {
	namespace Nodes {
		class StructStatement;
	}

	namespace Parsing {
		/// Parser for structs.
		class StructParser {
		public:
			/// Parses a struct.
			static Nodes::NodePtr Parse(ParsingInfo& info);

		private:
			static Boxx::Pointer<Nodes::StructStatement> ParseName(ParsingInfo& info, const Boxx::UInt structLine);
			static Nodes::NodePtr ParseFunction(ParsingInfo& info);
			static Nodes::NodePtr ParseVariable(ParsingInfo& info);
		};
	}
}