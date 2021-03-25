#pragma once

#include "Types.h"
#include "List.h"
#include "Math.h"
#include "Optional.h"
#include "Collection.h"

///[Setting] block: indent

///[Namespace] Boxx
namespace Boxx {
	///[Heading] Collections

	///[Title] Set
	/// A Set of sorted unique values used for fast search.
	///[para] The values in the set must implement these operator functions to work:
	///[item] {bool operator<(const T) const}
	///[item] {bool operator==(const T) const}
	///[Warning] The values in the set should never be modified in a way that affects the result of the above operator functions.
	///[Block] Set
	///M
	template <class T>
	class Set final {
	///M
	public:
		///[Heading] Constructors

		/// Creates an empty set.
		Set();

		/// Creates a set with a reserved capacity.
		explicit Set(const UInt capacity);

		Set(const Set<T>& set);
		Set(Set<T>&& set) noexcept;
		~Set();

		///[Heading] Methods

		/// Returns the current size of the set.
		UInt Size() const;

		/// Returns the current capacity of the set.
		UInt Capacity() const;

		/// Checks if the set is empty.
		bool IsEmpty() const;

		/// Adds a value to the set.
		/// The value is overwritten if the value already exists in the set.
		void Add(const T& value);

		/// Removes the specified value from the set if it exists.
		void Remove(const T& value);

		/// Checks if the set contains a specific value.
		bool Contains(const T& value) const;

		/// Generates all combinations of the set that contains a specific amount of elements.
		Collection<Set<T>> Combinations(const UInt numElements) const;

		/// Generates all combinations of the set.
		Collection<Set<T>> AllCombinations() const;

		/// Creates a copy of the set.
		Set<T> Copy() const;

		void operator=(const Set<T>& set);
		void operator=(Set<T>&& set) noexcept;

		///[Heading] Static functions

		/// Returns the union of two sets.
		static Set<T> Union(const Set<T>& set1, const Set<T>& set2);

		/// Returns the intersection of two sets.
		static Set<T> Intersection(const Set<T>& set1, const Set<T>& set2);

		/// Returns the difference of two sets.
		static Set<T> Difference(const Set<T>& set1, const Set<T>& set2);

		/// Returns the symmetric difference of two sets.
		static Set<T> SymmetricDifference(const Set<T>& set1, const Set<T>& set2);

		///[Heading] Iterators

		/// Iterates over all values in the set.
		///[Code] for (const T t : set)
		const T* begin() const;
		const T* end() const;

	private:
		List<T> list;

		UInt BinarySearch(const T& value) const;
		void Combinations(Array<bool>& used, UInt index, UInt count, UInt numElements, Collection<Set<T>>& combinations) const;
	};

	template <class T>
	inline Set<T>::Set() {
	
	}

	template <class T>
	inline Set<T>::Set(const UInt size) {
		list = List<T>(size);
	}

	template <class T>
	inline Set<T>::Set(const Set<T>& set) {
		list = set.list;
	}

	template <class T>
	inline Set<T>::Set(Set<T>&& set) noexcept {
		list = std::move(set.list);
	}

	template <class T>
	inline Set<T>::~Set() {
	
	}

	template <class T>
	inline UInt Set<T>::Size() const {
		return list.Size();
	}

	template <class T>
	inline UInt Set<T>::Capacity() const {
		return list.Capacity();
	}

	template <class T>
	inline bool Set<T>::IsEmpty() const {
		return list.IsEmpty();
	}

	template <class T>
	inline void Set<T>::Add(const T& value) {
		int start = 0;
		int end = list.Size();
		int i;

		while (start != end) {
			i = start + (end - start) / 2;

			if (value < list[i]) {
				end = i;
			}
			else if (value == list[i]) {
				list[i] = value;
				return;
			}
			else {
				start = i + 1;
			}
		}

		list.Insert(start, value);
	}

	template <class T>
	inline void Set<T>::Remove(const T& value) {
		const int index = BinarySearch(value);

		if (index >= 0) {
			list.RemoveAt(index);
		}
	}

	template <class T>
	inline bool Set<T>::Contains(const T& value) const {
		return BinarySearch(value) != Math::UIntMax();
	}

	template <class T>
	inline Collection<Set<T>> Set<T>::Combinations(const UInt numElements) const {
		Collection<Set<T>> combinations;
		Array<bool> used = Array<bool>(Size());

		for (UInt i = 0; i < used.Size(); i++)
			used[i] = false;

		Combinations(used, 0, 0, numElements, combinations);

		return combinations;
	}

	template <class T>
	inline Collection<Set<T>> Set<T>::AllCombinations() const {
		Collection<Set<T>> combinations;
		Array<bool> used = Array<bool>(Size());

		for (UInt i = 0; i < used.Size(); i++)
			used[i] = false;

		for (UInt i = 0; i < Size() + 1; i++)
			Combinations(used, 0, 0, i, combinations);

		return combinations;
	}

	template <class T>
	inline void Set<T>::Combinations(Array<bool>& used, UInt index, UInt count, UInt numElements, Collection<Set<T>>& combinations) const {
		if (numElements == 0) {
			combinations.Add(Set<T>());
		}
		else if (count == numElements) {
			Set<T> comb = Set<T>(count);

			for (UInt i = 0; i < Size(); i++) {
				if (used[i]) comb.Add(list[i]);
			}

			combinations.Add(comb);
		}
		else if (numElements >= Size()) {
			Set<T> comb = Set<T>(Size());

			for (const T& t : *this) {
				comb.Add(t);
			}

			combinations.Add(comb);
		}
		else if (index < Size()) {
			used[index] = true;
			Combinations(used, index + 1, count + 1, numElements, combinations);

			used[index] = false;
			Combinations(used, index + 1, count, numElements, combinations);
		}
	}

	template <class T>
	inline Set<T> Set<T>::Copy() const {
		Set<T> set;
		set.list = list.Copy();
		return set;
	}

	template <class T>
	inline void Set<T>::operator=(const Set<T>& set) {
		list = set.list;
	}

	template <class T>
	inline void Set<T>::operator=(Set<T>&& set) noexcept {
		list = std::move(set.list);
	}
	
	template <class T>
	inline Set<T> Set<T>::Union(const Set<T>& set1, const Set<T>& set2) {
		Set<T> set{set1.Size()};

		for (const T& v : set1)
			set.Add(v);
		for (const T& v : set2)
			set.Add(v);

		return set;
	}

	template <class T>
	inline Set<T> Set<T>::Intersection(const Set<T>& set1, const Set<T>& set2) {
		Set<T> set;

		for (const T& v : set1)
			if (set2.Contains(v))
				set.Add(v);

		return set;
	}

	template <class T>
	inline Set<T> Set<T>::Difference(const Set<T>& set1, const Set<T>& set2) {
		Set<T> set;

		for (const T& v : set1)
			if (!set2.Contains(v))
				set.Add(v);

		return set;
	}

	template <class T>
	inline Set<T> Set<T>::SymmetricDifference(const Set<T>& set1, const Set<T>& set2) {
		Set<T> set;

		for (const T& v : set1)
			if (!set2.Contains(v))
				set.Add(v);
		for (const T& v : set2)
			if (!set1.Contains(v))
				set.Add(v);

		return set;
	}

	template <class T>
	inline UInt Set<T>::BinarySearch(const T& value) const {
		UInt start = 0;
		UInt end = list.Size();
		UInt i;

		while (start != end) {
			i = start + (end - start) / 2;

			if (value == list[i]) {
				return i;
			}
			else if (value < list[i]) {
				end = i;
			}
			else {
				start = i + 1;
			}
		}

		return Math::UIntMax();
	}

	template <class T>
	inline const T* Set<T>::begin() const {
		return list.begin();
	}

	template <class T>
	inline const T* Set<T>::end() const {
		return list.end();
	}
}
