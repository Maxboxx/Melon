#pragma once

#include "Parser.h"

#include "Boxx/List.h"

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
			static Nodes::NodePtr Parse(ParsingInfo& info, const bool returnOnError = false);

		private:
			static bool ParseSimple(ParsingInfo& info, Nodes::IfExpression* const node, const Boxx::UInt line, bool& error);
			static bool ParseFull(ParsingInfo& info, Nodes::IfExpression* const node, const Boxx::UInt line, bool& error);
		};
	}
}