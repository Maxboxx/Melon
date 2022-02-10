#pragma once

#include "Types.h"
#include "ScanInfo.h"

#include "Boxx/Pointer.h"
#include "Boxx/Stack.h"

namespace Melon {
	namespace Symbols {
		class VariableSymbol;
		class TypeSymbol;
		class FunctionSymbol;
	}

	///N Melon::Nodes
	namespace Nodes {
		class FunctionBody;

		///H Structs

		/// A stack of ScanInfo values.
		class ScanInfoStack {
		public:
			ScanInfoStack() {
				stack.Push(ScanInfo());
			}

			/// Removes the top value form the stack.
			void Pop() {
				stack.Pop();
			}

			/// Adds a copy of the top value to the stack.
			void Push() {
				stack.Push(stack.Peek());
			}

			/// Get the top value of the stack.
			ScanInfo& Get() {
				return stack.Peek();
			}

			/// Sets the top of the stack.
			void Set(const ScanInfo& info) {
				stack.Set(info);
			}

			/// Used to get/set members of the top value of the stack.
			ScanInfo* operator->() {
				return &stack.Peek();
			}

			const ScanInfo* operator->() const {
				return &stack.Peek();
			}

			/// All used variables.
			Boxx::Set<Symbols::VariableSymbol*> usedVariables;

			/// All used types.
			Boxx::Set<Symbols::TypeSymbol*> usedTypes;

			/// All used functions.
			Boxx::Set<Symbols::FunctionSymbol*> usedFunctions;

			/// All function nodes.
			Boxx::Collection<Weak<FunctionBody>> functions;

		private:
			Boxx::Stack<ScanInfo> stack;
		};
	}
}
