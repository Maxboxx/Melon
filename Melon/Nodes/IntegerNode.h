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

			///T Constructor
			IntegerNode(const FileInfo& file);
			~IntegerNode();

			virtual Symbols::ScopeList Type() const override;
			virtual bool IsImmediate() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::Mango ToMango() const override;
		};
	}
}