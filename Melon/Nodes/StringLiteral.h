#pragma once

#include "Expression.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for a character.
		class StringLiteral : public Expression {
		public:
			/// The string value.
			Boxx::String value;

			StringLiteral(const FileInfo& file);
			~StringLiteral();

			virtual Symbols::TypeSymbol* Type(Symbols::TypeSymbol* expected = nullptr) const override;
			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}
