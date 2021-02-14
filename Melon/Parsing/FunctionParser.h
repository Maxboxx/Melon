#pragma once

#include "Boxx/List.h"
#include "Boxx/Set.h"
#include "Boxx/Optional.h"

#include "Parser.h"

#include "Melon/Symbols/TypeSymbol.h"
#include "Melon/Symbols/FunctionSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"

namespace Melon {
	namespace Parsing {
		class FunctionParser {
		public:
			static Nodes::NodePtr Parse(ParsingInfo& info, Symbols::TypeSymbol* const parent = nullptr);

		private:
			struct Argument {
				Symbols::Scope name;
				Symbols::ScopeList type;
				Symbols::VariableAttributes attributes;
			};

			struct FunctionHead {
				Symbols::FunctionAttributes attributes;
				bool isMethod;
				bool isOperator;
				Symbols::Scope name;
				Boxx::List<Argument> arguments;
				Boxx::List<Symbols::ScopeList> templateArgs;
				Boxx::List<Symbols::ScopeList> returnTypes;

				~FunctionHead() {}
			};

			static Boxx::Optional<Symbols::Scope> ParseFunctionName(ParsingInfo& info, const bool isPlain);
			static Boxx::Optional<Symbols::Scope> ParseOperatorName(ParsingInfo& info);
			static Boxx::Optional<Symbols::Scope> ParseName        (const bool isOperator, ParsingInfo& info, const bool isPlain);
			static Boxx::List<Symbols::ScopeList> ParseReturnTypes (ParsingInfo& info);
			static Symbols::FunctionAttributes    ParseAttributes  (ParsingInfo& info, const bool isPlain);
			static Boxx::List<Argument>           ParseArguments   (ParsingInfo& info);
			static Boxx::Optional<FunctionHead>   ParseFunctionHead(ParsingInfo& info, const bool isPlain);
		};
	}
}