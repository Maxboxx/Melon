#pragma once

#include "Node.h"

#include "NewVariableNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for assignment.
		class AssignNode : public Node {
		public:
			/// The assignable values to assign values to.
			Boxx::List<NodePtr> vars;

			/// The values to assign.
			Boxx::List<NodePtr> values;

			/// The types for new variables.
			Boxx::List<Symbols::NameList> types;

			AssignNode(Symbols::Symbol* const scope, const FileInfo& file);
			~AssignNode();

			/// Gets the types and nodes for all values.
			Boxx::List<Boxx::Pair<Symbols::TypeSymbol*, NodePtr>> Values() const;

			virtual Boxx::UInt GetSize() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign) override;
		};
	}
}