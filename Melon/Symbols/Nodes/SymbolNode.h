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
				virtual Ptr<Kiwi::Value> Compile(Weak<Melon::Nodes::Expression> operand, Melon::Nodes::CompileInfo& info, bool includeType) const {
					throw SymbolNodeError("SymbolNode does not override Compile");
				}

				/// Compiles the symbol node.
				///A operand1: The first operand.
				///A operand2: The second operand.
				virtual Ptr<Kiwi::Value> Compile(Weak<Melon::Nodes::Expression> operand1, Weak<Melon::Nodes::Expression> operand2, Melon::Nodes::CompileInfo& info, bool includeType) const {
					throw SymbolNodeError("SymbolNode does not override Compile");
				}

				/// Compiles the symbol node.
				///A operands: The operands.
				virtual Ptr<Kiwi::Value> Compile(Boxx::List<Weak<Melon::Nodes::Expression>> operands, Melon::Nodes::CompileInfo& info, bool includeType) const {
					throw SymbolNodeError("SymbolNode does not override Compile");
				}
			};

			/// Base for unary symbol nodes.
			class UnarySymbolNode : public SymbolNode {
			public:
				virtual Ptr<Kiwi::Value> Compile(Weak<Melon::Nodes::Expression> operand1, Weak<Melon::Nodes::Expression> operand2, Melon::Nodes::CompileInfo& info, bool includeType) const override final {
					throw SymbolNodeError("UnarySymbolNode does not support binary operations");
				}
			};

			/// Base for binary symbol nodes.
			class BinarySymbolNode : public SymbolNode  {
			public:
				virtual Ptr<Kiwi::Value> Compile(Weak<Melon::Nodes::Expression> operand, Melon::Nodes::CompileInfo& info, bool includeType) const override final {
					throw SymbolNodeError("BinarySymbolNode does not support unary operations");
				}
			};

			/// Base for constant symbol nodes.
			class ConstantSymbolNode : public SymbolNode {
			public:
				virtual Ptr<Kiwi::Value> Compile(Weak<Melon::Nodes::Expression> operand, Melon::Nodes::CompileInfo& info, bool includeType) const override final {
					return Compile(info);
				}

				virtual Ptr<Kiwi::Value> Compile(Weak<Melon::Nodes::Expression> operand1, Weak<Melon::Nodes::Expression> operand2, Melon::Nodes::CompileInfo& info, bool includeType) const override final {
					return Compile(info);
				}

				/// Compiles the symbol node.
				virtual Ptr<Kiwi::Value> Compile(Melon::Nodes::CompileInfo& info) const {
					throw SymbolNodeError("ConstantSymbolNode does not override Compile");
				};
			};
		}
	}
}