#pragma once

#include "Boxx/List.h"
#include "Boxx/Set.h"
#include "Boxx/Optional.h"

#include "Parser.h"

namespace Melon {
	namespace Parsing {
		class FunctionParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info, const bool isPlain = true);

		private:
			struct FunctionHead {
				Boxx::Set<Symbols::SymbolAttribute> attributes;
				bool isMethod;
				bool isOperator;
				Symbols::Scope name;
				Boxx::List<Symbols::ScopeList> retrunTypes;

				~FunctionHead() {}
			};

			static Boxx::Optional<Symbols::Scope> ParseFunctionName(ParsingInfo& info, const bool isPlain);
			static Boxx::Optional<Symbols::Scope> ParseOperatorName(ParsingInfo& info);
			static Boxx::Optional<FunctionHead>   ParseFunctionHead(ParsingInfo& info, const bool isPlain);
		};
	}
}