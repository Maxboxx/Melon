#pragma once

#include "Types.h"

///N Pair
namespace Boxx {
	///B Pair
	/// Used to store a key-value pair
	///M
	template <class Key, class Value>
	struct Pair {
	///M
		///H Members

		///T Key
		/// The key of the pair
		Key key;

		///T Value
		/// The value of the pair
		Value value;

		///H Constructors

		///T Default pair
		Pair()
		{
			key = Key();
			value = Value();
		}

		///T Key and value
		Pair(const Key& key, const Value& value)
		:key(key),value(value){}

		~Pair() {}
	};
}
