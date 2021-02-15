#pragma once

#include "ScanInfo.h"

#include "Boxx/Pointer.h"
#include "Boxx/Stack.h"

namespace Melon {
	namespace Symbols {
		class VariableSymbol;
		class TypeSymbol;
		class FunctionSymbol;
	}

	namespace Nodes {
		class Node;

		///B ScanInfoStack
		/// A stack of ScanInfo values
		class ScanInfoStack {
		public:
			ScanInfoStack() {
				stack.Push(ScanInfo());
			}

			///H Methods

			///T Pop
			/// Removes the top value form the stack
			void Pop() {
				stack.Pop();
			}

			///T Push
			/// Adds a copy of the top value to the stack
			void Push() {
				stack.Push(stack.Peek());
			}

			///T Get
			/// Get the top value of the stack
			ScanInfo& Get() {
				return stack.Peek();
			}

			///T Set
			/// Sets the top of the stack
			void Set(const ScanInfo& info) {
				stack.Set(info);
			}

			///T Structure dereference
			/// Used to get/set members of the top value of the stack
			ScanInfo* operator->() {
				return &stack.Peek();
			}

			const ScanInfo* operator->() const {
				return &stack.Peek();
			}

			///H Members

			///T Used Variables
			Boxx::Set<Symbols::VariableSymbol*> usedVariables;

			///T Used Types
			Boxx::Set<Symbols::TypeSymbol*> usedTypes;

			///T Used Functions
			Boxx::Set<Symbols::FunctionSymbol*> usedFunctions;

			///T Functions
			Boxx::Collection<Boxx::Pointer<Node>> functions;

		private:
			Boxx::Stack<ScanInfo> stack;
		};
	}
}
