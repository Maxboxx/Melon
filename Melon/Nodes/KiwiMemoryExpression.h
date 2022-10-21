#pragma once

#include "Expression.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Expression for a memory location.
		class KiwiMemoryExpression : public Expression {
		public:
			/// The memory location.
			KiwiOld::MemoryLocation mem;

			/// The absolute type name of the node.
			Symbols::NameList type;

			KiwiMemoryExpression(const Boxx::Int offset, const Symbols::NameList& type);
			KiwiMemoryExpression(const KiwiOld::MemoryLocation& mem, const Symbols::NameList& type);
			~KiwiMemoryExpression();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}