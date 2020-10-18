#pragma once

#include "Types.h"
#include "Map.h"

///N ReplacementMap

namespace Boxx {
	///B ReplacementMap
	/// A type of map used to store replacements for values
	///M
	template <class T>
	class ReplacementMap {
	///M
	public:
		///H Constructors

		///T Constructor
		ReplacementMap();
		ReplacementMap(const ReplacementMap<T>& map);
		ReplacementMap(ReplacementMap<T>&& map) noexcept;
		~ReplacementMap();

		///H Methods

		///T Add Replacement
		/// Adds a new replacement key to the map
		///A const T& key: The value to replace with a new value
		///A const T& replacement: The replacement value
		///E MapKeyError: Thrown if the key already exists in the map
		void Add(const T& key, const T& replacement);

		///T Remove key
		/// Remove a key from the map if it exists
		void Remove(const T& key);

		///T Contains key
		/// Checks if the map contains a specific key
		bool Contains(const T& key) const;

		///T Is Empty
		/// Checks if the map is empty
		bool IsEmpty() const;

		///T Copy
		/// Creates a copy of the map
		ReplacementMap<T> Copy() const;

		///T GetValue
		/// Gets the replacement value for a specific key
		///R T value: The replacement value or the value of <code>key</code> if the key was not found
		T GetValue(const T& key) const;

		///T GetChain
		/// Gets the last replacement value in an entire replacement chain
		///R T value: The last value of the replacement chain or the value of <code>key</code> if the key was not found
		///W This will cause infinite recursion if the chain is a cycle
		T GetChain(const T& key) const;

		///H Operators

		///T Index
		/// Gets/sets the replacement value for a specific key
		///E MapKeyError: Thrown if the key does not exist
		///M
		T operator[](const T& key) const;
		T& operator[](const T& key);
		///M

		void operator=(const ReplacementMap<T>& map);
		void operator=(ReplacementMap<T>&& map) noexcept;

		///T Equality operators
		///M
		bool operator==(const ReplacementMap<T>& map) const;
		bool operator!=(const ReplacementMap<T>& map) const;
		///M

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
		map.Add(key, value);
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
	inline T& ReplacementMap<T>::operator[](const T& key) {
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
}