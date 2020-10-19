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
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize() override;
			virtual Boxx::Mango ToMango() const override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}