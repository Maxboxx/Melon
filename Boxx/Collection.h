#pragma once

#include "Types.h"
#include "Array.h"
#include "Math.h"
#include "Optional.h"

#include <cstring>

///[Settings] block: indent

///[Namespace] Boxx
namespace Boxx {
	///[Heading] Collections

	///[Title] Collection
	/// A collection of items stored in no perticular order for fast insertions and deletions.
	///[Block] Collection
	///M
	template <class T>
	class Collection final {
	///M
	public:
		///[Heading] Constructors

		/// Creates an empty collection.
		Collection();

		/// Creates a collection with a reserved capacity.
		explicit Collection(const UInt capacity);

		Collection(const Collection<T>& c);
		Collection(Collection<T>&& c) noexcept;
		~Collection();

		///[Heading] Methods

		/// Returns the number of items in the collection.
		UInt Size() const;

		/// Returns the current capacity of the collection.
		UInt Capacity() const;

		/// Checks if the collection is empty.
		bool IsEmpty() const;

		/// Adds a value to the collection.
		void Add(const T& value);

		/// Removes the first occurrence of the specified value from the collection.
		void Remove(const T& value);

		/// Removes all occurrences of the specified value from the collection.
		void RemoveAll(const T& value);

		/// Removes an element from the collection at a specific index.
		void RemoveAt(const UInt index);

		/// Removes everything from the collection.
		void Clear();

		/// Finds the index of the first occurrence of the specified value.
		Optional<UInt> Find(const T& value) const;

		/// Checks if the collection contains a specific value.
		bool Contains(const T& value) const;

		/// Converts the collection to an array.
		Array<T> ToArray() const;

		/// Creates a copy of the collection.
		Collection<T> Copy() const;

		///[Heading] Operators

		/// Used to get/set the value at a specific index.
		///M
		T& operator[](const UInt i);
		const T& operator[](const UInt i) const;
		///M
		
		void operator=(const Collection<T>& c);
		void operator=(Collection<T>&& c) noexcept;

		/// Checks if two collections are the same.
		///M
		bool operator==(const Collection<T>& c) const;
		bool operator!=(const Collection<T>& c) const;
		///M

		///[Heading] Iterators

		/// Iterates over all values in the collection.
		///[Code] for (T t : collection)
		T* begin();
		const T* begin() const;
		T* end();
		const T* end() const;

	private:
		struct CollectionRef {
			T* list;
			UInt size;
			UInt capacity;

			~CollectionRef() {
				delete[] list;
			}
		};

		static const UInt minGrow = 4;
		static const UInt maxGrow = 1 << 16;

		CollectionRef* list;
		UInt* ref;

		void Cleanup();
		void Grow();
	};

	template <class T>
	inline Collection<T>::Collection() {
		list = new CollectionRef();
		list->list = nullptr;
		list->size = 0;
		list->capacity = 0;

		ref = new UInt(1);
	}

	template <class T>
	inline Collection<T>::Collection(const UInt capacity) {
		list = new CollectionRef();
		list->list = new T[capacity];
		list->size = 0;
		list->capacity = capacity;

		ref = new UInt(1);
	}

	template <class T>
	inline Collection<T>::Collection(const Collection<T>& c)  {
		list = c.list;
		ref = c.ref;

		if (ref != nullptr)
			(*ref)++;
	}

	template <class T>
	inline Collection<T>::Collection(Collection<T>&& c) noexcept  {
		list = c.list;
		ref = c.ref;
		c.list = nullptr;
		c.ref = nullptr;
	}

	template <class T>
	inline Collection<T>::~Collection() {
		Cleanup();
	}

	template <class T>
	inline UInt Collection<T>::Size() const {
		return list->size;
	}

	template <class T>
	inline UInt Collection<T>::Capacity() const {
		return list->capacity;
	}

	template <class T>
	inline bool Collection<T>::IsEmpty() const {
		return list->size == 0;
	}

	template <class T>
	inline void Collection<T>::Add(const T& value) {
		if (list->size >= list->capacity)
			Grow();

		list->list[list->size++] = value;
	}

	template <class T>
	inline void Collection<T>::Remove(const T& value) {
		for (UInt i = 0; i < Size(); i++) {
			if (list->list[i] == value) {
				RemoveAt(i);
				break;
			}
		}
	}

	template <class T>
	inline void Collection<T>::RemoveAll(const T& value) {
		for (UInt i = 0; i < Size(); i++) {
			if (list->list[i] == value) {
				RemoveAt(i);
				i--;
			}
		}
	}

	template <class T>
	inline void Collection<T>::RemoveAt(const UInt index) {
		list->size--;

		if (index < list->size)
			list->list[index] = std::move(list->list[list->size]);
	}

	template <class T>
	inline void Collection<T>::Clear() {
		delete[] list->list;
		list->list = new T[list->capacity];
		list->size = 0;
	}

	template <class T>
	inline Optional<UInt> Collection<T>::Find(const T& value) const {
		for (UInt i = 0; i < Size(); i++)
			if (list->list[i] == value)
				return i;

		return nullptr;
	}

	template <class T>
	inline bool Collection<T>::Contains(const T& value) const {
		for (UInt i = 0; i < Size(); i++) if (list->list[i] == value) return true;
		return false;
	}

	template <class T>
	inline Array<T> Collection<T>::ToArray() const {
		Array<T> arr = Array<T>(Size());

		for (UInt i = 0; i < Size(); i++) {
			arr[i] = (*this)[i];
		}

		return arr;
	}

	template <class T>
	inline Collection<T> Collection<T>::Copy() const {
		Collection<T> collection{Size()};
		collection.list->size = Size();

		T* const last = &collection.list->list[Size()];
		T* source = list->list;

		if (std::is_trivially_copyable<T>::value)
			memmove(collection.list->list, list->list, sizeof(T) * Size());
		else for (T* dest = collection.list->list; dest != last; dest++, source++)
			*dest = *source;

		return collection;
	}

	template <class T>
	inline T& Collection<T>::operator[](const UInt i) {
		return list->list[i];
	}

	template <class T>
	inline const T& Collection<T>::operator[](const UInt i) const {
		return list->list[i];
	}

	template <class T>
	inline void Collection<T>::operator=(const Collection<T>& c) {
		if (this->list == c.list)
			return;

		Cleanup();
		this->list = c.list;
		ref = c.ref;
		
		if (ref != nullptr)
			(*ref)++;
	}

	template <class T>
	inline void Collection<T>::operator=(Collection<T>&& c) noexcept {
		Cleanup();
		this->list = c.list;
		this->ref = c.ref;
		c.list = nullptr;
		c.ref = nullptr;
	}

	template <class T>
	inline bool Collection<T>::operator==(const Collection<T>& c) const {
		return this->list == c.list;
	}

	template <class T>
	inline bool Collection<T>::operator!=(const Collection<T>& c) const {
		return this->list != c.list;
	}

	template <class T>
	inline void Collection<T>::Cleanup() {
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
	inline void Collection<T>::Grow() {
		UInt grow = list->capacity >> 1;

		if (grow < minGrow)
			grow = minGrow;
		else if (grow > maxGrow)
			grow = maxGrow;

		const UInt newCapacity = list->capacity + grow;
		T* const newCollection = new T[newCapacity];

		if (std::is_trivially_copyable<T>::value)
			memcpy(newCollection, list->list, sizeof(T) * Size());
		else for (UInt i = 0; i < list->size; i++)
			newCollection[i] = std::move(list->list[i]);

		delete[] list->list;

		list->list = newCollection;
		list->capacity = newCapacity;
	}

	template <class T>
	inline T* Collection<T>::begin() {
		return list->list;
	}

	template <class T>
	inline const T* Collection<T>::begin() const {
		return list->list;
	}

	template <class T>
	inline T* Collection<T>::end() {
		return list->list + list->size;
	}

	template <class T>
	inline const T* Collection<T>::end() const {
		return list->list + list->size;
	}
}