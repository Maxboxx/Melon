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
			LogicNode(Symbols::Symbol* const scope, const TokenType type, const FileInfo& file);
			~LogicNode();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual Symbols::Scope GetOperator() const;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::ScopeList FindSideEffectScope(const bool assign);

			static CompiledNode CompileToBool(const NodePtr& node, CompileInfo& info);

			CompiledNode CompileAndOr(CompileInfo& info, const bool checkTrue, const bool setTrue) const;
			CompiledNode CompileAndOrOperand(CompileInfo& info, CompiledNode& cn, Boxx::List<Boxx::UInt>& jumps, const bool checkTrue) const;
			CompiledNode CompileXor(CompileInfo& info, const bool checkEqual) const;
		};
	}
}