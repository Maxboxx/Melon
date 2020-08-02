#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B CustomInitNode
		/// Node for custom init on struct
		class CustomInitNode : public Node {
		public:

			///T Node
			NodePtr node;

			///T Variables
			/// The variables to assign values to
			Boxx::List<Symbols::Scope> vars;

			///T Expressions
			/// The expressions to assign to the variables
			Boxx::List<NodePtr> expressions;

			CustomInitNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~CustomInitNode();

			virtual Symbols::ScopeList Type() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfo& info) const override;
			virtual Boxx::Mango ToMango() const override;
		};
	}
}