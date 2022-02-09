#pragma once

#include "Parser.h"

#include "Boxx/List.h"

#include "Melon/Nodes/IfExpression.h"

///N Melon::Parsing
namespace Melon {
	namespace Nodes {
		class IfExpression;
	}

	namespace Parsing {
		/// Parser for {if} expressions.
		class IfExpressionParser {
		public:
			/// Parses an {if} expression.
			static Ptr<Nodes::IfExpression> Parse(ParsingInfo& info, const bool returnOnError = false);

		private:
			static bool ParseSimple(ParsingInfo& info, Weak<Nodes::IfExpression> node, const Boxx::UInt line, bool& error);
			static bool ParseFull(ParsingInfo& info, Weak<Nodes::IfExpression> node, const Boxx::UInt line, bool& error);
		};
	}
}