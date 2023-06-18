#pragma once

#include "Expression.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for a kiwi variable.
		class KiwiVariable : public Expression {
		public:
			/// The kiwi variable.
			Ptr<Kiwi::Variable> var;

			/// The absolute type name for the variable.
			Symbols::NameList type;

			/// Create from kiwi argument.
			KiwiVariable(Ptr<Kiwi::Variable> var, const Symbols::NameList& type);

			virtual Symbols::TypeSymbol* Type(Symbols::TypeSymbol* expected = nullptr) const override;
			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}