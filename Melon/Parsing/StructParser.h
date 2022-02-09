#pragma once

#include "Parser.h"

#include "Melon/Nodes/StructStatement.h"
#include "Melon/Nodes/NewVariableNode.h"

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
			static Ptr<Nodes::StructStatement> Parse(ParsingInfo& info);

		private:
			static Ptr<Nodes::StructStatement> ParseName(ParsingInfo& info, const Boxx::UInt structLine);
			static Ptr<Nodes::NewVariableNode> ParseVariable(ParsingInfo& info);
		};
	}
}