#pragma once

#include "Types.h"
#include "Error.h"

///N Optional
namespace Boxx {
	///B Optional
	/// Stores an optional value
	/// The optional value is either a <code>T</code> value or no value
	/// <code>nullptr</code> is used to represent an empty optional
	///M
	template <class T>
	class Optional {
	///M
	public:
		///H Constructors

		///T Create an empty optional
		///M
		Optional();
		Optional(std::nullptr_t);
		///M

		///T Create from value
		Optional(const T& value);

		Optional(const Optional<T>& value);
		Optional(Optional<T>&& value) noexcept;
		~Optional();

		///H Methods

		///T Has value
		/// Checks if the optional contains a value
		bool HasValue() const;

		///T Get value
		/// Gets the value of the optional
		///E OptionalError: Thrown if the optional is empty
		///M
		T Get() const;
		T& Get();
		///M

		///H Operators

		///T Assignment
		///M
		void operator=(std::nullptr_t);
		void operator=(const T& value);
		///M

		void operator=(const Optional<T>& value);
		void operator=(Optional<T>&& value) noexcept;

		///T Conversion to value
		///E OptionalError: Thrown if the optional is empty
		///M
		operator T() const;
		operator T&();
		///M

		///T Conversion to bool
		///R bool: <code>true</code> if the optional has a value. <code>false</code> otherwise
		///W The <code>T</code> conversion operator is used instead of this in some cases if <code>T</code> is an integer type
		///W Use <code>HasValue()</code> to avoid this problem
		explicit operator bool() const;

	private:
		bool hasValue;
		T value{};
	};

	///B OptionalError
	/// Used if the optional does not contain a value
	class OptionalError : public Error {
	public:
		OptionalError() : Error() {}
		OptionalError(const char* const msg) : Error(msg) {}
	};

	template <class T>
	inline Optional<T>::Optional() {
		hasValue = false;
	}

	template <class T>
	inline Optional<T>::Optional(std::nullptr_t) {
		hasValue = false;
	}

	template <class T>
	inline Optional<T>::Optional(const T& value) {
		hasValue = true;
		this->value = value;
	}

	template <class T>
	inline Optional<T>::Optional(const Optional<T>& value) {
		hasValue = value.hasValue;
		this->value = value.value;
	}

	template <class T>
	inline Optional<T>::Optional(Optional<T>&& value) noexcept {
		hasValue = value.hasValue;
		this->value = std::move(value.value);
		value.hasValue = false; 
	}

	template <class T>
	inline Optional<T>::~Optional() {
		
	}

	template <class T>
	inline bool Optional<T>::HasValue() const {
		return hasValue;
	}

	template <class T>
	inline T Optional<T>::Get() const {
		if (!hasValue) throw OptionalError("Optional is null");
		return value;
	}

	template <class T>
	inline T& Optional<T>::Get() {
		if (!hasValue) throw OptionalError("Optional is null");
		return value;
	}

	template <class T>
	inline void Optional<T>::operator=(std::nullptr_t) {
		hasValue = false;
	}

	template <class T>
	inline void Optional<T>::operator=(const T& value) {
		hasValue = true;
		this->value = value;
	}

	template <class T>
	inline void Optional<T>::operator=(const Optional<T>& value) {
		hasValue = value.hasValue;
		this->value = value.value;
	}

	template <class T>
	inline void Optional<T>::operator=(Optional<T>&& value) noexcept {
		hasValue = value.hasValue;
		this->value = std::move(value.value);
		value.hasValue = false; 
	}

	template <class T>
	inline Optional<T>::operator T() const {
		if (!hasValue) throw OptionalError("Optional is null");
		return value;
	}

	template <class T>
	inline Optional<T>::operator T&() {
		if (!hasValue) throw OptionalError("Optional is null");
		return value;
	}

	template <class T>
	inline Optional<T>::operator bool() const {
		return hasValue;
	}
}
