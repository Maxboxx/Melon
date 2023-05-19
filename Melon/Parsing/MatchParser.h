#pragma once

#include "Parser.h"

#include "Boxx/Tuple.h"
#include "Boxx/List.h"

#include "Melon/Nodes/MatchStatement.h"
#include "Melon/Nodes/MatchExpression.h"

///N Melon::Parsing
namespace Melon {
	namespace Nodes {
		class MatchStatement;
	}

	namespace Parsing {
		/// Parser for {match} statements and expressions.
		class MatchParser {
		public:
			/// Parses a {switch} statement.
			static Ptr<Nodes::MatchStatement> ParseStatement(ParsingInfo& info);

			/// Parses a {switch} expression.
			static Ptr<Nodes::MatchExpression> ParseExpression(ParsingInfo& info, const bool returnOnError = true);

		private:
			static Boxx::List<Ptr<Nodes::Expression>> ParseCaseExpressions(ParsingInfo& info, bool& error);

			static void ParseStatementCases(ParsingInfo& info, Ptr<Nodes::MatchStatement>& switchNode);
			static bool ParseStatementCase(ParsingInfo& info, Ptr<Nodes::MatchStatement>& switchNode);
			static bool ParseStatementDefault(ParsingInfo& info, Ptr<Nodes::MatchStatement>& switchNode);

			static void ParseExpressionCases(ParsingInfo& info, Ptr<Nodes::MatchExpression>& switchNode, bool& error);
			static bool ParseExpressionCase(ParsingInfo& info, Ptr<Nodes::MatchExpression>& switchNode, bool& error);
			static bool ParseExpressionDefault(ParsingInfo& info, Ptr<Nodes::MatchExpression>& switchNode, bool& error);
		};
	}
}
