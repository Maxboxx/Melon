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

			NameNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~NameNode();

			virtual Symbols::ScopeList Type() const override;
			virtual Symbols::Symbol GetSymbol() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) override;
			virtual Boxx::Mango ToMango() const override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::ScopeList FindSideEffectScope(const bool assign);
		};
	}
}