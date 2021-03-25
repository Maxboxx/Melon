#pragma once

#include "Types.h"
#include "Error.h"

///[Settings] block: indent

///[Namespace] Boxx
namespace Boxx {
	///[Heading] Template Types

	///[Title] Either
	/// Stores either a {T1} or a {T2}.
	///[Block] Either
	///M
	template <class T1, class T2>
	class Either {
	///M
	public:
		///[Heading] Constructors

		Either();

		/// Create with a {T1} value.
		Either(const T1& value);

		/// Create with a {T2} value.
		Either(const T2& value);

		Either(const Either<T1, T2>& value);
		Either(Either<T1, T2>&& value) noexcept;
		~Either();

		///[Heading] Methods

		/// Checks if the value of the either is the left template type.
		bool IsLeft() const;

		/// Checks if the value of the either is the right template type.
		bool IsRight() const;

		/// Gets the left value.
		///[Error] EitherTypeError: Thrown if the either is of the wrong type.
		///M
		T1 GetLeft() const;
		T1& GetLeft();
		///M

		/// Gets the right value.
		///[Error] EitherTypeError: Thrown if the either is of the wrong type.
		///M
		T2 GetRight() const;
		T2& GetRight();
		///M

		///[Heading] Operators

		/// Assignment of {T1} value.
		/// Changes the either type to left.
		void operator=(const T1& value);

		/// Assignment of {T2} value.
		/// Changes the either type to right.
		void operator=(const T2& value);

		void operator=(const Either<T1, T2>& value);
		void operator=(Either<T1, T2>&& value) noexcept;

		/// Checks if two either values are equal.
		bool operator==(const Either<T1, T2>& value) const;

		/// Checks if two either values are not equal.
		bool operator!=(const Either<T1, T2>& value) const;

		bool operator<(const Either<T1, T2>& value) const;

		/// Gets the left value.
		///[Error] EitherTypeError: Thrown if the either is of the wrong type.
		///M
		explicit operator T1() const;
		explicit operator T1&();
		///M

		/// Gets the right value.
		///[Error] EitherTypeError: Thrown if the either is of the wrong type.
		///M
		explicit operator T2() const;
		explicit operator T2&();
		///M

	private:
		bool isLeft;
		T1 left{};
		T2 right{};
	};

	///[Title] EitherTypeError
	/// Used if the wrong either type is used.
	///[Block] EitherTypeError: Error
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
	inline bool Either<T1, T2>::operator==(const Either<T1, T2>& value) const {
		if (isLeft != value.isLeft) return false;

		if (isLeft)
			return left == value.left;
		else
			return right == value.right;
	}

	template <class T1, class T2>
	inline bool Either<T1, T2>::operator!=(const Either<T1, T2>& value) const {
		if (isLeft != value.isLeft) return true;

		if (isLeft)
			return left != value.left;
		else
			return right != value.right;
	}

	template <class T1, class T2>
	inline bool Either<T1, T2>::operator<(const Either<T1, T2>& value) const {
		if (isLeft != value.isLeft) return value.isLeft;

		if (isLeft)
			return left < value.left;
		else
			return right < value.right;
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
