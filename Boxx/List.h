#pragma once

#include "Types.h"
#include "Array.h"
#include "Math.h"
#include "Optional.h"

#include <cstring>

///N List
namespace Boxx {
	class String;
	class Regex;

	///B List
	/// A List is a collection of items stored in a specific order.
	///M
	template <class T>
	class List {
	///M
	public:
		///H Constructors

		///T Empty list
		/// Creates an empty list
		List();

		///T Reserved capacity
		/// Creates a list with a reserved capacity
		explicit List(const UInt capacity);

		List(const List<T>& list);
		List(List<T>&& list) noexcept;
		~List();

		///H Methods

		///T List size
		/// Returns the current size of the list
		UInt Size() const;

		///T List capacity
		/// Returns the current capacity of the list
		UInt Capacity() const;

		///T Is Empty
		/// Checks if the list is empty
		bool IsEmpty() const;

		///T Add value
		/// Adds a value to the end of the list
		void Add(const T& value);

		///T Insert value
		/// Inserts a value at the specified index in the list
		void Insert(const UInt index, const T& value);

		///T Remove value
		/// Removes the first occurrence of the specified value from the list
		void Remove(const T& value);

		///T Remove all
		/// Removes all occurrences of the specified value from the list
		void RemoveAll(const T& value);

		///T Remove at
		/// Removes elements from the list
		///A const UInt index: The position to remove at
		///A const UInt numElements: The amount of elements to remove
		void RemoveAt(const UInt index, const UInt numElements = 1);

		///T Remove last
		/// Removes an element from the end of the list
		///A const UInt pos: The position from the last element to remove at
		void RemoveLast(const UInt pos = 0);

		///T Clear list
		/// Removes everything from the list
		void Clear();

		///T Find index
		/// Returns the index of the first occurrence of the specified value
		///R Optional<UInt>: Contains the index of the value if the value was found
		Optional<UInt> Find(const T& value) const;

		///T Contains
		/// Checks if the list contains a specific value
		bool Contains(const T& value) const;

		///T Last value
		/// Returns an element near the end of the list
		///A const UInt pos: The position from the last element
		///M
		T& Last(const UInt pos = 0);
		const T& Last(const UInt pos = 0) const;
		///M

		///T Convert to array
		/// Converts the list to an array
		Array<T> ToArray() const;

		///T Copy
		/// Creates a copy of the list
		List<T> Copy() const;

		///H Operators

		///T Index operators
		/// Used to get/set the value at a specific index
		///M
		T& operator[](const UInt i);
		const T& operator[](const UInt i) const;
		///M
		
		void operator=(const List<T>& list);
		void operator=(List<T>&& list) noexcept;

		///T Equality
		/// Checks if two lists are the same
		///M
		bool operator==(const List<T>& list) const;
		bool operator!=(const List<T>& list) const;
		///M

		///H Iterators

		///T Iterator
		/// Iterates over all values in the list
		///S for (T t : list)
		T* begin();
		const T* begin() const;
		T* end();
		const T* end() const;
	private:
		friend class String;

		struct ListRef {
			T* list;
			UInt size;
			UInt capacity;

			~ListRef() {
				delete[] list;
			}
		};

		static const UInt minGrow = 4;
		static const UInt maxGrow = 1 << 16;

		ListRef* list;
		UInt* ref;

		void Cleanup();
		void Grow();
	};

	template <class T>
	inline List<T>::List() {
		list = new ListRef();
		list->list = nullptr;
		list->size = 0;
		list->capacity = 0;

		ref = new UInt(1);
	}

	template <class T>
	inline List<T>::List(const UInt size) {
		list = new ListRef();
		list->list = new T[size];
		list->size = 0;
		list->capacity = size;

		ref = new UInt(1);
	}

	template <class T>
	inline List<T>::List(const List<T>& list)  {
		this->list = list.list;
		ref = list.ref;

		if (ref != nullptr)
			(*ref)++;
	}

	template <class T>
	inline List<T>::List(List<T>&& list) noexcept {
		this->list = list.list;
		this->ref = list.ref;
		list.list = nullptr;
		list.ref = nullptr;
	}

	template <class T>
	inline List<T>::~List() {
		Cleanup();
	}

	template <class T>
	inline UInt List<T>::Size() const {
		return list->size;
	}

	template <class T>
	inline UInt List<T>::Capacity() const {
		return list->capacity;
	}

	template <class T>
	inline bool List<T>::IsEmpty() const {
		return list->size == 0;
	}

	template <class T>
	inline void List<T>::Add(const T& value) {
		if (list->size >= list->capacity)
			Grow();

		list->list[list->size++] = value;
	}

	template <class T>
	inline void List<T>::Insert(const UInt index, const T& value) {
		if (Size() >= Capacity())
			Grow();

		T* dest = &list->list[Size()];
		T* const first = &list->list[index];
		T* last = &list->list[Size() - 1];

		if (std::is_trivially_copyable<T>::value)
			memmove(first + 1, first, sizeof(T) * (Size() - index));
		else for (; dest != first; --dest, --last)
			*dest = std::move(*last);

		*first = value;
		list->size++;
	}

	template <class T>
	inline void List<T>::Remove(const T& value) {
		for (UInt i = 0; i < Size(); i++) {
			if (list->list[i] == value) {
				RemoveAt(i);
				break;
			}
		}
	}

	template <class T>
	inline void List<T>::RemoveAll(const T& value) {
		for (UInt i = 0; i < Size(); i++) {
			if (list->list[i] == value) {
				RemoveAt(i);
				i--;
			}
		}
	}

	template <class T>
	inline void List<T>::RemoveAt(const UInt index, const UInt numElements) {
		T* dest = &list->list[index];
		T* first = &list->list[index + numElements];
		T* const last = &list->list[Size() - numElements];

		if (std::is_trivially_copyable<T>::value)
			memmove(dest, first, sizeof(T) * (Size() - index - numElements));
		else for (; dest != last; ++dest, ++first)
			*dest = std::move(*first);

		list->size -= numElements;
	}

	template <class T>
	inline void List<T>::RemoveLast(const UInt pos) {
		RemoveAt(Size() - 1 - pos);
	}

	template <class T>
	inline void List<T>::Clear() {
		delete[] list->list;
		list->list = new T[list->capacity];
		list->size = 0;
	}

	template <class T>
	inline Optional<UInt> List<T>::Find(const T& value) const {
		for (UInt i = 0; i < Size(); i++)
			if (list->list[i] == value)
				return i;

		return nullptr;
	}

	template <class T>
	inline bool List<T>::Contains(const T& value) const {
		for (UInt i = 0; i < Size(); i++) if (list->list[i] == value) return true;
		return false;
	}

	template <class T>
	inline T& List<T>::Last(const UInt pos) {
		return list->list[Size() - pos - 1];
	}

	template <class T>
	inline const T& List<T>::Last(const UInt pos) const {
		return list->list[Size() - pos - 1];
	}

	template <class T>
	inline Array<T> List<T>::ToArray() const {
		Array<T> arr = Array<T>(Size());

		for (UInt i = 0; i < Size(); i++) {
			arr[i] = (*this)[i];
		}

		return arr;
	}

	template <class T>
	inline List<T> List<T>::Copy() const {
		List<T> lst{Size()};
		lst.list->size = Size();

		T* const last = &lst.list->list[Size()];
		T* source = list->list;

		if (std::is_trivially_copyable<T>::value)
			memmove(lst.list->list, list->list, sizeof(T) * Size());
		else for (T* dest = lst.list->list; dest != last; dest++, source++)
			*dest = *source;

		return lst;
	}

	template <class T>
	inline T& List<T>::operator[](const UInt i) {
		return list->list[i];
	}

	template <class T>
	inline const T& List<T>::operator[](const UInt i) const {
		return list->list[i];
	}

	template <class T>
	inline void List<T>::operator=(const List<T>& list) {
		if (this->list == list.list)
			return;

		Cleanup();

		this->list = list.list;
		ref = list.ref;

		if (ref != nullptr)
			(*ref)++;
	}

	template <class T>
	inline void List<T>::operator=(List<T>&& list) noexcept {
		Cleanup();
		this->list = list.list;
		this->ref = list.ref;
		list.list = nullptr;
		list.ref = nullptr;
	}

	template <class T>
	inline bool List<T>::operator==(const List<T>& list) const {
		return this->list == list.list;
	}

	template <class T>
	inline bool List<T>::operator!=(const List<T>& list) const {
		return this->list != list.list;
	}

	template <class T>
	inline void List<T>::Cleanup() {
		if (ref != nullptr && --(*ref) < 1) {
			if (list != nullptr && *ref == 0) {
				delete list;
				delete ref;
			}
			
			list = nullptr;
			ref = nullptr;
		}
	}

	template <class T>
	inline void List<T>::Grow() {
		int grow = list->capacity >> 1;

		if (grow < minGrow)
			grow = minGrow;
		else if (grow > maxGrow)
			grow = maxGrow;

		const int newCapacity = list->capacity + grow;
		T* const newList = new T[newCapacity];

		if (std::is_trivially_copyable<T>::value)
			memcpy(newList, list->list, sizeof(T) * Size());
		else for (UInt i = 0; i < list->size; i++)
			newList[i] = std::move(list->list[i]);

		delete[] list->list;

		list->list = newList;
		list->capacity = newCapacity;
	}

	template <class T>
	inline T* List<T>::begin() {
		return list->list;
	}

	template <class T>
	inline const T* List<T>::begin() const {
		return list->list;
	}

	template <class T>
	inline T* List<T>::end() {
		return list->list + list->size;
	}

	template <class T>
	inline const T* List<T>::end() const {
		return list->list + list->size;
	}
}
