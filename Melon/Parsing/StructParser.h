#pragma once

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class StructParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info);

		private:
			static Nodes::NodePtr ParseFunction(ParsingInfo& info);
			static Nodes::NodePtr ParseVariable(ParsingInfo& info);
		};
	}
}