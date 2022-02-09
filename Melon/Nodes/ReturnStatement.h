#pragma once

#include "Statement.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for {return} statements.
		class ReturnStatement : public Statement {
		public:
			/// The values to return from the function.
			Boxx::List<Ptr<Expression>> values;

			/// The absolute name of the function to return from.
			Symbols::NameList func;

			ReturnStatement(Symbols::Symbol* const scope, const FileInfo& file);
			~ReturnStatement();

			/// Get the function to return from.
			Symbols::FunctionSymbol* GetFunc() const;

			/// Get the symbols of the return value types.
			Boxx::List<Symbols::TypeSymbol*> GetTypes() const;

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Ptr<Statement> Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign);
		};
	}
}