#pragma once

#include "Parser.h"

#include "Boxx/Tuple.h"
#include "Boxx/List.h"

#include "Melon/Nodes/SwitchStatement.h"
#include "Melon/Nodes/SwitchExpression.h"

///N Melon::Parsing
namespace Melon {
	namespace Nodes {
		class SwitchStatement;
	}

	namespace Parsing {
		/// Parser for {switch} statements and expressions.
		class SwitchParser {
		public:
			/// Parses a {switch} statement.
			static Ptr<Nodes::SwitchStatement> ParseStatement(ParsingInfo& info);

			/// Parses a {switch} expression.
			static Ptr<Nodes::SwitchExpression> ParseExpression(ParsingInfo& info, const bool returnOnError = true);

		private:
			static Boxx::List<Ptr<Nodes::Expression>> ParseCaseExpressions(ParsingInfo& info, bool& error);

			static void ParseStatementCases(ParsingInfo& info, Ptr<Nodes::SwitchStatement>& switchNode);
			static bool ParseStatementCase(ParsingInfo& info, Ptr<Nodes::SwitchStatement>& switchNode);
			static bool ParseStatementDefault(ParsingInfo& info, Ptr<Nodes::SwitchStatement>& switchNode);

			static void ParseExpressionCases(ParsingInfo& info, Ptr<Nodes::SwitchExpression>& switchNode, bool& error);
			static bool ParseExpressionCase(ParsingInfo& info, Ptr<Nodes::SwitchExpression>& switchNode, bool& error);
			static bool ParseExpressionDefault(ParsingInfo& info, Ptr<Nodes::SwitchExpression>& switchNode, bool& error);
		};
	}
}
