#pragma once

#include "Expression.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for a name.
		class NameExpression : public Expression {
		public:
			/// The name of the node.
			Symbols::Name name;

			/// Used to ignore {ref}.
			bool ignoreRef = false;

			NameExpression(Symbols::Symbol* const scope, const FileInfo& file);
			~NameExpression();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual Symbols::Symbol* Symbol() const override;
			virtual CompiledNode Compile(OldCompileInfo& info) override;
			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign);
		};
	}
}