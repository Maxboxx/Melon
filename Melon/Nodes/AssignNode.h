#pragma once

#include "Node.h"

#include "NewVariableNode.h"

namespace Melon {
	namespace Nodes {

		///B AssignNode
		/// Node for assignment
		class AssignNode : public Node {
		public:

			///T New variables
			/// List of new variables if there are any
			Boxx::Pointer<NewVariableNode> newVars;

			///T Variables and values
			/// List of variables and values for each variable
			Boxx::List<NodePtr> vars, values;

			AssignNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~AssignNode();

			Boxx::List<Boxx::Pair<Symbols::ScopeList, NodePtr>> Values() const;

			virtual Boxx::UInt GetSize() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize() override;
			virtual Boxx::Mango ToMango() const override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}