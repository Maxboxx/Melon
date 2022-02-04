#pragma once

#include "ExpressionNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for an integer.
		class IntegerNode : public Expression {
		public:
			/// The integer value.
			Boxx::Long number;

			/// {true} if the integer is unsigned.
			bool isUnsigned = true;

			IntegerNode(const FileInfo& file);
			~IntegerNode();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual bool IsImmediate() const override;
			virtual Boxx::Long GetImmediate() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}