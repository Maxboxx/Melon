#pragma once

#include "Node.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// The base of all expression nodes.
		class Statement : public Node {
		public:
			Statement(Symbols::Symbol* const scope, const FileInfo& file);
			~Statement();

			/// {true} if the statement is a scope.
			virtual bool IsScope() const;

			/// Optimizes the statement.
			///R A new optimized node or {nullptr} if the node was not optimized.
			virtual Ptr<Statement> Optimize(OptimizeInfo& info);
		};
	}
}
