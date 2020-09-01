#pragma once

#include "Types.h"
#include "Error.h"

///N Either
namespace Boxx {
	///B Either
	/// Stores either a <code>T1</code> or <code>T2</code>
	///M
	template <class T1, class T2>
	class Either {
	///M
	public:
		///H Constructors

		///T Create default either
		/// This will use the type of the first template argument
		Either();

		///T Create from value
		///M
		Either(const T1& value);
		Either(const T2& value);
		///M

		Either(const Either<T1, T2>& value);
		Either(Either<T1, T2>&& value) noexcept;
		~Either();

		///H Methods

		///T Is value
		/// Checks if the value of the either is the left or right template type
		///M
		bool IsLeft() const;
		bool IsRight() const;
		///M

		///T Get value
		/// Gets the left or right value
		///E EitherTypeError: Thrown if the either is of the wrong type
		///M
		T1 GetLeft() const;
		T1& GetLeft();
		T2 GetRight() const;
		T2& GetRight();
		///M

		///H Operators

		///T Assignment
		///M
		void operator=(const T1& value);
		void operator=(const T2& value);
		///M

		void operator=(const Either<T1, T2>& value);
		void operator=(Either<T1, T2>&& value) noexcept;

		///T Conversion to value
		///E EitherTypeError: Thrown if the either is empty
		///M
		explicit operator T1() const;
		explicit operator T1&();
		explicit operator T2() const;
		explicit operator T2&();
		///M

	private:
		bool isLeft;
		T1 left{};
		T2 right{};
	};

	///B EitherTypeError
	/// Used if the wrong type is used
	class EitherTypeError : public Error {
	public:
		EitherTypeError() : Error() {}
		EitherTypeError(const char* const msg) : Error(msg) {}
	};

	template <class T1, class T2>
	inline Either<T1, T2>::Either() {
		isLeft = true;
	}

	template <class T1, class T2>
	inline Either<T1, T2>::Either(const T1& value) {
		isLeft = true;
		left = value;
	}

	template <class T1, class T2>
	inline Either<T1, T2>::Either(const T2& value) {
		isLeft = false;
		right = value;
	}

	template <class T1, class T2>
	inline Either<T1, T2>::Either(const Either<T1, T2>& value) {
		isLeft = value.isLeft;
		
		if (isLeft)
			left = value.left;
		else
			right = value.right;
	}

	template <class T1, class T2>
	inline Either<T1, T2>::Either(Either<T1, T2>&& value) noexcept {
		isLeft = value.isLeft;
		left  = std::move(value.left);
		right = std::move(value.right);
	}

	template <class T1, class T2>
	inline Either<T1, T2>::~Either() {

	}

	template <class T1, class T2>
	inline bool Either<T1, T2>::IsLeft() const {
		return isLeft;
	}

	template <class T1, class T2>
	inline bool Either<T1, T2>::IsRight() const {
		return !isLeft;
	}

	template <class T1, class T2>
	inline T1 Either<T1, T2>::GetLeft() const {
		if (!isLeft) throw EitherTypeError();
		return left;
	}

	template <class T1, class T2>
	inline T1& Either<T1, T2>::GetLeft() {
		if (!isLeft) throw EitherTypeError();
		return left;
	}

	template <class T1, class T2>
	inline T2 Either<T1, T2>::GetRight() const {
		if (isLeft) throw EitherTypeError();
		return right;
	}

	template <class T1, class T2>
	inline T2& Either<T1, T2>::GetRight() {
		if (isLeft) throw EitherTypeError();
		return right;
	}

	template <class T1, class T2>
	inline void Either<T1, T2>::operator=(const T1& value) {
		isLeft = true;
		left = value;
	}

	template <class T1, class T2>
	inline void Either<T1, T2>::operator=(const T2& value) {
		isLeft = false;
		right = value;
	}

	template <class T1, class T2>
	inline void Either<T1, T2>::operator=(const Either<T1, T2>& value) {
		isLeft = value.isLeft;

		if (isLeft)
			left = value.left;
		else
			right = value.right;
	}

	template <class T1, class T2>
	inline void Either<T1, T2>::operator=(Either<T1, T2>&& value) noexcept {
		isLeft = value.isLeft;
		left  = std::move(value.left);
		right = std::move(value.right);
	}

	template <class T1, class T2>
	inline Either<T1, T2>::operator T1() const {
		if (!isLeft) throw EitherTypeError();
		return left;
	}

	template <class T1, class T2>
	inline Either<T1, T2>::operator T1&() {
		if (!isLeft) throw EitherTypeError();
		return left;
	}

	template <class T1, class T2>
	inline Either<T1, T2>::operator T2() const {
		if (isLeft) throw EitherTypeError();
		return right;
	}

	template <class T1, class T2>
	inline Either<T1, T2>::operator T2&() {
		if (isLeft) throw EitherTypeError();
		return right;
	}
}
