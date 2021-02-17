#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {
		///B ConvertNode
		/// Node for converting a value to a different type
		class ConvertNode : public Node {
		public:
			///T Node
			/// The node to convert the type of
			NodePtr node;

			///T Type
			/// The type to convert to
			Symbols::ScopeList type;

			///T Is Explicit
			/// Is the conversion explicit or not
			bool isExplicit = false;

			ConvertNode(Symbols::Symbol* const scope, const FileInfo& file);
			~ConvertNode();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::ScopeList FindSideEffectScope(const bool assign);
		};
	}
}