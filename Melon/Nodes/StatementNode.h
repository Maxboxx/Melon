#pragma once

#include "Node.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// The base of all expression nodes.
		class StatementNode : public Node {
		public:
			StatementNode(Symbols::Symbol* const scope, const FileInfo& file);
			~StatementNode();

			/// {true} if the statement is a scope.
			virtual bool IsScope() const;

		protected:
			virtual StatementNode* Optimize(OptimizeInfo& info) override = 0;
		};
	}
}
