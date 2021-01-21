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

			///T Symbol
			/// The function symbol
			Symbols::FunctionSymbol* sym;

			///T Argument names
			Boxx::List<Symbols::Scope> argNames;

			///T Is used
			bool isUsed = true;

			FunctionNode(Symbols::Symbol* const scope, const FileInfo& file);
			~FunctionNode();

			///T Is Not Specialized
			/// Checks if the function is a template function and has not been specialized
			bool IsNotSpecialized() const;

			///T Specialize Templates
			/// Specialize the values of templates
			void SetTemplateValues() const;

			virtual bool IsScope() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual void Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}