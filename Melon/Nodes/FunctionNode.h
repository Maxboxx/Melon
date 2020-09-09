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

			///T Is Not Specialized
			/// Checks if the function is a template function and has not been specialized
			bool IsNotSpecialized() const;

			///T Specialize Templates
			/// Specialize the values of templates
			void SetTemplateValues() const;

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) override;
			virtual Boxx::Mango ToMango() const override;
		};
	}
}