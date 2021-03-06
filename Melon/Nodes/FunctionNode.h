#pragma once

#include "Node.h"

#include "Melon/Symbols/FunctionSymbol.h"

namespace Melon {
	namespace Nodes {

		///B FunctionNode
		/// Node for functions
		class FunctionNode : public Node {
		public:

			///T Node
			/// The function body
			NodePtr node;

			///T Func
			Symbols::FunctionSymbol* sym;

			///T Is used
			bool isUsed = true;

			FunctionNode(Symbols::Symbol* const scope, const FileInfo& file);
			~FunctionNode();

			virtual bool IsScope() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}