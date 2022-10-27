#pragma once

#include "Expression.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for a kiwi variable.
		class KiwiVariable : public Expression {
		public:
			/// The kiwi variable name.
			Boxx::String var;

			/// The absolute type name for the variable.
			Symbols::NameList type;

			/// Create from kiwi argument.
			KiwiVariable(const Boxx::String& var, const Symbols::NameList& type);

			virtual Symbols::TypeSymbol* Type() const override;
			virtual CompiledNode Compile(OldCompileInfo& info) override;
			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}