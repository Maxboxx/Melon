#pragma once

#include "Node.h"

#include "Melon/Symbols/VariableSymbol.h"

namespace Melon {
	namespace Nodes {

		///B NewVariableNode
		/// Node for creating new variables
		class NewVariableNode : public Node {
		public:

			///T Types
			/// List of relative types for the new variables
			/// If the list only contains one type all variables will be of that type
			Boxx::List<Symbols::ScopeList> types;

			///T Names
			/// All variable names to create
			Boxx::List<Symbols::Scope> names;

			///T Attributes
			/// All attributes for each variable
			Boxx::List<Symbols::VariableAttributes> attributes;

			NewVariableNode(Symbols::Symbol* const scope, const FileInfo& file);
			~NewVariableNode();

			///T Get Variables
			/// Gets the absolute name of the variables
			Boxx::List<Symbols::ScopeList> GetVariables() const;

			///T Get the type of the specified type index
			Symbols::TypeSymbol* GetType(const Boxx::UInt index) const;

			virtual Symbols::TypeSymbol* Type() const override;
			virtual Boxx::UInt GetSize() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}