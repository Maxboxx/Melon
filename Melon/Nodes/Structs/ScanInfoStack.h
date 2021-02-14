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
		class ScanInfoStack {
		public:
			ScanInfoStack() {
				stack.Push(ScanInfo());
			}

			void Pop() {stack.Pop();}
			void Push() {stack.Push(stack.Peek());}
			ScanInfo& Get() {return stack.Peek();}
			void Set(const ScanInfo& info) {stack.Set(info);}

			bool Init() const {return stack.Peek().init;}
			void Init(const bool init) {stack.Peek().init = init;}

			bool Assign() const {return stack.Peek().assign;}
			void Assign(const bool assign) {stack.Peek().assign = assign;}

			bool UseFunction() const {return stack.Peek().useFunction;}
			void UseFunction(const bool use) {stack.Peek().useFunction = use;}

			Symbols::TypeSymbol* Type() const {return stack.Peek().type;}
			void Type(Symbols::TypeSymbol* const type) {stack.Peek().type = type;}

			FileInfo File() const {return stack.Peek().file;}
			void File(const FileInfo file) {stack.Peek().file = file;}

			ScopeInfo& ScopeInfo() {return stack.Peek().scopeInfo;}
			void ScopeInfo(const Nodes::ScopeInfo info) {stack.Peek().scopeInfo = info;}

			Boxx::Set<Symbols::VariableSymbol*> usedVariables;
			Boxx::Set<Symbols::TypeSymbol*> usedTypes;
			Boxx::Set<Symbols::FunctionSymbol*> usedFunctions;

			Boxx::Collection<Boxx::Pointer<Node>> functions;

		private:
			Boxx::Stack<ScanInfo> stack;
		};
	}
}
