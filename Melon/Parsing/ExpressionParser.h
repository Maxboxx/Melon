#pragma once

#include "Parser.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for expressions.
		class ExpressionParser {
		public:
			/// Parses an expression.
			static Ptr<Nodes::Expression> Parse(ParsingInfo& info, const bool statement = false);

			/// Gets the precedence of a specific operator.
			static Boxx::UByte Precedence(const TokenType op);

			/// {true} if the specified operator is a binary operator.
			static bool IsBinaryOperator(const TokenType op);

			/// {true} if the specified operator is a logic gate.
			static bool IsLogic(const TokenType type);

		private:
			static Ptr<Nodes::Expression> ParseBinaryOperand(ParsingInfo& info, const bool statement = false);
			static Ptr<Nodes::Expression> ParseUnaryOperand(ParsingInfo& info, const bool statement = false);
			static Ptr<Nodes::Expression> ParseChainOperand(ParsingInfo& info, const bool statement = false);
		};
	}
}
