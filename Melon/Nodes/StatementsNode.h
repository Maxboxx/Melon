#pragma once

#include "Node.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for statements.
		class StatementsNode : public Node {
		public:
			/// The list of statements.
			Boxx::List<NodePtr> statements;

			StatementsNode(Symbols::Symbol* const scope, const FileInfo& file);
			~StatementsNode();

			virtual Boxx::UInt GetSize() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

			static bool HasSpaceAround(const NodePtr& node);

		protected:
			virtual Symbols::ScopeList FindSideEffectScope(const bool assign);
		};
	}
}