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

			DotNode(Symbols::Symbol* const scope, const FileInfo& file);
			~DotNode();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual Symbols::Symbol* GetSymbol() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual void Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::ScopeList FindSideEffectScope(const bool assign);
		};
	}
}