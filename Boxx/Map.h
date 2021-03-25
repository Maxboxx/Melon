#pragma once

#include "Types.h"
#include "Pair.h"
#include "List.h"
#include "Error.h"
#include "Math.h"
#include "String.h"

///[Settings] block: indent

///[Namespace] Boxx
namespace Boxx {
	///[Heading] Collections

	///[Title] Map
	/// A collection of key-value pairs.
	///[para] The map key type must implement these operator functions to work:
	///[item] {bool operator<(const Key) const}
	///[item] {bool operator==(const Key) const}
	///[Warning] The map keys should never be modified in a way that affects the result of the above operator functions.
	///[Block] Map
	///M
	template <class Key, class Value>
	class Map final {
	///M
	public:
		///[Heading] Constructors

		/// Creates an empty map.
		Map();

		/// Creates a map with a reserved capacity.
		explicit Map(const UInt capacity);

		Map(const Map<Key, Value>& map);
		Map(Map<Key, Value>&& map) noexcept;
		~Map();

		///[Heading] Methods

		/// Adds a new key-value pair to the map.
		///[Error] MapKeyError: Thrown if the key already exists in the map.
		void Add(const Key& key, const Value& value);

		/// Adds a new key-value pair to the map.
		///[Error] MapKeyError: Thrown if the key already exists in the map.
		void Add(const Pair<Key, Value>& pair);

		/// Remove a key from the map if it exists.
		void Remove(const Key& key);

		/// Checks if the map contains a specific key.
		bool Contains(const Key& key) const;

		/// Checks if the map contains a specific key and assigns the value of that key to {value} if it exists.
		bool Contains(const Key& key, Value& value) const;

		/// Returns the current size of the map.
		UInt Size() const;

		/// Returns the current capacity of the map.
		UInt Capacity() const;

		/// Checks if the map is empty.
		bool IsEmpty() const;

		/// Creates a copy of the map.
		Map<Key, Value> Copy() const;

		///[Heading] Operators

		/// Used to get/set the value of a specific key.
		///[Error] MapKeyError: Thrown if the key does not exist.
		///M
		Value& operator[](const Key& key);
		const Value& operator[](const Key& key) const;
		///M

		void operator=(const Map<Key, Value>& map);
		void operator=(Map<Key, Value>&& map) noexcept;

		/// Checks if two maps are the same.
		bool operator==(const Map<Key, Value>& map) const;

		/// Checks if two maps are not the same.
		bool operator!=(const Map<Key, Value>& map) const;

		///[Heading] Iterators

		/// Iterates over all key-value pairs in the map.
		///[Code] for (Pair<Key, Value> pair : map)
		Pair<Key, Value>* begin();
		const Pair<Key, Value>* begin() const;
		Pair<Key, Value>* end();
		const Pair<Key, Value>* end() const;

	private:
		List<Pair<Key, Value>> values;

		UInt BinarySearch(const Key& key) const;
	};

	///[Heading] Errors

	///[Title] MapError
	/// Base class for all map errors.
	///[Block] MapError: Error
	class MapError : public Error {
	public:
		MapError():Error(){}
		MapError(const char* const msg):Error(msg){}

		virtual String Name() const override {
			return "MapError";
		}
	};

	///[Title] MapKeyError
	/// Used for key related errors in a map.
	///[Block] MapKeyError: MapError
	class MapKeyError : public MapError {
	public:
		MapKeyError():MapError(){}
		MapKeyError(const char* const msg):MapError(msg){}

		virtual String Name() const override {
			return "MapKeyError";
		}
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
				throw MapKeyError("key already exists");
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
	inline Map<Key, Value> Map<Key, Value>::Copy() const {
		Map<Key, Value> map;
		map.values = values.Copy();
		return map;
	}

	template <class Key, class Value>
	inline Value& Map<Key, Value>::operator[](const Key& key) {
		const UInt i = BinarySearch(key);
		if (i == Math::UIntMax()) throw MapKeyError("Key not found");
		return values[i].value;
	}

	template <class Key, class Value>
	inline const Value& Map<Key, Value>::operator[](const Key& key) const {
		const UInt i = BinarySearch(key);
		if (i == Math::UIntMax()) throw MapKeyError("Key not found");
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
