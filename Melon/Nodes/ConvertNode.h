#pragma once

#include "Node.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for converting a value to a different type.
		class ConvertNode : public Node {
		public:
			/// The node to convert to a different type.
			NodePtr node;

			/// The absolute name of the type to convert to.
			Symbols::NameList type;

			/// Is the conversion explicit or not.
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
			virtual Symbols::NameList FindSideEffectScope(const bool assign);
		};
	}
}