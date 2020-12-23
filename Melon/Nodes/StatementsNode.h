#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B StatementsNode
		/// Node for statements
		class StatementsNode : public Node {
		public:

			///T List of statements
			Boxx::List<NodePtr> statements;

			StatementsNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~StatementsNode();

			virtual Boxx::UInt GetSize() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

			static bool HasSpaceAround(const NodePtr& node);

		protected:
			virtual Symbols::ScopeList FindSideEffectScope(const bool assign);
		};
	}
}