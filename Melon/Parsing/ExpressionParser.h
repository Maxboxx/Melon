#pragma once

#include "Parser.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for expressions.
		class ExpressionParser {
		public:
			/// Parses an expression.
			static Nodes::NodePtr Parse(ParsingInfo& info, const bool statement = false);

			/// Gets the precedence of a specific operator.
			static Boxx::UByte Precedence(const TokenType op);

			/// {true} if the specified operator is a binary operator.
			static bool IsBinaryOperator(const TokenType op);

			/// {true} if the specified operator is a logic gate.
			static bool IsLogic(const TokenType type);

		private:
			static Nodes::NodePtr ParseOperand(ParsingInfo& info, const bool statement = false);
			static Nodes::NodePtr ParseRawValue(ParsingInfo& info, const bool statement = false);
			static Nodes::NodePtr ParseValue(ParsingInfo& info, const bool statement = false);
		};
	}
}
