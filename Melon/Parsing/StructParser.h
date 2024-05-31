#pragma once

#include "Parser.h"

#include "Melon/Nodes/StructStatement.h"
#include "Melon/Nodes/Assignment.h"

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

			/// Parses a curly struct.
			static Ptr<Nodes::StructStatement> ParseCurly(const Symbols::Name& name, ParsingInfo& info);

		private:
			static Ptr<Nodes::StructStatement> ParseBody(Ptr<Nodes::StructStatement> statement, ParsingInfo& info, const Boxx::UInt structLine, bool curly);

			static Ptr<Nodes::StructStatement> ParseName(Boxx::Optional<Symbols::Name> name, ParsingInfo& info, const Boxx::UInt structLine);
		};
	}
}