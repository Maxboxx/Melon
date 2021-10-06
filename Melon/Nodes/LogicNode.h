#pragma once

#include "BinaryOperatorNode.h"

#include "Melon/Token.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for logic gates.
		class LogicNode : public BinaryOperatorNode {
		public:
			/// The type of logic gate.
			TokenType type;

			LogicNode(Symbols::Symbol* const scope, const TokenType type, const FileInfo& file);
			~LogicNode();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual Symbols::Name GetOperator() const;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Expression Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign);

			static CompiledNode CompileToBool(const Expression& node, CompileInfo& info);

			CompiledNode CompileAndOr(CompileInfo& info, const bool checkTrue, const bool setTrue) const;
			CompiledNode CompileAndOrOperand(CompileInfo& info, CompiledNode& cn, Boxx::List<Boxx::UInt>& jumps, const bool checkTrue) const;
			CompiledNode CompileXor(CompileInfo& info, const bool checkEqual) const;
		};
	}
}