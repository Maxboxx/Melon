#pragma once

#include <optional>

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

		///T Value
		/// Gets the value of the optional
		///E OptionalError: Thrown if the optional is empty
		T Value() const;

		///T Value Or Default
		/// Gets the value or the default value if the optional does not have a value
		T ValueOr(const T& defaultValue) const;

		///H Operators

		///T Assignment
		///M
		void operator=(std::nullptr_t);
		void operator=(const T& value);
		///M

		void operator=(const Optional<T>& value);
		void operator=(Optional<T>&& value) noexcept;

		///T Comparison
		/// Compares the optional with a value
		///M
		bool operator==(std::nullptr_t) const;
		bool operator==(const T& value) const;
		bool operator==(const Optional<T>& value) const;
		bool operator!=(std::nullptr_t) const;
		bool operator!=(const T& value) const;
		bool operator!=(const Optional<T>& value) const;
		///M

		///T Not operator
		/// Checks if the optional doeas not have a value
		bool operator!() const;

		///T Conversion to value
		///E OptionalError: Thrown if the optional is empty
		///M
		T operator*() const;
		T& operator*();
		///M

		///T Member access
		/// Used to access members of the value
		///E OptionalError: Thrown if the optional is empty
		///M
		T* operator->();
		const T* operator->() const;
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

		bool Value() const;
		bool ValueOr(const bool defaultValue) const;

		void operator=(std::nullptr_t);
		void operator=(const bool value);

		void operator=(const Optional<bool>& value);
		void operator=(Optional<bool>&& value) noexcept;

		bool operator==(std::nullptr_t) const;
		bool operator==(const bool value) const;
		bool operator==(const Optional<bool>& value) const;
		bool operator!=(std::nullptr_t) const;
		bool operator!=(const bool value) const;
		bool operator!=(const Optional<bool>& value) const;
		
		bool operator!() const;

		bool operator*() const;
		bool& operator*();

		explicit operator bool() const;

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
	inline T Optional<T>::Value() const {
		if (!hasValue) throw OptionalError("Optional is null");
		return value;
	}

	template <class T>
	inline T Optional<T>::ValueOr(const T& defaultValue) const {
		return hasValue ? value : defaultValue;
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
	inline bool Optional<T>::operator==(std::nullptr_t) const {
		return !hasValue;
	}

	template <class T>
	inline bool Optional<T>::operator==(const T& value) const {
		return hasValue && this->value == value;
	}

	template <class T>
	inline bool Optional<T>::operator==(const Optional<T>& value) const {
		if (hasValue == value.hasValue) {
			if (hasValue) {
				return this->value == value.value;
			}

			return true;
		}

		return false;
	}

	template <class T>
	inline bool Optional<T>::operator!=(std::nullptr_t) const {
		return hasValue;
	}

	template <class T>
	inline bool Optional<T>::operator!=(const T& value) const {
		return !hasValue || this->value != value;
	}

	template <class T>
	inline bool Optional<T>::operator!=(const Optional<T>& value) const {
		if (hasValue == value.hasValue) {
			if (hasValue) {
				return this->value != value.value;
			}

			return false;
		}

		return true;
	}

	template <class T>
	inline bool operator==(std::nullptr_t, const Optional<T>& value) {
		return value == nullptr;
	}

	template <class T>
	inline bool operator==(const T& value1, const Optional<T>& value2) {
		return value2 == value1;
	}

	template <class T>
	inline bool operator!=(std::nullptr_t, const Optional<T>& value) {
		return value != nullptr;
	}

	template <class T>
	inline bool operator!=(const T& value1, const Optional<T>& value2) {
		return value2 != value1;
	}

	template <class T>
	inline bool Optional<T>::operator!() const {
		return !hasValue;
	}

	template <class T>
	inline T Optional<T>::operator*() const {
		if (!hasValue) throw OptionalError("Optional is null");
		return value;
	}

	template <class T>
	inline T& Optional<T>::operator*() {
		if (!hasValue) throw OptionalError("Optional is null");
		return value;
	}

	template <class T>
	inline T* Optional<T>::operator->() {
		if (!hasValue) throw OptionalError("Optional is null");
		return &value;
	}

	template <class T>
	inline const T* Optional<T>::operator->() const {
		if (!hasValue) throw OptionalError("Optional is null");
		return &value;
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

	inline bool Optional<bool>::Value() const {
		if (!hasValue) throw OptionalError("Optional is null");
		return value;
	}

	inline bool Optional<bool>::ValueOr(const bool defaultValue) const {
		return hasValue ? value : defaultValue;
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

	inline bool Optional<bool>::operator==(std::nullptr_t) const {
		return !hasValue;
	}

	inline bool Optional<bool>::operator==(const bool value) const {
		return hasValue && this->value == value;
	}

	inline bool Optional<bool>::operator==(const Optional<bool>& value) const {
		if (hasValue == value.hasValue) {
			if (hasValue) {
				return this->value == value.value;
			}

			return true;
		}

		return false;
	}

	inline bool Optional<bool>::operator!=(std::nullptr_t) const {
		return hasValue;
	}

	inline bool Optional<bool>::operator!=(const bool value) const {
		return !hasValue || this->value != value;
	}

	inline bool Optional<bool>::operator!=(const Optional<bool>& value) const {
		if (hasValue == value.hasValue) {
			if (hasValue) {
				return this->value != value.value;
			}

			return false;
		}

		return true;
	}

	inline bool operator==(std::nullptr_t, const Optional<bool>& value) {
		return value == nullptr;
	}

	inline bool operator==(const bool value1, const Optional<bool>& value2) {
		return value2 == value1;
	}

	inline bool operator!=(std::nullptr_t, const Optional<bool>& value) {
		return value != nullptr;
	}

	inline bool operator!=(const bool value1, const Optional<bool>& value2) {
		return value2 != value1;
	}

	inline bool Optional<bool>::operator!() const {
		return !hasValue;
	}

	inline Optional<bool>::operator bool() const {
		return hasValue;
	}

	inline bool Optional<bool>::operator*() const {
		if (!hasValue) throw OptionalError("Optional is null");
		return value;
	}

	inline bool& Optional<bool>::operator*() {
		if (!hasValue) throw OptionalError("Optional is null");
		return value;
	}
}
