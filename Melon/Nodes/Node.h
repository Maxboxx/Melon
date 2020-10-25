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

#include "ScopeInfo.h"

#include "Melon/Symbols/ScopeList.h"
#include "Melon/Symbols/Symbols.h"

#include "Melon/Optimizing/OptimizerInstruction.h"

namespace Melon {
	namespace Parsing {
		struct ParsingInfo;
	}

	namespace Nodes {

		///T NodePtr
		typedef Boxx::Pointer<class Node> NodePtr;

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

		///B StackPtr
		/// Contains information about the stack pointer
		struct StackPtr {
			///H Members

			///T Stack frame
			Boxx::UInt frame = 0;

			///T Stack top
			Boxx::Long top = 0;

			///T Pointer size
			static const Boxx::UByte ptrSize = 8;

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

			///T Push frame
			///A bytes: The number of bytes to push on the stack frame
			void PushFrame(const Boxx::UInt bytes) {
				frame += bytes;
			}

			///T Pop frame
			///A bytes: The number of bytes to pop from the stack frame
			void PopFrame(const Boxx::UInt bytes) {
				frame -= bytes;
			}

			///T Push Expression
			/// Pushes the top of the stack and also pushes the frame if it runs out of space
			void PushExpr(const Boxx::UInt bytes, CompiledNode& cn) {
				top += bytes;

				if (top > frame) {
					cn.instructions.Add(Kiwi::Instruction(Kiwi::InstructionType::Push, top - frame));
					frame = top;
				}
			}

			///T Pop Expression
			/// Pops the frame back to a previous position
			void PopExpr(const Boxx::UInt frame, CompiledNode& cn) {
				if (this->frame > frame) {
					cn.instructions.Add(Kiwi::Instruction(Kiwi::InstructionType::Pop, this->frame - frame));
				}

				this->frame = frame;
			}

			///T Offset
			/// Gets the offset between the top and frame
			Boxx::Long Offset() const {
				return frame - top;
			}

			///T Offset
			/// Gets the offset between <code>index</code> and frame
			Boxx::Long Offset(const Boxx::Long index) const {
				return frame - index;
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
			bool useFunction = true;
			Symbols::Symbol symbol;
			FileInfo file;
			
			ScopeInfo scopeInfo;
		};

		///B ScanInfoStack
		class ScanInfoStack {
		public:
			ScanInfoStack() {
				stack.Push(ScanInfo());
			}

			void Pop() {stack.Pop();}
			void Push() {stack.Push(stack.Peek());}
			ScanInfo& Get() {return stack.Peek();}
			void Set(const ScanInfo& info) {stack.Set(info);}

			Boxx::Set<Symbols::ScopeList> usedVariables;
			Boxx::Set<Symbols::ScopeList> usedTypes;
			Boxx::Set<Symbols::ScopeList> usedFunctions;

			Boxx::Collection<NodePtr> functions;

		private:
			Boxx::Stack<ScanInfo> stack;
		};

		///B OptimizeInfo
		struct OptimizeInfo {
			bool optimized = false;

			Boxx::Set<Symbols::ScopeList> usedVariables;
			Boxx::Set<Symbols::ScopeList> usedTypes;
			Boxx::Set<Symbols::ScopeList> usedFunctions;
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

			///T To Mango
			/// Converts the node to mango
			virtual Boxx::Mango ToMango() const = 0;

			///T To Melon
			/// Converts the node to melon
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const = 0;

			///T Include Scan
			/// Scans the node for potential extra includes
			virtual void IncludeScan(Parsing::ParsingInfo& info);

			///T Scan
			/// Scans the node for potential errors
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info);

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
			static void ScanAssignment(NodePtr var, NodePtr value, ScanInfoStack& info, const FileInfo& file);

			///T Compile Assignment
			/// Compiles an assignment
			static CompiledNode CompileAssignment(NodePtr var, NodePtr value, CompileInfo& info, const FileInfo& file);

			///T Is Empty
			/// Checks if a node is an empty empty node
			static bool IsEmpty(const NodePtr& node);
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