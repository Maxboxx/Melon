#pragma once

#include "Statement.h"

#include "Melon/Symbols/FunctionSymbol.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for functions.
		class FunctionBody : public Statement {
		public:
			/// The function body.
			Ptr<Statements> statements;

			/// The function symbol.
			Symbols::FunctionSymbol* sym;

			/// {true} if the function is used.
			bool isUsed = true;

			FunctionBody(Symbols::Symbol* const scope, const FileInfo& file);
			~FunctionBody();

			virtual bool IsScope() const override;
			virtual CompiledNode Compile(OldCompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Ptr<Statement> Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}