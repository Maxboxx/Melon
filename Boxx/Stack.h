#pragma once

#include "Types.h"
#include "List.h"

///N Stack
namespace Boxx {

	///B Stack
	/// A stack data structure for storing values
	///M
	template <class T>
	class Stack {
	///M
	public:
		///H Constructors

		///T Empty stack
		Stack();

		///T Reserved capacity
		/// Creates a stack with a reserved capacity
		explicit Stack(const UInt capacity);

		Stack(const Stack<T>& stack);
		Stack(Stack<T>&& stack);
		~Stack();

		///H Methods

		///T Push
		/// Push a value to the top of the stack
		void Push(const T& value);

		///T Peek
		/// Return the item on the top of the stack
		const T& Peek() const;

		///T Pop
		/// Remove and return the item on the top of the stack
		T& Pop();

		///T Size
		/// Get the current size of the stack
		UInt Size() const;

		///T Capacity
		/// Get the current capacity of the stack
		UInt Capacity() const;

		///T Is Empty
		/// Checks if the stack is empty
		bool IsEmpty() const;

		///H Operators

		void operator=(const Stack<T>& stack);
		void operator=(Stack<T>&& stack);

		///T Equality
		/// Checks if two stacks are the same
		///M
		bool operator==(const Stack<T>& stack) const;
		bool operator!=(const Stack<T>& stack) const;
		///M

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
		return stack.Empty();
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
