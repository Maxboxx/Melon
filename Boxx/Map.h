#pragma once

#include "Types.h"
#include "Pair.h"
#include "List.h"
#include "Error.h"
#include "Math.h"

///N Map
namespace Boxx {
	///B Map
	/// A map is a collection of key-value pairs
	/// The map key type must implement these operator functions to work:
	/// <code>bool operator&lt;(const Key) const</code>
	/// <code>bool operator==(const Key) const</code>
	///W The map keys should never be modified in a way that affects the result of the above operator functions
	///M
	template <class Key, class Value>
	class Map {
	///M
	public:
		///H Constructors

		///T Empty map
		/// Creates an empty map
		Map();

		///T Reserved capacity
		/// Creates a map with a reserved capacity
		explicit Map(const UInt capacity);

		Map(const Map<Key, Value>& map);
		Map(Map<Key, Value>&& map) noexcept;
		~Map();

		///H Methods

		///T Add key-value pair
		/// Adds a new key-value pair to the map
		///E MapKeyError: Thrown if the key already exists in the map
		///M
		void Add(const Key& key, const Value& value);
		void Add(const Pair<Key, Value>& pair);
		///M

		///T Remove key
		/// Remove a key from the map if it exists
		void Remove(const Key& key);

		///T Contains key
		/// Checks if the map contains a specific key
		///A Value& value: An output argument containing the value associated with the key
		///M
		bool Contains(const Key& key) const;
		bool Contains(const Key& key, Value& value) const;
		///M

		///T Size
		/// Returns the current size of the map
		UInt Size() const;

		///T Capacity
		/// Returns the current capacity of the map
		UInt Capacity() const;

		///T Is Empty
		/// Checks if the map is empty
		bool IsEmpty() const;

		///H Operators

		///T Indexing
		/// Used to get/set the value at a specific key
		///E MapKeyError: Thrown if the key does not exist
		///M
		Value& operator[](const Key& key);
		const Value& operator[](const Key& key) const;
		///M

		void operator=(const Map<Key, Value>& map);
		void operator=(Map<Key, Value>&& map) noexcept;

		///T Equality
		/// Checks if two maps are the same
		///M
		bool operator==(const Map<Key, Value>& map) const;
		bool operator!=(const Map<Key, Value>& map) const;
		///M

		///H Iterators

		///T Iterator
		/// Iterates over all key-value pairs in the map
		///S for (Pair<Key, Value> pair : collection)
		Pair<Key, Value>* begin();
		const Pair<Key, Value>* begin() const;
		Pair<Key, Value>* end();
		const Pair<Key, Value>* end() const;

	private:
		List<Pair<Key, Value>> values;

		UInt BinarySearch(const Key& key) const;
	};

	///B MapError
	/// Base class for all map errors
	class MapError : public Error {
	public:
		MapError():Error(){}
		MapError(const char* const msg):Error(msg){}
	};

	///B MapKeyError
	/// Used for key related errors in a map
	class MapKeyError : public MapError {
	public:
		MapKeyError():MapError(){}
		MapKeyError(const char* const msg):MapError(msg){}
	};

	template <class Key, class Value>
	inline Map<Key, Value>::Map() {}

	template <class Key, class Value>
	inline Map<Key, Value>::Map(const UInt capacity) {
		values = List<Pair<Key, Value>>(capacity);
	}

	template <class Key, class Value>
	inline Map<Key, Value>::~Map() {}

	template <class Key, class Value>
	inline Map<Key, Value>::Map(const Map<Key, Value>& map) {
		values = map.values;
	}

	template <class Key, class Value>
	inline Map<Key, Value>::Map(Map<Key, Value>&& map) noexcept {
		values = std::move(map.values);
	}

	template <class Key, class Value>
	inline void Map<Key, Value>::Add(const Key& key, const Value& value) {
		Add(Pair<Key, Value>(key, value));
	}

	template <class Key, class Value>
	inline void Map<Key, Value>::Add(const Pair<Key, Value>& pair) {
		int start = 0;
		int end = values.Size();
		int i;

		while (start != end) {
			i = start + (end - start) / 2;

			if (pair.key < values[i].key) {
				end = i;
			}
			else if (pair.key == values[i].key) {
				throw MapKeyError();
			}
			else {
				start = i + 1;
			}
		}

		values.Insert(start, pair);
	}

	template <class Key, class Value>
	inline void Map<Key, Value>::Remove(const Key& key) {
		const int index = BinarySearch(key);

		if (index >= 0) {
			values.RemoveAt(index);
		}
	}

	template <class Key, class Value>
	inline bool Map<Key, Value>::Contains(const Key& key) const {
		return BinarySearch(key) != Math::UIntMax();
	}

	template <class Key, class Value>
	inline bool Map<Key, Value>::Contains(const Key& key, Value& value) const {
		const UInt i = BinarySearch(key);
		if (i == Math::UIntMax()) return false;

		value = values[i].value;
		return true;
	}

	template <class Key, class Value>
	inline UInt Map<Key, Value>::Size() const {
		return values.Size();
	}

	template <class Key, class Value>
	inline UInt Map<Key, Value>::Capacity() const {
		return values.Capacity();
	}

	template <class Key, class Value>
	inline bool Map<Key, Value>::IsEmpty() const {
		return values.IsEmpty();
	}

	template <class Key, class Value>
	inline Value& Map<Key, Value>::operator[](const Key& key) {
		const UInt i = BinarySearch(key);
		if (i == Math::UIntMax()) throw MapKeyError();
		return values[i].value;
	}

	template <class Key, class Value>
	inline const Value& Map<Key, Value>::operator[](const Key& key) const {
		const UInt i = BinarySearch(key);
		if (i == Math::UIntMax()) throw MapKeyError();
		return values[i].value;
	}

	template <class Key, class Value>
	inline void Map<Key, Value>::operator=(const Map<Key, Value>& map) {
		values = map.values;
	}

	template <class Key, class Value>
	inline void Map<Key, Value>::operator=(Map<Key, Value>&& map) noexcept {
		values = std::move(map.values);
	}

	template <class Key, class Value>
	inline bool Map<Key, Value>::operator==(const Map<Key, Value>& map) const {
		return values == map.values;
	}

	template <class Key, class Value>
	inline bool Map<Key, Value>::operator!=(const Map<Key, Value>& map) const {
		return values != map.values;
	}

	template <class Key, class Value>
	inline UInt Map<Key, Value>::BinarySearch(const Key& key) const {
		int start = 0;
		int end = values.Size();
		int i;

		while (start != end) {
			i = start + (end - start) / 2;

			if (key == values[i].key) {
				return i;
			}
			else if (key < values[i].key) {
				end = i;
			}
			else {
				start = i + 1;
			}
		}

		return Math::UIntMax();
	}

	template <class Key, class Value>
	inline Pair<Key, Value>* Map<Key, Value>::begin() {
		return values.begin();
	}

	template <class Key, class Value>
	inline const Pair<Key, Value>* Map<Key, Value>::begin() const {
		return values.begin();
	}

	template <class Key, class Value>
	inline Pair<Key, Value>* Map<Key, Value>::end() {
		return values.end();
	}

	template <class Key, class Value>
	inline const Pair<Key, Value>* Map<Key, Value>::end() const {
		return values.end();
	}
}
