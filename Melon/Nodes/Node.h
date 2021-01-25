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
#include "Melon/Symbols/Symbols.h"
#include "Melon/Symbols/SymbolTable.h"
#include "Melon/Symbols/TypeSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"

#include "Melon/Optimizing/OptimizerInstruction.h"

namespace Melon {
	namespace Parsing {
		struct ParsingInfo;
	}

	namespace Nodes {
		class RootNode;

		///T NodePtr
		typedef Boxx::Pointer<class Node> NodePtr;

		///B ScanType
		/// The different things to scan for in a node
		enum class [[deprecated]] ScanType : Boxx::UByte {
			///T Self
			/// Used if self is used
			Self
		};

		///B Node
		/// Base for all nodes
		class Node {
		public:
			///H Members

			///T Scope
			/// The scope the node is in
			Symbols::Symbol* scope;

			///T File
			/// The file info of the node
			FileInfo file;

			///T Side Effect Scope
			Boxx::Optional<Symbols::ScopeList> sideEffectScope;

			///H Constructors

			///T Constructor
			Node(Symbols::Symbol* const scope, const FileInfo& file);
			~Node();

			///H Methods

			///T Type
			/// Returns the absolute type of the node
			virtual Symbols::TypeSymbol* Type() const;

			///T Types
			/// Returns the absolute types of the node
			/// Defaults to a list containing the result from <code>Type()</code>
			virtual Boxx::List<Symbols::TypeSymbol*> Types() const;

			///T Get Symbol
			/// Returns the symbol for the current node
			virtual Symbols::Symbol* GetSymbol() const;

			///T Get Size
			/// Gets the byte size of the node
			/// This is only used by the compile step
			virtual Boxx::UInt GetSize() const;

			///T Is Scope
			/// Whether or not the node is a scope
			virtual bool IsScope() const;

			///T Compile
			/// Compiles the node
			virtual CompiledNode Compile(CompileInfo& info) = 0;

			///T To Melon
			/// Converts the node to melon
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const = 0;

			///T Include Scan
			/// Scans the node for potential extra includes
			virtual void IncludeScan(Parsing::ParsingInfo& info);

			///T Scan
			/// Scans the node for potential errors
			virtual ScanResult Scan(ScanInfoStack& info);

			///T Has Side Effects
			/// Checks if the node has side effects outside of the specified scope
			///M
			bool HasSideEffects();
			bool HasSideEffects(const Symbols::ScopeList& scope);
			///M

			///T Get Side Effect Scope
			/// Gets the side effect scope 
			Symbols::ScopeList GetSideEffectScope(const bool assign);

			///T Optimize
			/// Optimizes the node
			///R NodePtr node: The optimized node. If the node was not optimized, this will be <code>nullptr</code>
			virtual NodePtr Optimize(OptimizeInfo& info);

			///T Is Immediate
			/// <code>true</code> if the compiled node is an immediate value
			/// Defaults to <code>false</code>
			virtual bool IsImmediate() const;

			///T Get Immediate
			/// Gets the immediate value of the node
			virtual Boxx::Long GetImmediate() const;

			///H Static functions
			
			///T Scan Assignment
			/// Scans an assignment
			static ScanResult ScanAssignment(NodePtr var, NodePtr value, ScanInfoStack& info, const FileInfo& file);

			///T Compile Assignment
			/// Compiles an assignment
			static CompiledNode CompileAssignment(NodePtr var, NodePtr value, CompileInfo& info, const FileInfo& file);

			///T Is Empty
			/// Checks if a node is an empty empty node
			static bool IsEmpty(const NodePtr& node);

			///T Combine Side Effects
			/// Combines two side effect scopes by returning the most global of the two
			static Symbols::ScopeList CombineSideEffects(const Symbols::ScopeList& scope1, const Symbols::ScopeList& scope2);

			static RootNode* root;

		protected:

			///T Find Side Effect Scope
			/// Finds the most global scope 
			virtual Symbols::ScopeList FindSideEffectScope(const bool assign);
		};

		///B IncludeScanError
		/// Used in include scan if more info is needed for the scan
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