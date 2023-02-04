#pragma once

#include "Expression.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for a kiwi value.
		class KiwiValue : public Expression {
		public:
			/// The kiwi value.
			Ptr<Kiwi::Value> value;

			/// The absolute type name for the value.
			Symbols::NameList type;

			/// Create from kiwi argument.
			KiwiValue(Ptr<Kiwi::Value> value, const Symbols::NameList& type);
			~KiwiValue();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual CompiledNode Compile(OldCompileInfo& info) override;
			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}