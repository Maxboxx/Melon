#pragma once

#include "Expression.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for a boolean value.
		class Boolean : public Expression {
		public:
			/// The boolean value.
			bool value = false;

			/// Creates a boolean node.
			Boolean(const FileInfo& file);
			~Boolean();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}