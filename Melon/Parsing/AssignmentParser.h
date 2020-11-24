#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class AssignmentParser {
		public:
			enum class Flags : Boxx::UByte {
				None = 0,
				Single = 1,
				NewAssign = 2,
				MethodCall = 4
			};

			static Nodes::NodePtr Parse(ParsingInfo& info, const Flags flags = Flags::None);

		private:
			static Boxx::List<Symbols::ScopeList> ParseTypes(ParsingInfo& info);
		};

		BOXX_ENUM_FLAGS(AssignmentParser::Flags);
	}
}