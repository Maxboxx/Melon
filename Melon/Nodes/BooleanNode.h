#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B BooleanNode
		/// Node for a boolean value
		class BooleanNode : public Node {
		public:

			///T Boolean
			bool boolean = false;

			///T Constructor
			BooleanNode(const FileInfo& file);
			~BooleanNode();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual bool IsImmediate() const override;
			virtual Boxx::Long GetImmediate() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}