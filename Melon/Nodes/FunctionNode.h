#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B FunctionNode
		/// Node for functions
		class FunctionNode : public Node {
		public:

			///T Node
			/// The function body
			NodePtr node;

			///T Function
			/// Absolute name of the function
			Symbols::ScopeList func;

			///T Symbol
			/// The function symbol
			Symbols::Symbol s;

			///T Argument names
			Boxx::List<Symbols::Scope> argNames;

			FunctionNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~FunctionNode();

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) const override;
			virtual Boxx::Mango ToMango() const override;
		};
	}
}