#pragma once

#include "Parser.h"

#include "Boxx/Optional.h"

#include "Melon/Nodes/EnumStatement.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for enums.
		class EnumParser {
		public:
			/// Parses an enum.
			static Ptr<Nodes::EnumStatement> Parse(ParsingInfo& info);

		private:
			struct EnumValue {
				Symbols::Name name;
				Boxx::Optional<Symbols::NameList> type;
				Boxx::ULong   value;
				Boxx::UInt    line;
			};

			static Ptr<Nodes::EnumStatement> ParseName(ParsingInfo& info, const Boxx::UInt enumLine);
			static Boxx::List<EnumValue> ParseValues(ParsingInfo& info);
			static Boxx::Optional<EnumValue> ParseValue(ParsingInfo& info, Boxx::ULong& currentValue);
		};
	}
}