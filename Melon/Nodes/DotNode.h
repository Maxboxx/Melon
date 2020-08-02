#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B DotNode
		/// Node for the dot operator
		class DotNode : public Node {
		public:
	
			///T Node
			/// Node to perform the dot operation on
			NodePtr node;

			///T Name
			/// The name of the dot operator
			Symbols::Scope name;

			DotNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~DotNode();

			virtual Symbols::ScopeList Type() const override;
			virtual Symbols::Symbol GetSymbol() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfo& info) const override;
			virtual Boxx::Mango ToMango() const override;
		};
	}
}