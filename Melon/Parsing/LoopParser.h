#pragma once

#include "Parser.h"

#include "Melon/Nodes/LoopNode.h"

namespace Melon {
	namespace Parsing {
		class LoopParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info);

		private:
			static bool ParseIf(Nodes::LoopNode::LoopSegment& ls, const Boxx::String& value, ParsingInfo& info, const Boxx::UInt index);
			static bool ParseWhile(Nodes::LoopNode::LoopSegment& ls, const Boxx::String& value, ParsingInfo& info, const Boxx::UInt index);
			static bool ParseFor(Nodes::LoopNode::LoopSegment& ls, const Boxx::String& value, ParsingInfo& info, const Boxx::UInt index);
			static void ParseNone(Nodes::LoopNode::LoopSegment& ls, const Boxx::String& value, ParsingInfo& info, const Boxx::UInt index);

			static bool IsLoop(const TokenType t);
			static bool IsLoopStart(const TokenType t);
			static bool IsLoopMiddle(const TokenType t);
			static Nodes::LoopNode::LoopType GetLoopType(const TokenType t);
			static bool IsLoopAlso(const TokenType t);
		};
	}
}