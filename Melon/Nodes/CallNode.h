#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {
		///B CallNode
		/// Node for calling a value
		class CallNode : public Node {
		public:

			///H Members

			///T Node
			/// The node to call
			NodePtr node;

			///T Arguments
			Boxx::List<NodePtr> args;

			///T No Ref Args
			Boxx::List<bool> noRefs;

			///T Is Method
			bool isMethod = false;

			///T Method Name
			Symbols::Scope methodName;

			///T Operator
			bool op = false;

			///T Statement
			bool isStatement = false;

			CallNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~CallNode();

			///H Methods

			///T Get function
			/// Finds the function to call
			Symbols::Symbol GetFunc() const;

			///T Is function
			/// Checks if the node to call is a function
			bool IsSelfPassing() const;

			///T Is Init
			bool IsInit() const;

			virtual Symbols::ScopeList Type() const override;
			virtual Boxx::List<Symbols::ScopeList> Types() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize() override;
			virtual Boxx::Mango ToMango() const override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}