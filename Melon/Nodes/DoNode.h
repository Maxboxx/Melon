#pragma once

#include "Node.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// A node for {do} statements.
		class DoNode : public Node {
		public:
			/// The content of the do statement.
			NodePtr nodes;

			DoNode(Symbols::Symbol* const scope, const FileInfo& file);
			~DoNode();

			virtual Boxx::UInt GetSize() const override;
			virtual bool IsScope() const override;
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