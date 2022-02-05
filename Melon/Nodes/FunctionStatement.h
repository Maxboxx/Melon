#pragma once

#include "Statement.h"

#include "Melon/Symbols/FunctionSymbol.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for functions.
		class FunctionStatement : public Statement {
		public:
			/// The function body.
			Ptr<Statements> statements;

			/// The function symbol.
			Symbols::FunctionSymbol* sym;

			/// {true} if the function is used.
			bool isUsed = true;

			FunctionStatement(Symbols::Symbol* const scope, const FileInfo& file);
			~FunctionStatement();

			virtual bool IsScope() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Ptr<Statement> Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}