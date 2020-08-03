#pragma once

#include "Boxx/Pointer.h"
#include "Boxx/List.h"
#include "Boxx/Mango.h"
#include "Boxx/Set.h"

#include "Kiwi/Kiwi.h"

#include "Melon/Symbols/ScopeList.h"
#include "Melon/Symbols/Symbols.h"

#include "Melon/Optimizing/OptimizerInstruction.h"

namespace Melon {
	namespace Nodes {

		///T NodePtr
		typedef Boxx::Pointer<class Node> NodePtr;

		///B StackPtr
		/// Contains information about the stack pointer
		struct StackPtr {
			///H Members

			///T Stack base
			Boxx::UInt base = 0;

			///T Stack top
			Boxx::Long top = 0;

			///T Pointer size
			Boxx::UByte ptrSize = 8;

			///H Methods

			///T Push
			///A bytes: The number of bytes to push on the stack
			void Push(const Boxx::UInt bytes) {
				top += bytes;
			}

			///T Pop
			///A bytes: The number of bytes to pop from stack
			void Pop(const Boxx::UInt bytes) {
				top -= bytes;
			}

			///T Push base
			///A bytes: The number of bytes to push on the stack base
			void PushBase(const Boxx::UInt bytes) {
				base += bytes;
			}

			///T Pop base
			///A bytes: The number of bytes to pop from the stack base
			void PopBase(const Boxx::UInt bytes) {
				base -= bytes;
			}

			///T Offset
			/// Gets the offset between the top and base
			Boxx::Long Offset() const {
				return base - top;
			}

			///T Offset
			/// Gets the offset between <code>index</code> and base
			Boxx::Long Offset(const Boxx::Long index) const {
				return base - index;
			}
		};

		///B CompileInfo
		/// Contains info about the compiliation
		struct CompileInfo {
			///T Next free label index
			Boxx::UInt label = 0;

			///T Stack
			StackPtr stack;

			///T Next free register index
			Boxx::UByte index = 0;

			///T Important stack values
			bool important = false;
		};

		///B CompiledNode
		/// Contains info about a compiled node
		struct CompiledNode {
			///T Instructions
			/// All instructions for the node
			Boxx::List<Optimizing::OptimizerInstruction> instructions;

			///T Argument
			/// The argument to use if the node returns something
			Kiwi::Argument argument;

			///T Size
			/// The size in bytes of the returned value;
			Boxx::UByte size;

			///T Add instruction
			/// Adds a list of instructions to the instruction list
			void AddInstructions(const Boxx::List<Optimizing::OptimizerInstruction>& instructions);
		};

		///B ScanType
		/// The different things to scan for in a node
		enum class ScanType : Boxx::UByte {
			///T Self
			/// Used if self is used
			Self
		};

		///B ScanInfo
		/// Used for info when scanning
		struct ScanInfo {
			bool init = false;
			bool assign = false;
			Symbols::Symbol symbol;
		};

		///B Node
		/// Base for all nodes
		class Node {
		public:
			///H Members

			///T Scope
			/// The scope the node is in
			Symbols::ScopeList scope;

			///T File
			/// The file info of the node
			FileInfo file;

			///H Constructors

			///T Constructor
			Node(const Symbols::ScopeList& scope, const FileInfo& file);
			~Node();

			///H Methods

			///T Type
			/// Returns the absolute type of the node
			virtual Symbols::ScopeList Type() const;

			///T Types
			/// Returns the absolute types of the node
			/// Defaults to a list containing the result from <code>Type()</code>
			virtual Boxx::List<Symbols::ScopeList> Types() const;

			///T Get Symbol
			/// Returns the symbol for the current node
			virtual Symbols::Symbol GetSymbol() const;

			///T Compile
			/// Compiles the node
			virtual CompiledNode Compile(CompileInfo& info) = 0;

			///T To Mango
			/// Converts the node to mango
			virtual Boxx::Mango ToMango() const = 0;

			///T Scan
			/// Scans the node for potential errors
			virtual Boxx::Set<ScanType> Scan(ScanInfo& info) const;

			///T Is Immediate
			/// <code>true</code> if the compiled node is an immediate value
			/// Defaults to <code>false</code>
			virtual bool IsImmediate() const;
		};
	}
}