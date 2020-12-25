#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B RefNode
		/// Node for references
		class RefNode : public Node {
		public:

			///T Node
			/// The node to reference
			NodePtr node;

			RefNode(const NodePtr& node);
			~RefNode();

			virtual Symbols::Symbol* Type() const override;
			virtual Boxx::List<Symbols::Symbol*> Types() const override;
			virtual Symbols::Symbol* GetSymbol() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}