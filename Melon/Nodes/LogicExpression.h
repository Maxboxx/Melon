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

			virtual Symbols::TypeSymbol* Type(Symbols::TypeSymbol* expected = nullptr) const override;
			virtual Symbols::Name GetOperator() const;
			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Ptr<Expression> Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign);

			static Ptr<Kiwi::Value> CompileToBool(Weak<Expression> node, CompileInfo& info);

			Ptr<Kiwi::Value> CompileAndOr(CompileInfo& info, const bool checkTrue, const bool setTrue);
			Ptr<Kiwi::Value> CompileXor(CompileInfo& info, const bool checkEqual);
		};
	}
}