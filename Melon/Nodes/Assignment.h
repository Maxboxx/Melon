#pragma once

#include "Statement.h"
#include "Expression.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Assignment statement.
		class Assignment : public Statement {
		public:
			/// The assignable values to assign values to.
			Boxx::List<Ptr<Expression>> assignableValues;

			/// The values to assign.
			Boxx::List<Ptr<Expression>> values;

			/// The types of the assignable values.
			Boxx::List<Symbols::NameList> types;

			Assignment(Symbols::Symbol* const scope, const FileInfo& file);
			~Assignment();

			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

			virtual Ptr<Statement> Optimize(OptimizeInfo& info) override;
			void OptimizeAsCondition(OptimizeInfo& info);

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign) override;

		private:
			struct Value {
				Symbols::TypeSymbol* type;
				Weak<Expression> value;

				Value() {
					
				}

				Value(Symbols::TypeSymbol* const type, const Weak<Expression>& value) {
					this->type  = type;
					this->value = value;
				}
			};

			ScanResult ScanAssignableValues(ScanInfoStack& info);

			Boxx::List<Value> Values() const;
		};
	}
}
