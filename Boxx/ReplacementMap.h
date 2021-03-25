#pragma once

#include "Types.h"
#include "Map.h"

///[Settings] block: indent

///[Namespace] Boxx
namespace Boxx {
	///[Heading] Collections

	///[Title] ReplacementMap
	/// A type of map used to store replacements for values.
	///[Block] ReplacementMap
	///M
	template <class T>
	class ReplacementMap final {
	///M
	public:
		ReplacementMap();
		ReplacementMap(const ReplacementMap<T>& map);
		ReplacementMap(ReplacementMap<T>&& map) noexcept;
		~ReplacementMap();

		///[Heading] Methods

		/// Adds a new replacement key to the map.
		///[Error] MapKeyError: Thrown if the key already exists in the map.
		void Add(const T& key, const T& replacement);

		/// Sets the value of a replacement key.
		///[Error] MapKeyError: Thrown if the key does not exist.
		void Set(const T& key, const T& replacement);

		/// Removes a key from the map if it exists.
		void Remove(const T& key);

		/// Checks if the map contains a specific key.
		bool Contains(const T& key) const;

		/// Checks if the map is empty.
		bool IsEmpty() const;

		/// Creates a copy of the map.
		ReplacementMap<T> Copy() const;

		/// Gets the replacement value for a specific key.
		///[para] The key is returned if the key does not exist in the map.
		T GetValue(const T& key) const;

		/// Gets the last value in a replacement chain.
		///W This will cause infinite recursion if the chain is a cycle.
		T GetChain(const T& key) const;

		///[Heading] Operators

		/// Gets the replacement value for a specific key.
		///[Error] MapKeyError: Thrown if the key does not exist.
		T operator[](const T& key) const;

		void operator=(const ReplacementMap<T>& map);
		void operator=(ReplacementMap<T>&& map) noexcept;

		/// Checks if two maps are the same.
		bool operator==(const ReplacementMap<T>& map) const;

		/// Checks if two maps are not the same.
		bool operator!=(const ReplacementMap<T>& map) const;

		///[Heading] Iterators

		/// Iterates over all key-replacement pairs in the map.
		///[Code] for (Pair<T, T> pair : map)
		Pair<T, T>* begin();
		const Pair<T, T>* begin() const;
		Pair<T, T>* end();
		const Pair<T, T>* end() const;

	private:
		Map<T, T> map;
	};

	template <class T>
	inline ReplacementMap<T>::ReplacementMap() {
		
	}

	template <class T>
	inline ReplacementMap<T>::ReplacementMap(const ReplacementMap<T>& map) {
		this->map = map.map;
	}

	template <class T>
	inline ReplacementMap<T>::ReplacementMap(ReplacementMap<T>&& map) noexcept {
		this->map = std::move(map.map);
	}

	template <class T>
	inline ReplacementMap<T>::~ReplacementMap() {
		
	}

	template <class T>
	inline void ReplacementMap<T>::Add(const T& key, const T& value) {
		if (key != value) {
			map.Add(key, value);
		}
	}

	template <class T>
	inline void ReplacementMap<T>::Set(const T& key, const T& value) {
		if (key == value) {
			Remove(key);
		}
		else {
			map[key] = value;
		}
	}

	template <class T>
	inline void ReplacementMap<T>::Remove(const T& key) {
		map.Remove(key);
	}

	template <class T>
	inline bool ReplacementMap<T>::Contains(const T& key) const {
		return map.Contains(key);
	}

	template <class T>
	inline bool ReplacementMap<T>::IsEmpty() const {
		return map.IsEmpty();
	}

	template <class T>
	inline ReplacementMap<T> ReplacementMap<T>::Copy() const {
		ReplacementMap<T> map;
		map.map = this->map.Copy();
		return map;
	}

	template <class T>
	inline T ReplacementMap<T>::GetValue(const T& key) const {
		T value;

		if (map.Contains(key, value)) {
			return value;
		}
		else {
			return key;
		}
	}

	template <class T>
	inline T ReplacementMap<T>::GetChain(const T& key) const {
		T value;

		if (map.Contains(key, value)) {
			return GetChain(value);
		}
		else {
			return key;
		}
	}

	template <class T>
	inline T ReplacementMap<T>::operator[](const T& key) const {
		return map[key];
	}

	template <class T>
	inline void ReplacementMap<T>::operator=(const ReplacementMap<T>& map) {
		this->map = map.map;
	}

	template <class T>
	inline void ReplacementMap<T>::operator=(ReplacementMap<T>&& map) noexcept {
		this->map = std::move(map.map);
	}

	template <class T>
	inline bool ReplacementMap<T>::operator==(const ReplacementMap<T>& map) const {
		return this->map == map.map;
	}

	template <class T>
	inline bool ReplacementMap<T>::operator!=(const ReplacementMap<T>& map) const {
		return this->map != std::move(map.map);
	}

	template <class T>
	inline Pair<T, T>* ReplacementMap<T>::begin() {
		return map.begin();
	}

	template <class T>
	inline const Pair<T, T>* ReplacementMap<T>::begin() const {
		return map.begin();
	}

	template <class T>
	inline Pair<T, T>* ReplacementMap<T>::end() {
		return map.end();
	}

	template <class T>
	inline const Pair<T, T>* ReplacementMap<T>::end() const {
		return map.end();
	}
}
