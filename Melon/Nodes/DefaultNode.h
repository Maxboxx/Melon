#pragma once

#include "Node.h"
#include "BinaryOperatorNode.h"

namespace Melon {
	namespace Nodes {

		class IfExprNode;

		///B DefaultNode
		class DefaultNode : public BinaryOperatorNode {
		public:
			DefaultNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~DefaultNode();

			virtual Symbols::ScopeList Type() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) override;
			virtual Boxx::Mango ToMango() const override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}