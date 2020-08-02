#pragma once

#include "Types.h"
#include "Error.h"
#include "Array.h"

///N StaticArray
namespace Boxx {
	///B StaticArray
	/// An array class for arrays with a static size
	/// It automatically calls <code>delete[]</code> on the internal array when it is no longer in use
	///M
	template <class T, UInt S>
	class StaticArray {
	///M
	public:
		///H Constructors

		///T Empty array
		/// Creates an array whith the same size as template argument <code>S</code>
		StaticArray();

		///T Array pointer
		/// Creates an array from an array pointer
		///A T* const arr: Must be allocated with <code>new</code> and must have the same size as template argument <code>S</code>
		StaticArray(T* const array);

		///T Array
		/// Creates a static array from a regular array
		///E ArraySizeError: Thrown if the array size is not the same as template argument <code>S</code>
		StaticArray(const Array<T>& arr);

		StaticArray(const StaticArray<T, S>& arr);
		StaticArray(StaticArray<T, S>&& arr);
		~StaticArray();

		///H Methods

		///T Size
		/// Returns the size of the array
		UInt Size() const;

		///T Last value
		/// Returns an element near the end of the array
		///A const UInt pos: The position from the last element
		///M
		T& Last(const UInt pos = 0);
		const T& Last(const UInt pos = 0) const;
		///M

		///H Operators

		///T Indexing
		/// Used to get/set the value at a specific index
		///M
		T& operator[](const UInt i);
		const T operator[](const UInt i) const;
		///M

		///T Assignment
		///A T* const arr: Must be allocated with <code>new</code> and must have the same size as template argument <code>S</code>
		void operator=(T* const arr);

		void operator=(const StaticArray<T, S>& arr);
		void operator=(StaticArray<T, S>&& arr);

		///T Equality
		/// Checks if two arrays have the same pointer
		///M
		bool operator==(T* const arr) const;
		bool operator!=(T* const arr) const;
		bool operator==(const StaticArray<T, S>& arr) const;
		bool operator!=(const StaticArray<T, S>& arr) const;
		///M

		///H Conversions

		///T Conversion to array pointer
		/// Conversion to the stored array pointer
		explicit operator T*() const;

		///T Conversion to bool
		///R bool: <code>true</code> if the array pointer is not <code>nullptr</code>
		/// <code>false</code> otherwise
		explicit operator bool() const;

		///T Conversion to Array
		/// Conversion to regular Array
		operator Array<T>() const;

		///H Iterators

		///T Iterator
		/// Iterates over all values in the array
		///S for (T t : array)
		T* begin();
		const T* begin() const;
		T* end();
		const T* end() const;

	private:
		T* array;
		UInt* ref;

		void Cleanup();
	};

	///B Array error
	/// Base class for all array errors
	class ArrayError : public Error {
	public:
		ArrayError():Error(){}
		ArrayError(const char* const msg):Error(msg){}
	};

	///B Array size error
	/// Used for array size mismatch
	class ArraySizeError : public ArrayError {
	public:
		ArraySizeError():ArrayError(){}
		ArraySizeError(const char* const msg):ArrayError(msg){}
	};

	template <class T, UInt S>
	inline StaticArray<T, S>::StaticArray() {
		if (S > 0) {
			array = new T[S];
			ref = new UInt(1);
		}
		else {
			ref = nullptr;
			array = nullptr;
		}
	}

	template <class T, UInt S>
	inline StaticArray<T, S>::StaticArray(T* const array) {
		this->array = array;
		ref = new UInt(1);
	}

	template <class T, UInt S>
	inline StaticArray<T, S>::StaticArray(const Array<T>& arr) {
		if (S != arr.Size())
			throw ArraySizeError();

		array = arr.array;
		ref = arr.ref;
	
		if (ref != nullptr)
			(*ref)++;
	}

	template <class T, UInt S>
	inline StaticArray<T, S>::StaticArray(const StaticArray<T, S>& arr)  {
		array = arr.array;
		ref = arr.ref;
		
		if (ref != nullptr)
			(*ref)++;
	}

	template <class T, UInt S>
	inline StaticArray<T, S>::StaticArray(StaticArray<T, S>&& arr)  {
		array = arr.array;
		ref = arr.ref;
		arr.array = nullptr;
		arr.ref = nullptr;
	}

	template <class T, UInt S>
	inline StaticArray<T, S>::~StaticArray() {
		Cleanup();
	}

	template <class T, UInt S>
	inline UInt StaticArray<T, S>::Size() const {
		return S;
	}

	template <class T, UInt S>
	inline T& StaticArray<T, S>::Last(const UInt pos) {
		return array[Size() - pos - 1];
	}

	template <class T, UInt S>
	inline const T& StaticArray<T, S>::Last(const UInt pos) const {
		return array[Size() - pos - 1];
	}

	template <class T, UInt S>
	inline T& StaticArray<T, S>::operator[](const UInt i) {
		return array[i];
	}

	template <class T, UInt S>
	inline const T StaticArray<T, S>::operator[](const UInt i) const {
		return array[i];
	}

	template <class T, UInt S>
	inline void StaticArray<T, S>::operator=(T* const arr) {
		if (array == arr)
			return;

		Cleanup();
		array = array;
		ref = new UInt(1);
	}

	template <class T, UInt S>
	inline void StaticArray<T, S>::operator=(const StaticArray<T, S>& arr) {
		if (array == arr.array)
			return;

		Cleanup();
		array = arr.array;
		ref = arr.ref;

		if (ref != nullptr)
			(*ref)++;
	}

	template <class T, UInt S>
	inline void StaticArray<T, S>::operator=(StaticArray<T, S>&& arr) {
		Cleanup();
		array = arr.array;
		ref = arr.ref;
		arr.array = nullptr;
		arr.ref = nullptr;
	}

	template <class T, UInt S>
	inline bool StaticArray<T, S>::operator==(T* const arr) const {
		return array == arr;
	}

	template <class T, UInt S>
	inline bool StaticArray<T, S>::operator!=(T* const arr) const {
		return array != arr;
	}

	template <class T, UInt S>
	inline bool StaticArray<T, S>::operator==(const StaticArray<T, S>& arr) const {
		return array == arr.array;
	}

	template <class T, UInt S>
	inline bool StaticArray<T, S>::operator!=(const StaticArray<T, S>& arr) const {
		return array != arr.array;
	}

	template <class T, UInt S>
	inline StaticArray<T, S>::operator T*() const {
		return array;
	}

	template <class T, UInt S>
	inline StaticArray<T, S>::operator bool() const {
		return array != nullptr;
	}

	template <class T, UInt S>
	inline StaticArray<T, S>::operator Array<T>() const {
		Array<T> arr = Array<T>(S, array);
		arr.ref = ref;
		(*ref)++;
		return arr;
	}

	template <class T, UInt S>
	inline void StaticArray<T, S>::Cleanup() {
		if (ref != nullptr && --(*ref) < 1) {
			if (array != nullptr && *ref == 0) {
				delete[] array;
				delete ref;
			}
			
			array = nullptr;
			ref = nullptr;
		}
	}

	template <class T, UInt S>
	inline T* StaticArray<T, S>::begin() {
		return array;
	}

	template <class T, UInt S>
	inline const T* StaticArray<T, S>::begin() const {
		return array;
	}

	template <class T, UInt S>
	inline T* StaticArray<T, S>::end() {
		return array + S;
	}

	template <class T, UInt S>
	inline const T* StaticArray<T, S>::end() const {
		return array + S;
	}

	template <class T, UInt S>
	inline bool operator==(T* const arr1, const StaticArray<T, S>& arr2) {
		return arr2 == arr1;
	}

	template <class T, UInt S>
	inline bool operator!=(T* const arr1, const StaticArray<T, S>& arr2) {
		return arr2 != arr1;
	}
}
