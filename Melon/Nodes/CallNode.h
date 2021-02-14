#pragma once

#include "Node.h"

#include "Melon/Symbols/FunctionSymbol.h"

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

			///T Call Operator
			bool op = false;

			///T Statement
			bool isStatement = false;

			CallNode(Symbols::Symbol* const scope, const FileInfo& file);
			~CallNode();

			///H Methods

			///T Get function
			/// Finds the function to call
			Symbols::FunctionSymbol* GetFunc() const;

			///T Is function
			/// Checks if the node to call is a function
			bool IsSelfPassing() const;

			///T Is Init
			bool IsInit() const;

			virtual Symbols::TypeSymbol* Type() const override;
			virtual Boxx::List<Symbols::TypeSymbol*> Types() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}