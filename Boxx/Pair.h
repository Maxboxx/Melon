#pragma once

#include "Types.h"

///[Settings] block: indent

///[Namespace] Boxx
namespace Boxx {
	///[Heading] Template Types

	///[Title] Pair
	/// A key-value pair.
	///[Block] Pair
	///M
	template <class Key, class Value>
	struct Pair {
	///M
		///[Heading] Members

		/// The key of the pair.
		Key key;

		/// The value of the pair.
		Value value;

		///[Heading] Constructors

		/// Default pair.
		Pair()
		{
			key = Key();
			value = Value();
		}

		/// Create from key and value.
		Pair(const Key& key, const Value& value)
		:key(key),value(value){}

		~Pair() {}
	};
}
