#pragma once

#include "StatementNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for {return} statements.
		class ReturnNode : public StatementNode {
		public:
			/// The values to return from the function.
			Boxx::List<Expression> values;

			/// The absolute name of the function to return from.
			Symbols::NameList func;

			ReturnNode(Symbols::Symbol* const scope, const FileInfo& file);
			~ReturnNode();

			/// Get the function to return from.
			Symbols::FunctionSymbol* GetFunc() const;

			/// Get the symbols of the return value types.
			Boxx::List<Symbols::TypeSymbol*> GetTypes() const;

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Statement Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign);
		};
	}
}