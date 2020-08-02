#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B AssignNode
		/// Node for assignment
		class AssignNode : public Node {
		public:

			///T New variables
			/// List of new variables if there are any
			Boxx::Pointer<struct NewVariableNode> newVars;

			///T Variables and values
			/// List of variables and values for each variable
			Boxx::List<NodePtr> vars, values;

			AssignNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~AssignNode();

			static bool IsAssignable(const NodePtr& node);

			Boxx::List<Boxx::Pair<Symbols::ScopeList, NodePtr>> Values() const;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfo& info) const override;
			virtual Boxx::Mango ToMango() const override;
		};
	}
}