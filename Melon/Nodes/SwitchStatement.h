#pragma once

#include "Statement.h"
#include "SwitchBaseNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for {switch} statements.
		class SwitchStatement : public SwitchBaseNode<Statement, Statements> {
		public:
			SwitchStatement(Symbols::Symbol* const scope, const FileInfo& file);
			~SwitchStatement();

			virtual bool IsScope() const override;
			virtual Ptr<Statement> Optimize(OptimizeInfo& info) override;
		};
	}
}
