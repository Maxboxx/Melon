#pragma once

#include "Melon/Nodes/Node.h"

#include "Kiwi/Kiwi.h"

namespace Melon {
	namespace Symbols {
		namespace Nodes {

			///B SymbolNode
			/// Base for symbol nodes
			class SymbolNode {
				public:

				///T Compile
				/// Compiles the symbol node
				virtual Melon::Nodes::CompiledNode Compile(const Boxx::List<Melon::Nodes::NodePtr>& nodes, Melon::Nodes::CompileInfo& info) const = 0;

				///T Offset Argument
				/// Offsets a kiwi argument
				static Kiwi::Argument OffsetArgument(const Kiwi::Argument& arg, const Boxx::UInt frame, Melon::Nodes::CompileInfo& info) {
					if (arg.type != Kiwi::ArgumentType::Memory) return arg;
					if (arg.mem.memptr.IsRight() || arg.mem.memptr.GetLeft().type != Kiwi::RegisterType::Stack) return arg;

					Kiwi::Argument a = arg;
					a.mem.offset += info.stack.frame - frame;
					return a;
				}
			};
		}
	}
}