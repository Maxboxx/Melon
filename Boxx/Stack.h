#pragma once

#include "Types.h"
#include "List.h"

///[Settings] block: indent

///[Namespace] Boxx
namespace Boxx {
	///[Heading] Collections

	///[Title] Stack
	/// A stack of items.
	///[Block] Stack
	///M
	template <class T>
	class Stack final {
	///M
	public:
		///[Heading] Constructors

		/// Creates an empty stack.
		Stack();

		/// Creates a stack with a reserved capacity.
		explicit Stack(const UInt capacity);

		Stack(const Stack<T>& stack);
		Stack(Stack<T>&& stack);
		~Stack();

		///[Heading] Methods

		/// Push a value to the top of the stack.
		void Push(const T& value);

		/// Sets the value at the top of the stack.
		void Set(const T& value);

		/// Returns the item on the top of the stack.
		///M
		T& Peek();
		const T& Peek() const;
		///M

		/// Remove and return the item on the top of the stack.
		T& Pop();

		/// Get the current size of the stack.
		UInt Size() const;

		/// Get the current capacity of the stack.
		UInt Capacity() const;

		/// Checks if the stack is empty.
		bool IsEmpty() const;

		/// Creates a copy of the stack.
		Stack<T> Copy() const;

		///[Heading] Operators

		void operator=(const Stack<T>& stack);
		void operator=(Stack<T>&& stack);

		/// Checks if two stacks are the same.
		bool operator==(const Stack<T>& stack) const;

		/// Checks if two stacks are not the same.
		bool operator!=(const Stack<T>& stack) const;

	private:
		List<T> stack;
	};

	template <class T>
	inline Stack<T>::Stack() {
	
	}

	template <class T>
	inline Stack<T>::Stack(const UInt capacity) {
		stack = List<T>(capacity);
	}

	template <class T>
	inline Stack<T>::Stack(const Stack<T>& stack) {
		this->stack = stack.stack;
	}

	template <class T>
	inline Stack<T>::Stack(Stack<T>&& stack) {
		this->stack = std::move(stack.stack);
	}

	template <class T>
	inline Stack<T>::~Stack() {

	}

	template <class T>
	inline void Stack<T>::Push(const T& value) {
		stack.Add(value);
	}

	template <class T>
	inline void Stack<T>::Set(const T& value) {
		stack[stack.Size() - 1] = value;
	}

	template <class T>
	inline T& Stack<T>::Peek() {
		return stack.Last();
	}

	template <class T>
	inline const T& Stack<T>::Peek() const {
		return stack.Last();
	}

	template <class T>
	inline T& Stack<T>::Pop() {
		T& value = stack.Last();
		stack.RemoveAt(stack.Size() - 1);
		return value;
	}

	template <class T>
	inline UInt Stack<T>::Size() const {
		return stack.Size();
	}

	template <class T>
	inline UInt Stack<T>::Capacity() const {
		return stack.Capacity();
	}

	template <class T>
	inline bool Stack<T>::IsEmpty() const {
		return stack.IsEmpty();
	}

	template <class T>
	inline Stack<T> Stack<T>::Copy() const {
		Stack<T> s;
		s.stack = stack.Copy();
		return s;
	}

	template <class T>
	inline void Stack<T>::operator=(const Stack<T>& stack) {
		this->stack = stack.stack;
	}

	template <class T>
	inline void Stack<T>::operator=(Stack<T>&& stack) {
		this->stack = std::move(stack.stack);
	}

	template <class T>
	inline bool Stack<T>::operator==(const Stack<T>& stack) const {
		return this->stack == stack.stack;
	}

	template <class T>
	inline bool Stack<T>::operator!=(const Stack<T>& stack) const {
		return this->stack != stack.stack;
	}
}
