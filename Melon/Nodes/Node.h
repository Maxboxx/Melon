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

#include "Melon/Optimizing/OptimizerInstruction.h"

///N Melon::Nodes
namespace Melon {
	namespace Parsing {
		struct ParsingInfo;
	}

	namespace Nodes {
		class RootNode;

		/// Base for all nodes.
		class Node {
		public:
			/// Creates a node.
			Node(Symbols::Symbol* const scope, const FileInfo& file);
			~Node();

			/// Scans the node for potential extra includes.
			virtual void IncludeScan(Parsing::ParsingInfo& info);

			/// Scans the node for potential errors.
			virtual ScanResult Scan(ScanInfoStack& info);

			/// Optimizes the node.
			///R The optimized node. If the node was not optimized, this will be {nullptr}.
			virtual Node* Optimize(OptimizeInfo& info);

			/// Compiles the node.
			virtual CompiledNode Compile(CompileInfo& info) = 0;

			/// Gets the byte size of the node.
			///p This is only used by the compile step.
			virtual Boxx::UInt GetSize() const;

			/// Checks if the node has side effects
			bool HasSideEffects();

			/// Checks if the node has side effects outside of the specified scope.
			bool HasSideEffects(const Symbols::NameList& scope);

			/// Gets the side effect scope.
			Symbols::NameList GetSideEffectScope(const bool assign);

			/// Converts the node to melon.
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const = 0;

			/// Checks if the node is a specific type of node.
			///M
			template <class T>
			bool Is() {
			///M
				return dynamic_cast<T*>(this) != nullptr;
			}

			/// Casts the node to a specific node type.
			///p Returns {nullptr} if the cast fails.
			///M
			template <class T>
			T* Cast() {
			///M
				return dynamic_cast<T*>(this);
			}

		protected:
			Symbols::Symbol* scope;
			FileInfo file;

			/// Combines two side effect scopes by returning the outer scope.
			static Symbols::NameList CombineSideEffects(const Symbols::NameList& scope1, const Symbols::NameList& scope2);

			/// Scans an assignment.
			static ScanResult ScanAssignment(Node* const var, Node* const value, ScanInfoStack& info, const FileInfo& file);

			/// Compiles an assignment.
			static CompiledNode CompileAssignment(Node* const var, Node* const value, CompileInfo& info, const FileInfo& file);

			/// Checks if a node is an empty empty node.
			static bool IsEmpty(Node* const node);

			virtual Symbols::NameList FindSideEffectScope(const bool assign);

		private:
			Boxx::Optional<Symbols::NameList> sideEffectScope;
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