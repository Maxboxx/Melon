#pragma once

#include "Boxx/List.h"
#include "Boxx/Set.h"
#include "Boxx/Optional.h"

#include "Parser.h"

#include "Melon/Symbols/TypeSymbol.h"
#include "Melon/Symbols/FunctionSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"

///N Melon::Parsing
namespace Melon {
	namespace Parsing {
		/// Parser for functions.
		class FunctionParser {
		public:
			/// Parses a function.
			static Nodes::NodePtr Parse(ParsingInfo& info, Symbols::TypeSymbol* const parent = nullptr);

		private:
			struct Argument {
				Symbols::Name name;
				Symbols::NameList type;
				Symbols::VariableAttributes attributes;
			};

			struct FunctionHead {
				Symbols::FunctionAttributes attributes;
				bool isMethod;
				bool isOperator;
				Symbols::Name name;
				Boxx::List<Argument> arguments;
				Boxx::List<Symbols::NameList> templateArgs;
				Boxx::List<Symbols::NameList> returnTypes;

				~FunctionHead() {}
			};

			static Boxx::Optional<Symbols::Name> ParseFunctionName(ParsingInfo& info, const bool isPlain);
			static Boxx::Optional<Symbols::Name> ParseOperatorName(ParsingInfo& info);
			static Boxx::Optional<Symbols::Name> ParseName        (const bool isOperator, ParsingInfo& info, const bool isPlain);
			static Boxx::List<Symbols::NameList> ParseReturnTypes (ParsingInfo& info);
			static Symbols::FunctionAttributes    ParseAttributes  (ParsingInfo& info, const bool isPlain);
			static Boxx::List<Argument>           ParseArguments   (ParsingInfo& info);
			static Boxx::Optional<FunctionHead>   ParseFunctionHead(ParsingInfo& info, const bool isPlain);
		};
	}
}