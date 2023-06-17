#pragma once

#include "Expression.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for arrays.
		class Array : public Expression {
		public:
			/// The array type.
			Symbols::NameList type;

			Array(Symbols::Symbol* scope, const FileInfo& file);
			~Array();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}