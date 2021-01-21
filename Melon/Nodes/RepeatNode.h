#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {
		class RepeatNode : public Node {
		public:
			NodePtr condition;
			NodePtr content;

			RepeatNode(Symbols::Symbol* const scope, const FileInfo& file);
			~RepeatNode();

			virtual Boxx::UInt GetSize() const override;
			virtual bool IsScope() const override;
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