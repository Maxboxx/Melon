#pragma once

#include "Expression.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Expression for a memory location.
		class KiwiMemoryExpression : public Expression {
		public:
			/// The memory location.
			Kiwi::MemoryLocation mem;

			/// The absolute type name of the node.
			Symbols::NameList type;

			KiwiMemoryExpression(const Boxx::Int offset);
			KiwiMemoryExpression(const Kiwi::MemoryLocation& mem);
			~KiwiMemoryExpression();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}