#pragma once

#include "Statement.h"
#include "BaseCallNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Call statement.
		class CallStatement : public BaseCallNode<Statement> {
		public:
			CallStatement(Symbols::Symbol* const scope, const FileInfo& file);
			~CallStatement();
		};
	}
}
