#pragma once

#include "Parser.h"

///N Melon::Parsing
namespace Melon {
	namespace Nodes {
		class Assignment;
	}

	namespace Parsing {
		/// Parser for assignment statements.
		class AssignmentParser {
		public:
			/// Flags for an assignment.
			enum class Flags : Boxx::UByte {
				/// Parses every type of assignment.
				None = 0,

				/// Only parses single assignments.
				Single = 1,

				/// Only parses assignment of new variables.
				NewAssign = 2,

				// TODO: Figure out what this does.
				MethodCall = 4
			};

			/// Parses an assignment statement.
			static Ptr<Nodes::Assignment> Parse(ParsingInfo& info, const Flags flags = Flags::None);

		private:
			static Boxx::List<Symbols::NameList> ParseTypes(ParsingInfo& info);
			static bool ValidateTypes(ParsingInfo& info, Boxx::List<Symbols::NameList>& types, const Flags flags = Flags::None);
			static void ParseVariables(ParsingInfo& info, Boxx::List<Symbols::NameList>& types, Ptr<Nodes::Assignment>& assign, Boxx::List<Boxx::Tuple<Symbols::Name, Symbols::Symbol*>>& symbols, const bool singleType);
			static void ParseExpressions(ParsingInfo& info, Ptr<Nodes::Assignment>& assign);
		};

		BOXX_ENUM_FLAGS(AssignmentParser::Flags);
	}
}