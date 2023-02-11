#pragma once

#include "Statement.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// A node that does not get compiled.
		class EmptyStatement : public Statement {
		public:
			/// This statement does not get compiled but the {ToMelon()} method is called for it.
			Weak<Statement> statement = nullptr;

			EmptyStatement();
			~EmptyStatement();

			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}
