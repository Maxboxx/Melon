#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B BreakNode
		/// Node for breaking or aborting loops
		class BreakNode : public Node {
		public:
	
			///T Is break
			/// <key>true</key> if break
			/// <key>false</key> if abort
			bool isBreak;

			///T Loops
			/// The amount of loops to break
			Boxx::UInt loops;

			///T Break bool
			/// The boolean argument to break
			bool breakBool;

			bool scopeWise = false;

			BreakNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~BreakNode();

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) const override;
			virtual Boxx::Mango ToMango() const override;
		};
	}
}