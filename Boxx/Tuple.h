#pragma once

#include "Types.h"
#include "String.h"
#include "Error.h"

///[Settings] block: indent

///[Namespace] Boxx
namespace Boxx {
	///[Heading] Template Types

	///[Title] Tuple
	/// Class for storing a few values of different types.
	/// This class can handle a maximum of {8} values.
	///[Block] Tuple
	///M
	template <class ... T>
	class Tuple {
	///M

		///[Title] Values
		/// The values in a tuple can be accesed using the value member variables.
		/// The number of value varibles the tuple has is determined by the number of template arguments.
		///[Code] T1 value1
		///[Code] T2 value2
		///[Code] T3 value3
		///[Code]    ...   
	public:
		
		/// Creates a tuple of default values.
		///[Error] TupleError: Thrown if the tuple contains more than {8} values.
		Tuple();

		/// Creates a tuple from values.
		///[Error] TupleError: Thrown if the tuple contains more than {8} values.
		Tuple(T ... values);

		/// Creates a tuple from values.
		///[Error] TupleError: Thrown if the tuple contains more than {8} values.
		///M
		template <class ... Types>
		static Tuple<Types...> Create(Types ... values);
		///M

		~Tuple();
	};

	///[Heading] Errors

	///[Title] TupleError
	/// Used if a tuple contains more than {8} values.
	///[Block] TupleError: Error
	class TupleError : public Error {
	public:
		TupleError() : Error() {}
		TupleError(const char* const msg) : Error(msg) {}

		virtual String Name() const override {
			return "TupleError";
		}
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

	template <class ... T>
	template <class ... Types>
	inline Tuple<Types...> Tuple<T...>::Create(Types ... values) {
		return Tuple<Types...>(values...);
	}

	template <>
	class Tuple<> {
	public:
		Tuple() {}
		~Tuple() {}

		template <class ... Types>
		static Tuple<Types...> Create(Types ... values) {
			return Tuple<Types...>(values...);
		}
	};

	template <class T1>
	class Tuple<T1> {
		public:
		T1 value1{};
		Tuple() {}
		Tuple(const T1& value1)
			:value1(value1){}
		~Tuple() {}

		template <class ... Types>
		static Tuple<Types...> Create(Types ... values) {
			return Tuple<Types...>(values...);
		}
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

		template <class ... Types>
		static Tuple<Types...> Create(Types ... values) {
			return Tuple<Types...>(values...);
		}
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

		template <class ... Types>
		static Tuple<Types...> Create(Types ... values) {
			return Tuple<Types...>(values...);
		}
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

		template <class ... Types>
		static Tuple<Types...> Create(Types ... values) {
			return Tuple<Types...>(values...);
		}
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

		template <class ... Types>
		static Tuple<Types...> Create(Types ... values) {
			return Tuple<Types...>(values...);
		}
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

		template <class ... Types>
		static Tuple<Types...> Create(Types ... values) {
			return Tuple<Types...>(values...);
		}
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

		template <class ... Types>
		static Tuple<Types...> Create(Types ... values) {
			return Tuple<Types...>(values...);
		}
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

		template <class ... Types>
		static Tuple<Types...> Create(Types ... values) {
			return Tuple<Types...>(values...);
		}
	};
}