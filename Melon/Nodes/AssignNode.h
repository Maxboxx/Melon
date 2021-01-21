#pragma once

#include "Node.h"

#include "NewVariableNode.h"

namespace Melon {
	namespace Nodes {

		///B AssignNode
		/// Node for assignment
		class AssignNode : public Node {
		public:
			///T Variables and values
			/// List of variables and values for each variable
			Boxx::List<NodePtr> vars, values;

			///T Types
			/// List of all types
			Boxx::List<Symbols::ScopeList> types;

			AssignNode(Symbols::Symbol* const scope, const FileInfo& file);
			~AssignNode();

			Boxx::List<Boxx::Pair<Symbols::TypeSymbol*, NodePtr>> Values() const;

			virtual Boxx::UInt GetSize() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual void Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::ScopeList FindSideEffectScope(const bool assign) override;
		};
	}
}