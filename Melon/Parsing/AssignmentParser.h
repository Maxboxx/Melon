#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		///B AssignmentParser
		/// Parses an assignment statement
		class AssignmentParser {
		public:
			////B Assignement Flags
			enum class Flags : Boxx::UByte {
				////T Values
				////M
				None = 0,
				Single = 1,
				NewAssign = 2,
				MethodCall = 4
				////M
			};

			static Nodes::NodePtr Parse(ParsingInfo& info, const Flags flags = Flags::None);

		private:
			static Boxx::List<Symbols::ScopeList> ParseTypes(ParsingInfo& info);
			static bool ValidateTypes(ParsingInfo& info, Boxx::List<Symbols::ScopeList>& types, const Flags flags = Flags::None);
		};

		BOXX_ENUM_FLAGS(AssignmentParser::Flags);
	}
}