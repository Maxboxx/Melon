#pragma once

#include "Types.h"

///[Settings] block: indent

///[Namespace] Boxx
namespace Boxx {
	template <class T>
	class Pointer;

	template <class T>
	class WeakPointer;

	///[Heading] Template Types

	///[Title] Pointer
	/// A smart pointer class to help with memory management.
	/// It automatically calls {delete} on the internal pointer when it is no longer in use.
	///[Warning] Use a {WeakPointer} for circular referencing to avoid memory leaks.
	///[Block] Pointer
	///M
	template <class T>
	class Pointer final {
	///M
	public:
		///[Heading] Constructors

		/// Creates a {nullptr}.
		Pointer();

		/// Keeps track of a pointer and deletes it when it is no longer needed.
		///[Arg] ptr: The pointer to keep track of (must be allocated with {new} or be a {nullptr}).
		Pointer(T* const ptr);

		Pointer(const Pointer<T>& ptr);
		Pointer(Pointer<T>&& ptr) noexcept;

		/// Creates a pointer where template argument {U} is a subclass of {T}.
		///M
		template <class U>
		Pointer(const Pointer<U>& ptr);
		///M

		template <class U>
		Pointer(Pointer<U>&& ptr) noexcept;
		
		~Pointer();

		///[Heading] Methods

		/// Gets the number of references to this pointer.
		UInt References() const;

		/// Cast a pointer to a different type.
		///[para] Returns a {nullptr} if the cast fails.
		///M
		template<class U>
		Pointer<U> Cast() const;
		///M

		/// Checks if the pointer is of a specific type.
		///M
		template<class U>
		bool Is() const;
		///M

		///[Heading] Operators

		/// Gets acces to the members of the internal pointer.
		T* operator->() const;

		/// Dereferences the internal pointer.
		T& operator*() const;

		/// Assigns a new pointer.
		///[para] {ptr} must be allocated with {new} or be a {nullptr}.
		void operator=(T* const ptr);

		void operator=(const Pointer<T>& ptr);
		void operator=(Pointer<T>&& ptr) noexcept;

		/// Assigns a pointer where template argument {U} is a subclass of {T}.
		///M
		template <class U>
		void operator=(const Pointer<U>& ptr);
		///M

		template <class U>
		void operator=(Pointer<U>&& ptr) noexcept;

		/// Checks if two pointers point to the same object.
		bool operator==(T* const ptr) const;

		/// Checks if two pointers do not point to the same object.
		bool operator!=(T* const ptr) const;

		/// Checks if two pointers point to the same object.
		bool operator==(const Pointer<T>& ptr) const;

		/// Checks if two pointers do not point to the same object.
		bool operator!=(const Pointer<T>& ptr) const;

		/// Checks if two pointers point to the same object.
		bool operator==(const WeakPointer<T>& ptr) const;

		/// Checks if two pointers do not point to the same object.
		bool operator!=(const WeakPointer<T>& ptr) const;

		bool operator<(const Pointer<T>& ptr) const;
		bool operator>(const Pointer<T>& ptr) const;
		bool operator<=(const Pointer<T>& ptr) const;
		bool operator>=(const Pointer<T>& ptr) const;
		bool operator<(const WeakPointer<T>& ptr) const;
		bool operator>(const WeakPointer<T>& ptr) const;
		bool operator<=(const WeakPointer<T>& ptr) const;
		bool operator>=(const WeakPointer<T>& ptr) const;

		///[Heading] Conversions

		/// Returns the internal pointer.
		explicit operator T*() const;

		/// {true} if the pointer is not {nullptr}.
		explicit operator bool() const;

	private:
		template <class U>
		friend class Pointer;

		template <class U>
		friend class WeakPointer;

		template <class U>
		Pointer(const Pointer<U>& ptr, bool);

		T* ptr;
		UInt* ref;

		void Cleanup();
	};

	///[Title] WeakPointer
	/// Contains a pointer from a {Pointer} but does not keep track of references or delete memory.
	///W The weak pointer will contain an invalid pointer if all references to the pointer by {Pointer} objects are removed.
	///[Block] WeakPointer
	///M
	template <class T>
	class WeakPointer final {
	///M
	public:
		///[Heading] Constructors

		/// Creates a {nullptr}.
		WeakPointer();

		/// References the pointer stored in {ptr} but does not contribute to the reference count.
		WeakPointer(const Pointer<T>& ptr);

		WeakPointer(const WeakPointer<T>& ptr);
		WeakPointer(WeakPointer<T>&& ptr);
		
		/// Creates a reference to a pointer where template argument {U} is a subclass of {T}.
		///M
		template <class U>
		WeakPointer(const Pointer<U>& ptr);
		///M

		/// Creates a reference to a pointer where template argument {U} is a subclass of {T}.
		///M
		template <class U>
		WeakPointer(const WeakPointer<U>& ptr);
		///M

		template <class U>
		WeakPointer(WeakPointer<U>&& ptr);

		~WeakPointer();

		///[Heading] Methods

		/// Casts the pointer to a different type.
		///[para] Returns a {nullptr} if the cast fails.
		///M
		template<class U>
		WeakPointer<U> Cast() const;
		///M

		/// Checks if the pointer is of a specific type.
		///M
		template<class U>
		bool Is() const;
		///M

		///[Heading] Operators

		/// Gets acces to the members of the internal pointer.
		T* operator->() const;

		/// Dereferences the internal pointer.
		T& operator*() const;

		/// Assigns a new pointer to the weak pointer.
		void operator=(const Pointer<T>& ptr);

		/// Assigns a pointer where template argument {U} is a subclass of {T}.
		///M
		template <class U>
		void operator=(const WeakPointer<U>& ptr);
		///M

		/// Assigns a pointer where template argument {U} is a subclass of {T}.
		///M
		template <class U>
		void operator=(const Pointer<U>& ptr);
		///M

		template <class U>
		void operator=(WeakPointer<U>&& ptr);

		void operator=(const WeakPointer<T>& ptr);
		void operator=(WeakPointer<T>&& ptr);

		/// Checks if the pointers point to the same object.
		bool operator==(T* const ptr) const;

		/// Checks if the pointers do not point to the same object.
		bool operator!=(T* const ptr) const;

		/// Checks if the pointers point to the same object.
		bool operator==(const WeakPointer<T>& ptr) const;

		/// Checks if the pointers do not point to the same object.
		bool operator!=(const WeakPointer<T>& ptr) const;

		/// Checks if the pointers point to the same object.
		bool operator==(const Pointer<T>& ptr) const;

		/// Checks if the pointers do not point to the same object.
		bool operator!=(const Pointer<T>& ptr) const;

		bool operator<(const WeakPointer<T>& ptr) const;
		bool operator>(const WeakPointer<T>& ptr) const;
		bool operator<=(const WeakPointer<T>& ptr) const;
		bool operator>=(const WeakPointer<T>& ptr) const;
		bool operator<(const Pointer<T>& ptr) const;
		bool operator>(const Pointer<T>& ptr) const;
		bool operator<=(const Pointer<T>& ptr) const;
		bool operator>=(const Pointer<T>& ptr) const;

		///[Heading] Conversions

		/// Returns the internal pointer.
		explicit operator T*() const;

		/// {true} if the internal pointer is not {nullptr}.
		explicit operator bool() const;

	private:
		template <class U>
		friend class Pointer;

		template <class U>
		friend class WeakPointer;

		template <class U>
		WeakPointer(const WeakPointer<U>& ptr, bool);

		T* ptr;
	};

	template <class T>
	inline Pointer<T>::Pointer() {
		ptr = nullptr;
		ref = nullptr;
	}

	template <class T>
	inline Pointer<T>::Pointer(T* const ptr) {
		if (ptr != nullptr) {
			this->ptr = ptr;
			ref = new UInt(1);
		}
		else {
			this->ptr = nullptr;
			ref = nullptr;
		}
	}

	template <class T>
	inline Pointer<T>::Pointer(const Pointer<T>& ptr)  {
		this->ptr = ptr.ptr;
		ref = ptr.ref;

		if (ref != nullptr)
			(*ref)++;
	}

	template <class T>
	template <class U>
	inline Pointer<T>::Pointer(const Pointer<U>& ptr)  {
		this->ptr = ptr.ptr;
		ref = ptr.ref;

		if (ref != nullptr)
			(*ref)++;
	}

	template <class T>
	template <class U>
	inline Pointer<T>::Pointer(const Pointer<U>& ptr, bool)  {
		this->ptr = dynamic_cast<T*>(ptr.ptr);

		if (this->ptr != nullptr) {
			ref = ptr.ref;

			if (ref != nullptr)
				(*ref)++;
		}
		else {
			ref = nullptr;
		}
	}

	template <class T>
	inline Pointer<T>::Pointer(Pointer<T>&& ptr) noexcept {
		this->ptr = ptr.ptr;
		ref = ptr.ref;
		ptr.ptr = nullptr;
		ptr.ref = nullptr;
	}

	template <class T>
	template <class U>
	inline Pointer<T>::Pointer(Pointer<U>&& ptr) noexcept {
		this->ptr = ptr.ptr;
		ref = ptr.ref;
		ptr.ptr = nullptr;
		ptr.ref = nullptr;
	}

	template <class T>
	inline Pointer<T>::~Pointer() {
		Cleanup();
	}

	template <class T>
	inline UInt Pointer<T>::References() const {
		return *ref;
	}

	template<class T>
	template<class U>
	Pointer<U> Pointer<T>::Cast() const {
		return Pointer<U>(*this, true);
	}

	template<class T>
	template<class U>
	bool Pointer<T>::Is() const {
		return Pointer<U>(*this, true) != nullptr;
	}

	template <class T>
	inline T* Pointer<T>::operator->() const {
		return ptr;
	}

	template <class T>
	inline T& Pointer<T>::operator*() const { 
		return *ptr;
	}

	template <class T>
	inline void Pointer<T>::operator=(T* const ptr) {
		if (this->ptr != ptr) {
			Cleanup();

			if (ptr != nullptr) {
				this->ptr = ptr;
				ref = new UInt(1);
			}
			else {
				this->ptr = nullptr;
				ref = nullptr;
			}
		}
	}

	template <class T>
	inline void Pointer<T>::operator=(const Pointer<T>& ptr) {
		if (this->ptr != ptr.ptr) {
			Cleanup();

			this->ptr = ptr.ptr;
			ref = ptr.ref;

			if (ref != nullptr)
				(*ref)++;
		}
	}

	template <class T>
	template <class U>
	inline void Pointer<T>::operator=(const Pointer<U>& ptr) {
		if (this->ptr != ptr.ptr) {
			Cleanup();

			this->ptr = ptr.ptr;
			ref = ptr.ref;

			if (ref != nullptr)
				(*ref)++;
		}
	}

	template <class T>
	inline void Pointer<T>::operator=(Pointer<T>&& ptr) noexcept {
		Cleanup();
		this->ptr = ptr.ptr;
		ref = ptr.ref;
		ptr.ptr = nullptr;
		ptr.ref = nullptr;
	}

	template <class T>
	template <class U>
	inline void Pointer<T>::operator=(Pointer<U>&& ptr) noexcept {
		Cleanup();
		this->ptr = ptr.ptr;
		ref = ptr.ref;
		ptr.ptr = nullptr;
		ptr.ref = nullptr;
	}

	template <class T>
	inline bool Pointer<T>::operator==(T* const ptr) const {
		return this->ptr == ptr;
	}

	template <class T>
	inline bool Pointer<T>::operator!=(T* const ptr) const {
		return this->ptr != ptr;
	}

	template <class T>
	inline bool Pointer<T>::operator==(const Pointer<T>& ptr) const {
		return this->ptr == ptr.ptr;
	}

	template <class T>
	inline bool Pointer<T>::operator!=(const Pointer<T>& ptr) const {
		return this->ptr != ptr.ptr;
	}

	template <class T>
	inline bool Pointer<T>::operator<(const Pointer<T>& ptr) const {
		return this->ptr < ptr.ptr;
	}

	template <class T>
	inline bool Pointer<T>::operator>(const Pointer<T>& ptr) const {
		return this->ptr > ptr.ptr;
	}

	template <class T>
	inline bool Pointer<T>::operator<=(const Pointer<T>& ptr) const {
		return this->ptr <= ptr.ptr;
	}

	template <class T>
	inline bool Pointer<T>::operator>=(const Pointer<T>& ptr) const {
		return this->ptr >= ptr.ptr;
	}

	template <class T>
	inline bool Pointer<T>::operator==(const WeakPointer<T>& ptr) const {
		return this->ptr == ptr.ptr;
	}

	template <class T>
	inline bool Pointer<T>::operator!=(const WeakPointer<T>& ptr) const {
		return this->ptr != ptr.ptr;
	}

	template <class T>
	inline bool Pointer<T>::operator<(const WeakPointer<T>& ptr) const {
		return this->ptr < ptr.ptr;
	}

	template <class T>
	inline bool Pointer<T>::operator>(const WeakPointer<T>& ptr) const {
		return this->ptr > ptr.ptr;
	}

	template <class T>
	inline bool Pointer<T>::operator<=(const WeakPointer<T>& ptr) const {
		return this->ptr <= ptr.ptr;
	}

	template <class T>
	inline bool Pointer<T>::operator>=(const WeakPointer<T>& ptr) const {
		return this->ptr >= ptr.ptr;
	}

	template <class T>
	inline Pointer<T>::operator T*() const {
		return ptr;
	}

	template <class T>
	inline Pointer<T>::operator bool() const {
		return ptr != nullptr;
	}

	template <class T>
	inline void Pointer<T>::Cleanup() {
		if (ref != nullptr && --(*ref) < 1) {
			if (ptr != nullptr && *ref == 0) {
				delete ptr;
				delete ref;
			}
			
			ptr = nullptr;
			ref = nullptr;
		}
	}

	template <class T>
	inline bool operator==(T* const ptr1, const Pointer<T>& ptr2) {
		return ptr2 == ptr1;
	}

	template <class T>
	inline bool operator!=(T* const ptr1, const Pointer<T>& ptr2) {
		return ptr2 != ptr1;
	}

	template <class T>
	inline WeakPointer<T>::WeakPointer() {
		ptr = nullptr;
	}

	template <class T>
	inline WeakPointer<T>::WeakPointer(const Pointer<T>& ptr)  {
		this->ptr = ptr.ptr;
	}

	template <class T>
	inline WeakPointer<T>::WeakPointer(const WeakPointer<T>& ptr)  {
		this->ptr = ptr.ptr;
	}

	template <class T>
	template <class U>
	inline WeakPointer<T>::WeakPointer(const Pointer<U>& ptr)  {
		this->ptr = ptr.ptr;
	}

	template <class T>
	template <class U>
	inline WeakPointer<T>::WeakPointer(const WeakPointer<U>& ptr)  {
		this->ptr = ptr.ptr;
	}

	template <class T>
	template <class U>
	inline WeakPointer<T>::WeakPointer(const WeakPointer<U>& ptr, bool)  {
		this->ptr = dynamic_cast<T*>(ptr.ptr);
	}

	template <class T>
	inline WeakPointer<T>::WeakPointer(WeakPointer<T>&& ptr)  {
		this->ptr = ptr.ptr;
		ptr.ptr = nullptr;
	}

	template <class T>
	template <class U>
	inline WeakPointer<T>::WeakPointer(WeakPointer<U>&& ptr)  {
		this->ptr = ptr.ptr;
		ptr.ptr = nullptr;
	}

	template <class T>
	inline WeakPointer<T>::~WeakPointer() {
		
	}

	template<class T>
	template<class U>
	WeakPointer<U> WeakPointer<T>::Cast() const {
		return WeakPointer<U>(*this, true);
	}

	template<class T>
	template<class U>
	bool WeakPointer<T>::Is() const {
		return WeakPointer<U>(*this, true) != nullptr;
	}

	template <class T>
	inline T* WeakPointer<T>::operator->() const {
		return ptr;
	}

	template <class T>
	inline T& WeakPointer<T>::operator*() const { 
		return *ptr;
	}

	template <class T>
	inline void WeakPointer<T>::operator=(const WeakPointer<T>& ptr) {
		this->ptr = ptr.ptr;
	}

	template <class T>
	inline void WeakPointer<T>::operator=(WeakPointer<T>&& ptr) {
		this->ptr = ptr.ptr;
		ptr.ptr = nullptr;
	}

	template <class T>
	inline void WeakPointer<T>::operator=(const Pointer<T>& ptr) {
		this->ptr = ptr.ptr;
	}

	template <class T>
	template <class U>
	inline void WeakPointer<T>::operator=(const WeakPointer<U>& ptr) {
		this->ptr = ptr.ptr;
	}

	template <class T>
	template <class U>
	inline void WeakPointer<T>::operator=(WeakPointer<U>&& ptr) {
		this->ptr = ptr.ptr;
		ptr.ptr = nullptr;
	}

	template <class T>
	template <class U>
	inline void WeakPointer<T>::operator=(const Pointer<U>& ptr) {
		this->ptr = ptr.ptr;
	}

	template <class T>
	inline bool WeakPointer<T>::operator==(T* const ptr) const {
		return this->ptr == ptr;
	}

	template <class T>
	inline bool WeakPointer<T>::operator!=(T* const ptr) const {
		return this->ptr != ptr;
	}

	template <class T>
	inline bool WeakPointer<T>::operator==(const WeakPointer<T>& ptr) const {
		return this->ptr == ptr.ptr;
	}

	template <class T>
	inline bool WeakPointer<T>::operator!=(const WeakPointer<T>& ptr) const {
		return this->ptr != ptr.ptr;
	}

	template <class T>
	inline bool WeakPointer<T>::operator<(const WeakPointer<T>& ptr) const {
		return this->ptr < ptr.ptr;
	}

	template <class T>
	inline bool WeakPointer<T>::operator>(const WeakPointer<T>& ptr) const {
		return this->ptr > ptr.ptr;
	}

	template <class T>
	inline bool WeakPointer<T>::operator<=(const WeakPointer<T>& ptr) const {
		return this->ptr <= ptr.ptr;
	}

	template <class T>
	inline bool WeakPointer<T>::operator>=(const WeakPointer<T>& ptr) const {
		return this->ptr >= ptr.ptr;
	}

	template <class T>
	inline bool WeakPointer<T>::operator==(const Pointer<T>& ptr) const {
		return this->ptr == ptr.ptr;
	}

	template <class T>
	inline bool WeakPointer<T>::operator!=(const Pointer<T>& ptr) const {
		return this->ptr != ptr.ptr;
	}

	template <class T>
	inline bool WeakPointer<T>::operator<(const Pointer<T>& ptr) const {
		return this->ptr < ptr.ptr;
	}

	template <class T>
	inline bool WeakPointer<T>::operator>(const Pointer<T>& ptr) const {
		return this->ptr > ptr.ptr;
	}

	template <class T>
	inline bool WeakPointer<T>::operator<=(const Pointer<T>& ptr) const {
		return this->ptr <= ptr.ptr;
	}

	template <class T>
	inline bool WeakPointer<T>::operator>=(const Pointer<T>& ptr) const {
		return this->ptr >= ptr.ptr;
	}

	template <class T>
	inline WeakPointer<T>::operator T*() const {
		return ptr;
	}

	template <class T>
	inline WeakPointer<T>::operator bool() const {
		return ptr != nullptr;
	}

	template <class T>
	inline bool operator==(T* const ptr1, const WeakPointer<T>& ptr2) {
		return ptr2 == ptr1;
	}

	template <class T>
	inline bool operator!=(T* const ptr1, const WeakPointer<T>& ptr2) {
		return ptr2 != ptr1;
	}
}
