#pragma once

#include "Parser.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for scopes.
		class ScopeParser {
		public:
			/// Parses a scope.
			///p Uses either {ParseBlock} or {ParseSingle} to parse the scope. 
			///p This function does not modify {info.scope} or {info.scopeCount}.
			///A scopeStart: Used by {ParseBlock}.
			///A block: Used by {ParseBlock}.
			///A blockLine: Used by {ParseBlock}.
			static Nodes::NodePtr Parse(ParsingInfo& info, const TokenType scopeStart, const Boxx::String& block, const Boxx::UInt blockLine);

			/// Parses a scope with multiple statements.
			///p The scope starts with {scopeStart} and ends with {TokenType::End}.
			///p This function does not modify {info.scope} or {info.scopeCount}.
			///A scopeStart: The start token of the scope.
			///p If this value is {TokenType::None}, the scope starts with nothing and ends with {TokenType::End}.
			///A block: The block name to use for errors.
			///A blockLine: The block line to use for errors.
			static Nodes::NodePtr ParseBlock(ParsingInfo& info, const TokenType scopeStart, const Boxx::String& block, const Boxx::UInt blockLine);

			/// Parses a scope with a single statement.
			///p The scope starts with {TokenType::Arrow} and ends after the statement.
			///p This function does not modify {info.scope} or {info.scopeCount}.
			static Nodes::NodePtr ParseSingle(ParsingInfo& info);

			/// Parses an expression scope.
			///p Uses either {ParseExpressionBlock} or {ParseExpressionSingle} to parse the scope. 
			///p This function does not modify {info.scope} or {info.scopeCount}.
			///A scopeStart: Used by {ParseExpressionBlock}.
			///A block: Used by {ParseExpressionBlock}.
			///A blockLine: Used by {ParseExpressionBlock}.
			static Nodes::NodePtr ParseExpression(ParsingInfo& info, const TokenType scopeStart, const Boxx::String& block, const Boxx::UInt blockLine);

			/// Parses a scope with multiple statements.
			///p The scope starts with {scopeStart} and ends with {TokenType::End}.
			///p This function does not modify {info.scope} or {info.scopeCount}.
			///A scopeStart: The start token of the scope.
			///p If this value is {TokenType::None}, the scope starts with nothing and ends with {TokenType::End}.
			///A block: The block name to use for errors.
			///A blockLine: The block line to use for errors.
			static Nodes::NodePtr ParseExpressionBlock(ParsingInfo& info, const TokenType scopeStart, const Boxx::String& block, const Boxx::UInt blockLine);

			/// Parses a scope with a single statement.
			///p The scope starts with {TokenType::Arrow} and ends after the statement.
			///p This function does not modify {info.scope} or {info.scopeCount}.
			static Nodes::NodePtr ParseExpressionSingle(ParsingInfo& info);
		};
	}
}
