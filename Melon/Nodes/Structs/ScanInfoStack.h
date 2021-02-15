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

			///H Properties
			/// Used to get/set properties of the top value of the stack

			///T Get Init
			bool Init() const {
				return stack.Peek().init;
			}

			///T Set Init
			void Init(const bool init) {
				stack.Peek().init = init;
			}

			///T Get Assign
			bool Assign() const {
				return stack.Peek().assign;
			}

			///T Set Assign
			void Assign(const bool assign) {
				stack.Peek().assign = assign;
			}

			///T Get UseFunction
			bool UseFunction() const {
				return stack.Peek().useFunction;
			}

			///T Set UseFunction
			void UseFunction(const bool use) {
				stack.Peek().useFunction = use;
			}

			///T Get Type
			Symbols::TypeSymbol* Type() const {
				return stack.Peek().type;
			}

			///T Set Type
			void Type(Symbols::TypeSymbol* const type) {
				stack.Peek().type = type;
			}

			///T Get File
			FileInfo File() const {
				return stack.Peek().file;
			}

			///T Set File
			void File(const FileInfo file) {
				stack.Peek().file = file;
			}

			///T Get ScopeInfo
			ScopeInfo& ScopeInfo() {
				return stack.Peek().scopeInfo;
			}

			///T Set ScopeInfo
			void ScopeInfo(const Nodes::ScopeInfo info) {
				stack.Peek().scopeInfo = info;
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
