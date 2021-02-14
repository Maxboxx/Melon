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
		explicit operator T() const;
		explicit operator T&();
		///M

		///T Conversion to bool
		///R bool: <code>true</code> if the optional has a value. <code>false</code> otherwise
		explicit operator bool() const;

	private:
		bool hasValue;
		T value{};
	};

	template <>
	class Optional<bool> {
	public:
		Optional();
		Optional(std::nullptr_t);
		Optional(const bool value);

		Optional(const Optional<bool>& value);
		Optional(Optional<bool>&& value) noexcept;
		~Optional();

		bool HasValue() const;

		bool Get() const;
		bool& Get();

		void operator=(std::nullptr_t);
		void operator=(const bool value);

		void operator=(const Optional<bool>& value);
		void operator=(Optional<bool>&& value) noexcept;

		explicit operator bool() const;
		explicit operator bool&();

	private:
		bool hasValue;
		bool value{};
	};

	///B OptionalError
	/// Used if the optional does not contain a value
	class OptionalError : public Error {
	public:
		OptionalError() : Error() {}
		OptionalError(const char* const msg) : Error(msg) {}

		virtual String Name() const override;
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

	inline Optional<bool>::Optional() {
		hasValue = false;
	}

	inline Optional<bool>::Optional(std::nullptr_t) {
		hasValue = false;
	}

	inline Optional<bool>::Optional(const bool value) {
		hasValue = true;
		this->value = value;
	}

	inline Optional<bool>::Optional(const Optional<bool>& value) {
		hasValue = value.hasValue;
		this->value = value.value;
	}

	inline Optional<bool>::Optional(Optional<bool>&& value) noexcept {
		hasValue = value.hasValue;
		this->value = std::move(value.value);
		value.hasValue = false; 
	}

	inline Optional<bool>::~Optional() {

	}

	inline bool Optional<bool>::HasValue() const {
		return hasValue;
	}

	inline bool Optional<bool>::Get() const {
		if (!hasValue) throw OptionalError("Optional is null");
		return value;
	}

	inline bool& Optional<bool>::Get() {
		if (!hasValue) throw OptionalError("Optional is null");
		return value;
	}

	inline void Optional<bool>::operator=(std::nullptr_t) {
		hasValue = false;
	}

	inline void Optional<bool>::operator=(const bool value) {
		hasValue = true;
		this->value = value;
	}

	inline void Optional<bool>::operator=(const Optional<bool>& value) {
		hasValue = value.hasValue;
		this->value = value.value;
	}

	inline void Optional<bool>::operator=(Optional<bool>&& value) noexcept {
		hasValue = value.hasValue;
		this->value = std::move(value.value);
		value.hasValue = false; 
	}

	inline Optional<bool>::operator bool() const {
		if (!hasValue) throw OptionalError("Optional is null");
		return value;
	}

	inline Optional<bool>::operator bool&() {
		if (!hasValue) throw OptionalError("Optional is null");
		return value;
	}
}
