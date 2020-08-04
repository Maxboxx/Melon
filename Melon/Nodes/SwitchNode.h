#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {
		///B SwitchNode
		/// Node for both switch-statements and switch-expressions
		class SwitchNode : public Node {
		public:

			///T Expression
			/// <key>true</key> if the switch is an expression
			bool expr;

			///T Match expression
			NodePtr match;

			///T Nodes
			/// The nodes for all the cases
			Boxx::List<NodePtr> nodes;

			///T Cases
			/// All case expressions
			Boxx::List<Boxx::List<NodePtr>> cases;

			///T Default
			NodePtr def;

			SwitchNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~SwitchNode();

			virtual Symbols::ScopeList Type() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfo& info) const override;
			virtual Boxx::Mango ToMango() const override;
		};
	}
}