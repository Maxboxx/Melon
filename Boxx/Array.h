#pragma once

#include "Types.h"
#include "Error.h"

///N Array
namespace Boxx {
	template<class T, UInt S>
	class StaticArray;

	///B Array
	/// An array class that keeps track of its own size.
	/// It automatically calls <code>delete[]</code> on the internal array when it is no longer in use
	///M
	template <class T>
	class Array {
	///M
	public:
		///H Constructors

		///T Empty array
		/// Creates an empty array
		Array();

		///T Size
		/// Creates an array with a specific size
		explicit Array(const UInt size);

		///T Array pointer and size
		/// Creates an array with the specified size and array pointer
		///A T* const array: Must be allocated with <code>new</code> and have the same size as the value of <code>size</code>
		explicit Array(const UInt size, T* const array);

		Array(const Array<T>& arr);
		Array(Array<T>&& arr) noexcept;
		~Array();

		///H Methods

		///T Size
		/// Returns the size of the array
		UInt Size() const;

		///T Is Empty
		/// Checks if the size of the array is <code>0</code>
		bool IsEmpty() const;

		///T Contains
		/// Checks if the array contains the specified value
		bool Contains(const T& value) const;

		///T Last value
		/// Returns an element at the end of the array
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

		void operator=(const Array<T>& arr);
		void operator=(Array<T>&& arr) noexcept;

		///T Equality
		/// Checks if the arrays have the same array pointer
		///M
		bool operator==(T* const arr) const;
		bool operator!=(T* const arr) const;
		bool operator==(const Array<T>& arr) const;
		bool operator!=(const Array<T>& arr) const;
		///M

		///H Conversions

		///T Conversion to array pointer
		/// Returns the internal array pointer
		explicit operator T*() const;

		///T Conversion to bool
		///R bool: <code>true</code> if the array pointer is not <code>nullptr</code>
		/// <code>false</code> otherwise
		explicit operator bool() const;

		///H Iterators

		///T Iterator
		/// Iterates over all values in the array
		///S for (T t : array)
		T* begin();
		const T* begin() const;
		T* end();
		const T* end() const;

	private:
		template<class, UInt>
		friend class StaticArray;

		T* array;
		UInt size = 0;
		UInt* ref;

		void Cleanup();
	};

	template <class T>
	inline Array<T>::Array() {
		ref = nullptr;
		array = nullptr;
	}

	template <class T>
	inline Array<T>::Array(const UInt size) {
		if (size > 0) {
			array = new T[size];
			this->size = size;
			ref = new UInt(1);
		}
		else {
			ref = nullptr;
			array = nullptr;
		}
	}

	template <class T>
	inline Array<T>::Array(const UInt size, T* const array) {
		if (size > 0) {
			this->array = array;
			this->size = size;
			ref = new UInt(1);
		}
		else {
			ref = nullptr;
			this->array = nullptr;
		}
	}

	template <class T>
	inline Array<T>::Array(const Array<T>& arr) {
		array = arr.array;
		size = arr.size;
		ref = arr.ref;

		if (ref != nullptr)
			(*ref)++;
	}

	template <class T>
	inline Array<T>::Array(Array<T>&& arr) noexcept {
		array = arr.array;
		size = arr.size;
		ref = arr.ref;
		arr.array = nullptr;
		arr.ref = nullptr;
	}

	template <class T>
	inline Array<T>::~Array() {
		Cleanup();
	}

	template <class T>
	inline UInt Array<T>::Size() const {
		return size;
	}

	template <class T>
	inline bool Array<T>::IsEmpty() const {
		return size == 0;
	}

	template <class T>
	inline bool Array<T>::Contains(const T& value) const {
		for (UInt i = 0; i < Size(); i++) if (array[i] == value) return true;
		return false;
	}

	template <class T>
	inline T& Array<T>::Last(const UInt pos) {
		return array[Size() - pos - 1];
	}

	template <class T>
	inline const T& Array<T>::Last(const UInt pos) const {
		return array[Size() - pos - 1];
	}

	template <class T>
	inline T& Array<T>::operator[](const UInt i) {
		return array[i];
	}

	template <class T>
	inline const T Array<T>::operator[](const UInt i) const {
		return array[i];
	}

	template <class T>
	inline void Array<T>::operator=(const Array<T>& arr) {
		if (array == arr.array)
			return;

		Cleanup();
		size = arr.size;
		array = arr.array;
		ref = arr.ref;

		if (ref != nullptr)
			(*ref)++;
	}

	template <class T>
	inline void Array<T>::operator=(Array<T>&& arr) noexcept {
		Cleanup();
		size = arr.size;
		array = arr.array;
		ref = arr.ref;
		arr.array = nullptr;
		arr.ref = nullptr;
	}

	template <class T>
	inline bool Array<T>::operator==(T* const arr) const {
		return array == arr;
	}

	template <class T>
	inline bool Array<T>::operator!=(T* const arr) const {
		return array != arr;
	}

	template <class T>
	inline bool Array<T>::operator==(const Array<T>& arr) const {
		return array == arr.array;
	}

	template <class T>
	inline bool Array<T>::operator!=(const Array<T>& arr) const {
		return array != arr.array;
	}

	template <class T>
	inline Array<T>::operator T*() const {
		return array;
	}

	template <class T>
	inline Array<T>::operator bool() const {
		return array != nullptr;
	}

	template <class T>
	inline void Array<T>::Cleanup() {
		if (ref != nullptr && --(*ref) < 1) {
			if (array != nullptr && *ref == 0) {
				delete[] array;
				delete ref;
			}
			
			array = nullptr;
			ref = nullptr;
		}
	}

	template <class T>
	inline T* Array<T>::begin() {
		return array;
	}

	template <class T>
	inline const T* Array<T>::begin() const {
		return array;
	}

	template <class T>
	inline T* Array<T>::end() {
		return array + size;
	}

	template <class T>
	inline const T* Array<T>::end() const {
		return array + size;
	}

	template <class T>
	inline bool operator==(T* const arr1, const Array<T>& arr2) {
		return arr2 == arr1;
	}

	template <class T>
	inline bool operator!=(T* const arr1, const Array<T>& arr2) {
		return arr2 != arr1;
	}
}
