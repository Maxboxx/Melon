#pragma once

#include "Parser.h"

#include "Boxx/Optional.h"

namespace Melon {
	namespace Parsing {
		class EnumParser {
		public:
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