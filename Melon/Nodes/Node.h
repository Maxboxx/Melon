#pragma once

#include "Boxx/Pointer.h"
#include "Boxx/List.h"
#include "Boxx/Mango.h"
#include "Boxx/Set.h"
#include "Boxx/Stack.h"
#include "Boxx/Tuple.h"
#include "Boxx/Math.h"
#include "Boxx/Error.h"
#include "Boxx/String.h"
#include "Boxx/StringBuilder.h";

#include "Kiwi/Kiwi.h"

#include "Structs/CompiledNode.h"
#include "Structs/CompileInfo.h"
#include "Structs/OptimizeInfo.h"
#include "Structs/ScanInfo.h"
#include "Structs/ScanInfoStack.h"
#include "Structs/ScanResult.h"
#include "Structs/ScopeInfo.h"
#include "Structs/StackPtr.h"

#include "Melon/Symbols/ScopeList.h"
#include "Melon/Symbols/SymbolTable.h"
#include "Melon/Symbols/TypeSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"

#include "Melon/Optimizing/OptimizerInstruction.h"

namespace Melon {
	namespace Parsing {
		struct ParsingInfo;
	}

	///N Melon::Nodes
	namespace Nodes {
		class RootNode;

		///T NodePtr
		/// Typedef for a pointer to a {Node}.
		typedef Boxx::Pointer<class Node> NodePtr;

		/// Base for all nodes.
		class Node {
		public:
			/// The scope the node is in.
			Symbols::Symbol* scope;

			/// The file info of the node.
			FileInfo file;

			/// The inner side effect scope.
			Boxx::Optional<Symbols::ScopeList> sideEffectScope;

			/// Creates a node.
			Node(Symbols::Symbol* const scope, const FileInfo& file);
			~Node();

			/// Returns the type of the node.
			virtual Symbols::TypeSymbol* Type() const;

			/// Returns the types of all return values of the node.
			///p Defaults to a list containing the result from {Type()}.
			virtual Boxx::List<Symbols::TypeSymbol*> Types() const;

			/// Returns the symbol for the current node.
			virtual Symbols::Symbol* GetSymbol() const;

			/// Gets the byte size of the node.
			///p This is only used by the compile step.
			virtual Boxx::UInt GetSize() const;

			/// {true} if the node is a scope.
			virtual bool IsScope() const;

			/// Compiles the node.
			virtual CompiledNode Compile(CompileInfo& info) = 0;

			/// Converts the node to melon.
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const = 0;

			/// Scans the node for potential extra includes.
			virtual void IncludeScan(Parsing::ParsingInfo& info);

			/// Scans the node for potential errors.
			virtual ScanResult Scan(ScanInfoStack& info);

			/// Checks if the node has side effects
			bool HasSideEffects();

			/// Checks if the node has side effects outside of the specified scope.
			bool HasSideEffects(const Symbols::ScopeList& scope);

			/// Gets the side effect scope.
			Symbols::ScopeList GetSideEffectScope(const bool assign);

			/// Optimizes the node.
			///R The optimized node. If the node was not optimized, this will be {nullptr}.
			virtual NodePtr Optimize(OptimizeInfo& info);

			/// {true} if the compiled node is an immediate value.
			///p Defaults to {false}.
			virtual bool IsImmediate() const;

			/// Gets the immediate value of the node.
			virtual Boxx::Long GetImmediate() const;
			
			/// Scans an assignment.
			static ScanResult ScanAssignment(NodePtr var, NodePtr value, ScanInfoStack& info, const FileInfo& file);

			/// Compiles an assignment.
			static CompiledNode CompileAssignment(NodePtr var, NodePtr value, CompileInfo& info, const FileInfo& file);

			/// Checks if a node is an empty empty node.
			static bool IsEmpty(const NodePtr& node);

			/// Combines two side effect scopes by returning the outer scope.
			static Symbols::ScopeList CombineSideEffects(const Symbols::ScopeList& scope1, const Symbols::ScopeList& scope2);

			/// Pointer to the root node.
			static RootNode* root;

		protected:
			virtual Symbols::ScopeList FindSideEffectScope(const bool assign);
		};

		///H Errors

		/// Used in include scan if more info is needed for the scan.
		class IncludeScanError : public Boxx::Error {
		public:
			IncludeScanError() : Error() {}
			IncludeScanError(const char* const msg) : Error(msg) {}

			virtual Boxx::String Name() const override {
				return "IncludeScanError";
			}
		};
	}
}