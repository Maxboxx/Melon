#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class ExpressionParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info, const bool statement = false);

			static Boxx::UByte Precedence(const TokenType op);
			static bool IsBinaryOperator(const TokenType op);
			static bool IsLogic(const TokenType type);

		private:

			static Nodes::NodePtr ParseValue(ParsingInfo& info, const bool statement = false);
			static Nodes::NodePtr ParseRawValue(ParsingInfo& info, const bool statement = false);
			static Nodes::NodePtr ParseSingleValue(ParsingInfo& info, const bool statement = false);
		};
	}
}