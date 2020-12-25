#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B IntegerNode
		/// Node for an integer
		class IntegerNode : public Node {
		public:

			///T Number
			Boxx::Long number;

			///T is unsigned
			bool isUnsigned = true;

			///T Constructor
			IntegerNode(const FileInfo& file);
			~IntegerNode();

			virtual Symbols::Symbol* Type() const override;
			virtual bool IsImmediate() const override;
			virtual Boxx::Long GetImmediate() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}