#pragma once

#include "Statement.h"
#include "MatchBaseNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for {switch} statements.
		class MatchStatement : public MatchBaseNode<Statement, Statements> {
		public:
			MatchStatement(Symbols::Symbol* const scope, const FileInfo& file);
			~MatchStatement();

			virtual bool IsScope() const override;
			virtual Ptr<Statement> Optimize(OptimizeInfo& info) override;
		};
	}
}
