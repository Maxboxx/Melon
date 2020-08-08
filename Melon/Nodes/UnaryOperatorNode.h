#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B UnaryOperatorNode
		/// Node for unary operators
		class UnaryOperatorNode : public Node {
		public:

			///H Members

			///T The node to perform the oparation on
			NodePtr node;

			///T The operator
			Symbols::Scope op;

			///H Constructors

			///T Constructor
			UnaryOperatorNode(const Symbols::ScopeList& scope, const Symbols::Scope& op, const FileInfo& file);
			~UnaryOperatorNode();

			///H Methods

			virtual Symbols::ScopeList Type() const override;

			///T Get operator
			virtual Symbols::Scope GetOperator() const;

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) const override;
			virtual Boxx::Mango ToMango() const override;
		};
	}
}