#pragma once

#include "Node.h"

#include "Melon/Symbols/VariableSymbol.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for creating new variables.
		class [[deprecated]] NewVariableNode : public Node {
		public:
			/// List of relative types for the new variables.
			///p If the list only contains one type all variables will be of that type.
			Boxx::List<Symbols::NameList> types;

			/// All variable names to create.
			Boxx::List<Symbols::Name> names;

			/// All attributes for each variable.
			Boxx::List<Symbols::VariableAttributes> attributes;

			NewVariableNode(Symbols::Symbol* const scope, const FileInfo& file);
			~NewVariableNode();
			
			/// Gets the absolute name of the variables.
			Boxx::List<Symbols::NameList> GetVariables() const;

			/// Get the type of the specified type index.
			Symbols::TypeSymbol* GetType(const Boxx::UInt index) const;

			virtual Symbols::TypeSymbol* Type() const;
			virtual Boxx::UInt GetSize() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}