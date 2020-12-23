#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B PtrNode
		/// Node for pointers
		class PtrNode : public Node {
		public:

			///T Node
			/// The node to point to
			NodePtr node;

			PtrNode(const NodePtr& node);
			~PtrNode();

			virtual Symbols::ScopeList Type() const override;
			virtual Boxx::List<Symbols::ScopeList> Types() const override;
			virtual Symbols::Symbols GetSymbol() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}