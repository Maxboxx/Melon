#pragma once

#include "Expression.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for a character.
		class Character : public Expression {
		public:
			/// The character value.
			Boxx::UInt value;

			Character(const FileInfo& file);
			~Character();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}