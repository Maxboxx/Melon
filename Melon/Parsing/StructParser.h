#pragma once

#include "Parser.h"

namespace Melon {
	namespace Nodes {
		class StructNode;
	}

	namespace Parsing {
		class StructParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info);

		private:
			static Boxx::Pointer<Nodes::StructNode> ParseName(ParsingInfo& info, const Boxx::UInt structLine);
			static Nodes::NodePtr ParseFunction(ParsingInfo& info);
			static Nodes::NodePtr ParseVariable(ParsingInfo& info);
		};
	}
}