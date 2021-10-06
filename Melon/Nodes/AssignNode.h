#pragma once

#include "StatementNode.h"
#include "ExpressionNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for assignment.
		class AssignNode : public StatementNode {
		public:
			/// The assignable values to assign values to.
			Boxx::List<Expression> assignableValues;

			/// The values to assign.
			Boxx::List<Expression> values;

			/// The types of the assignable values.
			Boxx::List<Symbols::NameList> types;

			AssignNode(Symbols::Symbol* const scope, const FileInfo& file);
			~AssignNode();

			virtual Boxx::UInt GetSize() const override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

			virtual Statement Optimize(OptimizeInfo& info) override;

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign) override;

		private:
			struct Value {
				Symbols::TypeSymbol* type;
				Expression value;

				Value(TypeSymbol* const type, const Expression& value) {
					this->type  = type;
					this->value = value;
				}
			};

			ScanResult ScanAssignableValues(ScanInfoStack& info);

			Boxx::List<Value> Values() const;
		};
	}
}
