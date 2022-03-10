#pragma once

#include "Parser.h"

#include "Melon/Nodes/CallExpression.h"
#include "Melon/Nodes/CallStatement.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for function calls.
		class CallParser {
		public:
			/// Parses a call.
			static Ptr<Nodes::CallExpression> Parse(ParsingInfo& info);

			/// Parses a call statement.
			static Ptr<Nodes::CallStatement>  ParseStatement(ParsingInfo& info);

		private:
			static Nodes::CallArgAttributes ParseArgumentAttributes(ParsingInfo& info);
		};
	}
}
