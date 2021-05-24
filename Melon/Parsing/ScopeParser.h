#pragma once

#include "Parser.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for scopes.
		class ScopeParser {
		public:
			struct Info;

			/// Parses a scope.
			///p Uses either {ParseBlock} or {ParseSingle} to parse the scope. 
			///p This function does not modify {info.scope} or {info.scopeCount}.
			///A scopeStart: Used by {ParseBlock}.
			///A scopeInfo: Used by {ParseBlock} and {ParseSingle}.
			///A required: {true} if the scope is required.
			static Nodes::NodePtr Parse(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, const bool required = false);

			/// Parses a scope without the {end} keyword.
			///p Uses either {ParseBlockNoEnd} or {ParseSingle} to parse the scope. 
			///p This function does not modify {info.scope} or {info.scopeCount}.
			///A scopeStart: Used by {ParseBlockNoEnd}.
			///A scopeInfo: Used by {ParseBlockNoEnd} and {ParseSingle}.
			///A required: {true} if the scope is required.
			static Nodes::NodePtr ParseNoEnd(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, const bool required = false);

			/// Parses a scope with multiple statements.
			///p The scope starts with {scopeStart} and ends with {TokenType::End}.
			///p This function does not modify {info.scope} or {info.scopeCount}.
			///A scopeStart: The start token of the scope.
			///p If this value is {TokenType::None}, the scope starts with nothing.
			///A scopeInfo: Info used for errors.
			///A required: {true} if the scope is required.
			static Nodes::NodePtr ParseBlock(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, const bool required = false);

			/// Parses a scope with multiple statements without the {end} keyword.
			///p The scope starts with {scopeStart}.
			///p This function does not modify {info.scope} or {info.scopeCount}.
			///A scopeStart: The start token of the scope.
			///p If this value is {TokenType::None}, the scope starts with nothing.
			///A scopeInfo: Info used for errors.
			///A required: {true} if the scope is required.
			static Nodes::NodePtr ParseBlockNoEnd(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, const bool required = false);
			
			/// Parses a scope with a single statement.
			///p The scope starts with {TokenType::Arrow} and ends after the statement.
			///p This function does not modify {info.scope} or {info.scopeCount}.
			///A scopeInfo: Info for errors.
			///A required: {true} if the scope is required.
			static Nodes::NodePtr ParseSingle(ParsingInfo& info, const Info& scopeInfo, const bool required = false);

			/// Parses an expression scope.
			///p Uses either {ParseExpressionBlock} or {ParseExpressionSingle} to parse the scope. 
			///p This function does not modify {info.scope} or {info.scopeCount}.
			///A scopeStart: Used by {ParseExpressionBlock}.
			///A scopeInfo: Used by {ParseExpressionBlock} and {ParseExpressionSingle}.
			///A required: {true} if the scope is required.
			static Nodes::NodePtr ParseExpression(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, const bool required = false);

			/// Parses an expression scope without the {end} keyword.
			///p Uses either {ParseExpressionBlockNoEnd} or {ParseExpressionSingle} to parse the scope. 
			///p This function does not modify {info.scope} or {info.scopeCount}.
			///A scopeStart: Used by {ParseExpressionBlockNoEnd}.
			///A scopeInfo: Used by {ParseExpressionBlockNoEnd} and {ParseExpressionSingle}.
			///A required: {true} if the scope is required.
			static Nodes::NodePtr ParseExpressionNoEnd(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, const bool required = false);

			/// Parses an expression block scope.
			///p The scope starts with {scopeStart} and ends with {TokenType::End}.
			///p This function does not modify {info.scope} or {info.scopeCount}.
			///A scopeStart: The start token of the scope.
			///p If this value is {TokenType::None}, the scope starts with nothing.
			///A scopeInfo: Info used for errors.
			///A required: {true} if the scope is required.
			static Nodes::NodePtr ParseExpressionBlock(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, const bool required = false);

			/// Parses an expression block scope without the {end} keyword.
			///p The scope starts with {scopeStart}.
			///p This function does not modify {info.scope} or {info.scopeCount}.
			///A scopeStart: The start token of the scope.
			///p If this value is {TokenType::None}, the scope starts with nothing.
			///A scopeInfo: Info used for errors.
			///A required: {true} if the scope is required.
			static Nodes::NodePtr ParseExpressionBlockNoEnd(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, const bool required = false);

			/// Parses a single expression scope.
			///p The scope starts with {TokenType::Arrow} and ends after the expression.
			///p This function does not modify {info.scope} or {info.scopeCount}.
			///A scopeInfo: Info for errors.
			///A required: {true} if the scope is required.
			static Nodes::NodePtr ParseExpressionSingle(ParsingInfo& info, const Info& scopeInfo, const bool required = false);
		
			/// Info used for error messages.
			struct Info {
				/// The expected token to open the scope.
				///p This value will automatically be quoted.
				///p This value is only used by blocks.
				Boxx::String expected;

				/// The part before the opening of the scope.
				///p This value will not be automatically quoted.
				Boxx::String after;

				/// The block to end with the {end} keyword.
				///p This value will not be automatically quoted.
				Boxx::String block;

				/// The line of the block.
				Boxx::UInt blockLine;

				/// Creates an info object for scopes that are not required.
				Info() {
				
				}

				/// Creates an info object for single scopes that are required.
				Info(const Boxx::String& after) {
					this->after = after;
				}

				/// Creates a simple info object for required scopes.
				Info(const Boxx::String& expected, const Boxx::String& after) {
					this->expected  = expected;
					this->after     = after;
				}

				/// Creates a simple info object for scopes that end with the {end} keyword.
				Info(const Boxx::String& block, const Boxx::UInt blockLine) {
					this->block     = block;
					this->blockLine = blockLine;
				}

				/// Creates an info object for blocks that end with the {end} keyword.
				Info(const Boxx::String& expected, const Boxx::String& after, const Boxx::String& block, const Boxx::UInt blockLine) {
					this->expected  = expected;
					this->after     = after;
					this->block     = block;
					this->blockLine = blockLine;
				}

				~Info() {}
			};
		};
	}
}
