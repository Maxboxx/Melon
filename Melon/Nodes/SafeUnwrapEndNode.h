#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B SafeUnwrapEndNode
		/// End node for the safe unwrap opertator
		class SafeUnwrapEndNode : public Node {
		public:

			///T Node
			/// The unwrapped node
			NodePtr node;

			SafeUnwrapEndNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~SafeUnwrapEndNode();

			virtual Symbols::ScopeList Type() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize() override;
			virtual Boxx::Mango ToMango() const override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

			static Boxx::String jumpInstName;
		};
	}
}