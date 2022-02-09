#pragma once

#include "Parser.h"

#include "Boxx/Tuple.h"
#include "Boxx/Optional.h"

#include "Melon/Nodes/LoopStatement.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for {if}, {while}, and {for} statements.
		class LoopParser {
		public:
			/// Parser for {if}, {while}, or {for} statement.
			static Ptr<Nodes::LoopStatement> Parse(ParsingInfo& info);

		private:
			static bool ParseSegment(Nodes::LoopStatement::LoopSegment& ls, const Boxx::String& value, ParsingInfo& info);

			static bool ParseIf(Nodes::LoopStatement::LoopSegment& ls, const Boxx::String& value, ParsingInfo& info);
			static bool ParseWhile(Nodes::LoopStatement::LoopSegment& ls, const Boxx::String& value, ParsingInfo& info);
			static bool ParseFor(Nodes::LoopStatement::LoopSegment& ls, const Boxx::String& value, ParsingInfo& info);
			static bool ParseNone(Nodes::LoopStatement::LoopSegment& ls, const Boxx::String& value, ParsingInfo& info);

			static Boxx::Tuple<Boxx::Optional<Symbols::Name>, Ptr<Nodes::Condition>> ParseForCondition(ParsingInfo& info);
			static Boxx::Tuple<Boxx::Optional<Symbols::Name>, Ptr<Nodes::Node>> ParseForStep(ParsingInfo& info);

			static bool IsValidSegmentType(const TokenType t, Weak<Nodes::LoopStatement> loop);
			static bool IsLoop(const TokenType t);
			static bool IsLoopStart(const TokenType t);
			static bool IsLoopMiddle(const TokenType t);
			static Nodes::LoopStatement::LoopType GetLoopType(const TokenType t);
			static bool IsLoopAlso(const TokenType t);
		};
	}
}