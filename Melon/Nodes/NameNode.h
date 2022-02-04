#pragma once

#include "ExpressionNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for a name.
		class NameNode : public Expression {
		public:
			/// The name of the node.
			Symbols::Name name;

			/// Used to ignore {ref}.
			bool ignoreRef = false;

			NameNode(Symbols::Symbol* const scope, const FileInfo& file);
			~NameNode();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual Symbols::Symbol* Symbol() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign);
		};
	}
}