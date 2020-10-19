#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B BinaryOperatorNode
		/// Node for binary operators
		class BinaryOperatorNode : public Node {
		public:

			///H Members

			///T Operands
			NodePtr node1, node2;

			///T Operator
			Symbols::Scope op;

			///H Constructors

			///T Constructor
			BinaryOperatorNode(const Symbols::ScopeList& scope, const Symbols::Scope& op, const FileInfo& file);
			~BinaryOperatorNode();

			///H Methods

			virtual Symbols::ScopeList Type() const override;

			///T Get operator
			virtual Symbols::Scope GetOperator() const;

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize() override;
			virtual Boxx::Mango ToMango() const override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}