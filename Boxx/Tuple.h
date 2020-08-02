#pragma once

#include "Types.h"

#include "Error.h"

///N Tuple

namespace Boxx {
	///B Tuple
	/// Class for storing multiple values of different types
	/// This class can handle a maximum of <code>8</code> values
	///M
	template <class ... T>
	class Tuple {
	///M

		///T Values
		/// The values in a tuple can be accesed using the <code>value</code> variables
		/// The number of varibles the tuple has is determined by the number of template arguments
		///S T1 value1
		///S T2 value2
		///S T3 value3
		///S    ...   
	public:
		
		///T Default constructor
		///E TupleError: Thrown if the tuple contains more than <code>8</code> values
		Tuple();

		///T Tuple from values
		///E TupleError: Thrown if the tuple contains more than <code>8</code> values
		Tuple(T ... values);

		~Tuple();
	};

	///B TupleError
	/// Thrown if a tuple contains more than <code>8</code> values
	class TupleError : public Error {
	public:
		TupleError() : Error() {}
		TupleError(const char* const msg) : Error(msg) {}
	};

	template <class ... T>
	inline Tuple<T...>::Tuple() {
		throw TupleError("Too many tuple values");
	}

	template <class ... T>
	inline Tuple<T...>::Tuple(T ... values) {
		throw TupleError("Too many tuple values");
	}

	template <class ... T>
	inline Tuple<T...>::~Tuple() {
		
	}

	template <>
	class Tuple<> {
	public:
		Tuple() {}
		~Tuple() {}
	};

	template <class T1>
	class Tuple<T1> {
		public:
		T1 value1{};
		Tuple() {}
		Tuple(const T1& value1)
			:value1(value1){}
		~Tuple() {}
	};

	template <class T1, class T2>
	class Tuple<T1, T2> {
	public:
		T1 value1{};
		T2 value2{};
		Tuple() {}
		Tuple(const T1& value1, const T2& value2)
			:value1(value1),value2(value2){}
		~Tuple() {}
	};

	template <class T1, class T2, class T3>
	class Tuple<T1, T2, T3> {
	public:
		T1 value1{};
		T2 value2{};
		T3 value3{};
		Tuple() {}
		Tuple(const T1& value1, const T2& value2, const T3& value3)
			:value1(value1),value2(value2),value3(value3){}
		~Tuple() {}
	};

	template <class T1, class T2, class T3, class T4>
	class Tuple<T1, T2, T3, T4> {
	public:
		T1 value1{};
		T2 value2{};
		T3 value3{};
		T4 value4{};
		Tuple() {}
		Tuple(const T1& value1, const T2& value2, const T3& value3, const T4& value4)
			:value1(value1),value2(value2),value3(value3),value4(value4){}
		~Tuple() {}
	};

	template <class T1, class T2, class T3, class T4, class T5>
	class Tuple<T1, T2, T3, T4, T5> {
	public:
		T1 value1{};
		T2 value2{};
		T3 value3{};
		T4 value4{};
		T5 value5{};
		Tuple() {}
		Tuple(const T1& value1, const T2& value2, const T3& value3, const T4& value4, const T5& value5)
			:value1(value1),value2(value2),value3(value3),value4(value4),value5(value5){}
		~Tuple() {}
	};

	template <class T1, class T2, class T3, class T4, class T5, class T6>
	class Tuple<T1, T2, T3, T4, T5, T6> {
	public:
		T1 value1{};
		T2 value2{};
		T3 value3{};
		T4 value4{};
		T5 value5{};
		T6 value6{};
		Tuple() {}
		Tuple(const T1& value1, const T2& value2, const T3& value3, const T4& value4, const T5& value5, const T6& value6)
			:value1(value1),value2(value2),value3(value3),value4(value4),value5(value5),value6(value6){}
		~Tuple() {}
	};

	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
	class Tuple<T1, T2, T3, T4, T5, T6, T7> {
	public:
		T1 value1{};
		T2 value2{};
		T3 value3{};
		T4 value4{};
		T5 value5{};
		T6 value6{};
		T7 value7{};
		Tuple() {}
		Tuple(const T1& value1, const T2& value2, const T3& value3, const T4& value4, const T5& value5, const T6& value6, const T7& value7)
			:value1(value1),value2(value2),value3(value3),value4(value4),value5(value5),value6(value6),value7(value7){}
		~Tuple() {}
	};

	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
	class Tuple<T1, T2, T3, T4, T5, T6, T7, T8> {
	public:
		T1 value1{};
		T2 value2{};
		T3 value3{};
		T4 value4{};
		T5 value5{};
		T6 value6{};
		T7 value7{};
		T8 value8{};
		Tuple() {}
		Tuple(const T1& value1, const T2& value2, const T3& value3, const T4& value4, const T5& value5, const T6& value6, const T7& value7, const T8& value8)
			:value1(value1),value2(value2),value3(value3),value4(value4),value5(value5),value6(value6),value7(value7),value8(value8){}
		~Tuple() {}
	};
}