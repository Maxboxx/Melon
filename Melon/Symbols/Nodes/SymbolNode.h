#pragma once

#include "Melon/Nodes/Expression.h"

#include "Boxx/Error.h"
#include "Boxx/String.h"

#include "Kiwi/Old/Kiwi.h"

///N Melon::Symbols::Nodes
namespace Melon {
	namespace Symbols {
		namespace Nodes {
			/// Error for symbol nodes.
			class SymbolNodeError : public Boxx::Error {
			public:
				SymbolNodeError() : Boxx::Error() {}
				SymbolNodeError(const char* const msg) : Boxx::Error(msg) {}

				virtual Boxx::String Name() const override {
					return "SymbolNodeError";
				}
			};

			/// Base for symbol nodes.
			class SymbolNode {
			public:
				/// Compiles the symbol node.
				///A operand: The operand.
				virtual Melon::Nodes::CompiledNode Compile(Weak<Melon::Nodes::Expression> operand, Melon::Nodes::OldCompileInfo& info) const = 0;

				/// Compiles the symbol node.
				///A operand1: The first operand.
				///A operand2: The second operand.
				virtual Melon::Nodes::CompiledNode Compile(Weak<Melon::Nodes::Expression> operand1, Weak<Melon::Nodes::Expression> operand2, Melon::Nodes::OldCompileInfo& info) const = 0;

				/// Offsets a kiwi argument.
				static KiwiOld::Argument OffsetArgument(const KiwiOld::Argument& arg, const Boxx::UInt frame, Melon::Nodes::OldCompileInfo& info) {
					if (arg.type != KiwiOld::ArgumentType::Memory) return arg;
					if (arg.mem.memptr.IsRight() || arg.mem.memptr.GetLeft().type != KiwiOld::RegisterType::Stack) return arg;

					KiwiOld::Argument a = arg;
					a.mem.offset += info.stack.frame - frame;
					return a;
				}
			};

			/// Base for unary symbol nodes.
			class UnarySymbolNode : public SymbolNode {
			public:
				virtual Melon::Nodes::CompiledNode Compile(Weak<Melon::Nodes::Expression> operand1, Weak<Melon::Nodes::Expression> operand2, Melon::Nodes::OldCompileInfo& info) const override final {
					throw SymbolNodeError("UnarySymbolNode does not support binary operations");
				}
			};

			/// Base for binary symbol nodes.
			class BinarySymbolNode : public SymbolNode  {
			public:
				virtual Melon::Nodes::CompiledNode Compile(Weak<Melon::Nodes::Expression> operand, Melon::Nodes::OldCompileInfo& info) const override final {
					throw SymbolNodeError("BinarySymbolNode does not support unary operations");
				}
			};

			/// Base for constant symbol nodes.
			class ConstantSymbolNode : public SymbolNode {
			public:
				virtual Melon::Nodes::CompiledNode Compile(Weak<Melon::Nodes::Expression> operand, Melon::Nodes::OldCompileInfo& info) const override final {
					return Compile(info);
				}

				virtual Melon::Nodes::CompiledNode Compile(Weak<Melon::Nodes::Expression> operand1, Weak<Melon::Nodes::Expression> operand2, Melon::Nodes::OldCompileInfo& info) const override final {
					return Compile(info);
				}

				/// Compiles the symbol node.
				virtual Melon::Nodes::CompiledNode Compile(Melon::Nodes::OldCompileInfo& info) const = 0;
			};
		}
	}
}