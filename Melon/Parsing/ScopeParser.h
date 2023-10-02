#pragma once

#include "Parser.h"

#include "Melon/Nodes/Expression.h"
#include "Melon/Nodes/Statements.h"

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
			static Ptr<Nodes::Statements> Parse(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, const bool required = true);

			/// Parses a scope with an optional {end} keyword.
			///p Uses either {ParseBlockNoEnd} or {ParseSingle} to parse the scope. 
			///p This function does not modify {info.scope} or {info.scopeCount}.
			///A scopeStart: Used by {ParseBlockNoEnd}.
			///A scopeInfo: Used by {ParseBlockNoEnd} and {ParseSingle}.
			///A needsEnd: Output. {true} if the scope needs to be ended with end.
			///A required: {true} if the scope is required.
			static Ptr<Nodes::Statements> ParseNoEnd(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, bool& needsEnd, const bool required = true);

			/// Parses an expression scope.
			///p Uses either {ParseExpressionBlock} or {ParseExpressionSingle} to parse the scope. 
			///p This function does not modify {info.scope} or {info.scopeCount}.
			///A scopeStart: Used by {ParseExpressionBlock}.
			///A scopeInfo: Used by {ParseExpressionBlock} and {ParseExpressionSingle}.
			///A required: {true} if the scope is required.
			static Ptr<Nodes::Expression> ParseExpression(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, const bool required = true);

			/// Parses an expression scope without the {end} keyword.
			///p Uses either {ParseExpressionBlockNoEnd} or {ParseExpressionSingle} to parse the scope. 
			///p This function does not modify {info.scope} or {info.scopeCount}.
			///A scopeStart: Used by {ParseExpressionBlockNoEnd}.
			///A scopeInfo: Used by {ParseExpressionBlockNoEnd} and {ParseExpressionSingle}.
			///A needsEnd: Output. {true} if the scope needs to be ended with end.
			///A required: {true} if the scope is required.
			static Ptr<Nodes::Expression> ParseExpressionNoEnd(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, bool& needsEnd, const bool required = true);
		
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
				Boxx::UInt blockLine{};

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

		private:
			static Ptr<Nodes::Statements> Parse(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, bool required, bool requireEnd, bool& hasEnd);
			static Ptr<Nodes::Expression> ParseExpression(ParsingInfo& info, const TokenType scopeStart, const Info& scopeInfo, bool required, bool requireEnd, bool& hasEnd);
		};
	}
}
