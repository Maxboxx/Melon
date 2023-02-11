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

		private:
			static Ptr<Nodes::StructStatement> ParseName(ParsingInfo& info, const Boxx::UInt structLine);

			struct Variable {
				Boxx::List<Symbols::NameList> types;
				Boxx::List<Symbols::Name> varNames;

				~Variable() {};
			};

			static Boxx::Optional<Variable> ParseVariable(ParsingInfo& info);
		};
	}
}