#pragma once

#include "BinaryOperatorExpression.h"

#include "Melon/Token.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for logic gates.
		class LogicExpression : public BinaryOperatorExpression {
		public:
			/// The type of logic gate.
			TokenType type;

			LogicExpression(Symbols::Symbol* const scope, const TokenType type, const FileInfo& file);
			~LogicExpression();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual Symbols::Name GetOperator() const;
			virtual CompiledNode Compile(OldCompileInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Ptr<Expression> Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign);

			static CompiledNode CompileToBool(Ptr<Expression>& node, OldCompileInfo& info);

			CompiledNode CompileAndOr(OldCompileInfo& info, const bool checkTrue, const bool setTrue);
			CompiledNode CompileAndOrOperand(OldCompileInfo& info, CompiledNode& cn, Boxx::List<Boxx::UInt>& jumps, const bool checkTrue);
			CompiledNode CompileXor(OldCompileInfo& info, const bool checkEqual);
		};
	}
}