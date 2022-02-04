#pragma once

#include "ExpressionNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for object init.
		class ObjectInitNode : public Expression {
		public:
			/// The expression to perform object init on.
			_Expression_ expression;

			/// The variables to assign values to.
			Boxx::List<Symbols::Name> vars;

			/// The expressions to assign to the variables.
			Boxx::List<_Expression_> expressions;

			ObjectInitNode(Symbols::Symbol* const scope, const FileInfo& file);
			~ObjectInitNode();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual _Expression_ Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign);
		};
	}
}