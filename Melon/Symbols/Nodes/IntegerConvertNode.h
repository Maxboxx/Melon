#pragma once

#include "IntegerBaseNode.h"

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Node for converting an integer to an integer.
			class IntegerConvertNode : public IntegerBaseNode<UnarySymbolNode> {
			public:
				/// {true} if the result should be signed.
				bool sign;

				/// The target integer size.
				Boxx::UByte targetSize;

				virtual Melon::Nodes::CompiledNode Compile(const Melon::Nodes::Expression& operand, Melon::Nodes::CompileInfo& info) const override;
			};
		}
	}
}