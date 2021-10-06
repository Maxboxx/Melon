#pragma once

#include "StatementNode.h"

#include "Melon/Symbols/FunctionSymbol.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for functions.
		class FunctionNode : public StatementNode {
		public:
			/// The function body.
			Statements statements;

			/// The function symbol.
			Symbols::FunctionSymbol* sym;

			/// {true} if the function is used.
			bool isUsed = true;

			FunctionNode(Symbols::Symbol* const scope, const FileInfo& file);
			~FunctionNode();

			virtual bool IsScope() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Statement Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}