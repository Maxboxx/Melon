#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {
		class ConvertNode : public Node {
		public:
			NodePtr node;
			Symbols::ScopeList type;
			bool isExplicit = false;

			ConvertNode(Symbols::Symbol* const scope, const FileInfo& file);
			~ConvertNode();

			virtual Symbols::TypeSymbol* Type() const override;
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