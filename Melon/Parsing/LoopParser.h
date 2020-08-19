#pragma once

#include "Parser.h"

#include "Melon/Nodes/LoopNode.h"

namespace Melon {
	namespace Parsing {
		class LoopParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info);

		private:
			static void ParseIf(Nodes::LoopNode::LoopSegment& ls, const Boxx::String& value, ParsingInfo& info);
			static void ParseWhile(Nodes::LoopNode::LoopSegment& ls, const Boxx::String& value, ParsingInfo& info);
			static void ParseFor(Nodes::LoopNode::LoopSegment& ls, const Boxx::String& value, ParsingInfo& info);
			static void ParseNone(Nodes::LoopNode::LoopSegment& ls, const Boxx::String& value, ParsingInfo& info);

			static bool IsLoop(const TokenType t);
			static bool IsLoopStart(const TokenType t);
			static bool IsLoopMiddle(const TokenType t);
			static Nodes::LoopNode::LoopType GetLoopType(const TokenType t);
			static bool IsLoopAlso(const TokenType t);
		};
	}
}