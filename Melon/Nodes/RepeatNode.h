#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {
		///T RepeatNode
		/// Node for reapeat statements
		class RepeatNode : public Node {
		public:
			///T Condition
			NodePtr condition;

			///T Content
			/// The loop body
			NodePtr content;

			RepeatNode(Symbols::Symbol* const scope, const FileInfo& file);
			~RepeatNode();

			virtual Boxx::UInt GetSize() const override;
			virtual bool IsScope() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::ScopeList FindSideEffectScope(const bool assign);
		};
	}
}