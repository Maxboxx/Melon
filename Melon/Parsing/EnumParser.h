#pragma once

#include "Parser.h"

#include "Boxx/Optional.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for enums.
		class EnumParser {
		public:
			/// Parses an enum.
			static Nodes::NodePtr Parse(ParsingInfo& info);

		private:
			struct EnumValue {
				Symbols::Name name;
				Boxx::ULong    value;
				Boxx::UInt     line;
			};

			static Nodes::NodePtr ParseFunction(ParsingInfo& info);
			static Boxx::List<EnumValue> ParseValues(ParsingInfo& info);
			static Boxx::Optional<EnumValue> ParseValue(ParsingInfo& info, Boxx::ULong& currentValue);
		};
	}
}