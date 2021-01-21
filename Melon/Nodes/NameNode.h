#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B NameNode
		/// Node for a name
		class NameNode : public Node {
		public:

			///T Name
			Symbols::Scope name;

			///T Ignore Ref
			/// Used to ignore ref
			bool ignoreRef = false;

			NameNode(Symbols::Symbol* const scope, const FileInfo& file);
			~NameNode();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual Symbols::Symbol* GetSymbol() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void Scan(ScanInfoStack& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::ScopeList FindSideEffectScope(const bool assign);
		};
	}
}