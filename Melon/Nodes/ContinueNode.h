#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B ContinueNode
		/// Node for continuing loops
		class ContinueNode : public Node {
		public:

			///T Loops
			/// The amount of loops to continue
			Boxx::UInt loops;

			ContinueNode(Symbols::Symbol* const scope, const FileInfo& file);
			~ContinueNode();

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

			static Boxx::String continueInstName;
		};
	}
}