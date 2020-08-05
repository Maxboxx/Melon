#pragma once

#include "BinaryOperatorNode.h"

#include "Melon/Token.h"

namespace Melon {
	namespace Nodes {

		///B LogicNode
		/// Node for logic gates
		struct LogicNode : public BinaryOperatorNode {
			///T The logic gate type
			TokenType type;

			///T Constructor
			LogicNode(const Symbols::ScopeList& scope, const TokenType type, const FileInfo& file);
			~LogicNode();

			virtual Symbols::ScopeList Type() const override;
			virtual Symbols::Scope GetOperator() const;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfo& info) const override;
			virtual Boxx::Mango ToMango() const override;
		};
	}
}