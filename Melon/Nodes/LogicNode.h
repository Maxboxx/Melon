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

			static CompiledNode CompileToBool(const NodePtr& node, CompileInfo& info);

			CompiledNode CompileAndOr(CompileInfo& info, const bool checkTrue, const bool setTrue) const;
			CompiledNode CompileXor(CompileInfo& info, const bool checkEqual) const;

			virtual Symbols::ScopeList Type() const override;
			virtual Symbols::Scope GetOperator() const;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize() override;
			virtual Boxx::Mango ToMango() const override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}