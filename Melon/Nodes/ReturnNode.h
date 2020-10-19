#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B ReturnNode
		/// Node for function returns
		class ReturnNode : public Node {
		public:

			///T Nodes
			/// The nodes to return from the function
			Boxx::List<NodePtr> nodes;

			///T Function
			/// The absolute name of the function to return from
			Symbols::ScopeList func;

			ReturnNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~ReturnNode();

			///T Get Types
			/// Get the symbols of the return values
			Boxx::List<Symbols::Symbol> GetTypes() const;

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize() override;
			virtual Boxx::Mango ToMango() const override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}