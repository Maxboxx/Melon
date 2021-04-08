#pragma once

#include "Parser.h"

#include "Boxx/Tuple.h"
#include "Boxx/List.h"

///N Melon::Parsing
namespace Melon {
	namespace Nodes {
		class SwitchNode;
	}

	namespace Parsing {
		/// Parser for {switch} statements and expressions.
		class SwitchParser {
		public:
			/// Parses a {switch} statement.
			static Nodes::NodePtr ParseStatement(ParsingInfo& info);

			/// Parses a {switch} expression.
			static Nodes::NodePtr ParseExpression(ParsingInfo& info);

		private:
			static Boxx::List<Nodes::NodePtr> ParseCaseExpressions(ParsingInfo& info);

			static void ParseStatementCases(ParsingInfo& info, Boxx::Pointer<Nodes::SwitchNode>& switchNode);
			static bool ParseStatementCase(ParsingInfo& info, Boxx::Pointer<Nodes::SwitchNode>& switchNode);
			static bool ParseStatementDefault(ParsingInfo& info, Boxx::Pointer<Nodes::SwitchNode>& switchNode);

			static void ParseExpressionCases(ParsingInfo& info, Boxx::Pointer<Nodes::SwitchNode>& switchNode);
			static bool ParseExpressionCase(ParsingInfo& info, Boxx::Pointer<Nodes::SwitchNode>& switchNode);
			static bool ParseExpressionDefault(ParsingInfo& info, Boxx::Pointer<Nodes::SwitchNode>& switchNode);
		};
	}
}
