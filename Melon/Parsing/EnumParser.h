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
				Symbols::Scope name;
				Boxx::ULong   value;
			};

			static Nodes::NodePtr ParseFunction(ParsingInfo& info);
			static Boxx::List<EnumValue> ParseValues(ParsingInfo& info);
			static Boxx::Optional<EnumValue> ParseValue(ParsingInfo& info, Boxx::ULong& currentValue);
		};
	}
}