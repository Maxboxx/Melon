#pragma once

#include "Types.h"

///N Pointer
namespace Boxx {
	template <class T>
	class Pointer;

	template <class T>
	class WeakPointer;

	///B Pointer
	/// A smart pointer class to help with memory management
	/// It automatically calls <code>delete</code> on the internal pointer when it is no longer in use
	///W Use <code>WeakPointer</code> for circular referencing to avoid memory leaks
	/// Never assign a <code>WeakPointer</code> to a <code>Pointer</code>
	///M
	template <class T>
	class Pointer {
	///M
	public:
		///H Constructors

		///T Nullptr
		/// Creates a <code>nullptr</code>
		Pointer();

		///T Pointer
		/// Keeps track of a pointer and deletes it when it is no longer needed
		///A T* const ptr: The pointer to keep track of (must be allocated with <code>new</code> or be a <code>nullptr</code>)
		Pointer(T* const ptr);

		Pointer(const Pointer<T>& ptr);
		Pointer(Pointer<T>&& ptr) noexcept;

		///T Subclass constructor
		/// Creates a pointer where template argument <code>U</code> is a subclass of <code>T</code>
		///M
		template <class U>
		Pointer(const Pointer<U>& ptr);
		template <class U>
		Pointer(Pointer<U>&& ptr) noexcept;
		///M

		
		~Pointer();

		///H Methods

		///T References
		/// Gets the number of references to this pointer
		UInt References() const;

		///T Cast
		/// Cast a pointer to a different type
		/// Returns a <code>nullptr</code> if the cast fails
		///M
		template<class U>
		Pointer<U> Cast() const;
		///M

		///T Is
		/// Checks if the pointer is of a specific type
		///M
		template<class U>
		bool Is() const;
		///M

		///H Operators

		///T Structure dereference
		/// Gets acces to the members of the stored pointer
		T* operator->() const;

		///T Dereference
		/// Dereferences the stored pointer
		T& operator*() const;

		///T Assignment
		/// <code>ptr</code> must be allocated with <code>new</code> or be a <code>nullptr</code>
		void operator=(T* const ptr);

		void operator=(const Pointer<T>& ptr);
		void operator=(Pointer<T>&& ptr) noexcept;

		///T Subclass assignment
		/// Assigns a pointer where template argument <code>U</code> is a subclass of <code>T</code>
		///M
		template <class U>
		void operator=(const Pointer<U>& ptr);
		template <class U>
		void operator=(Pointer<U>&& ptr) noexcept;
		///M

		///T Equality operators
		/// Checks if two pointers point to the same object
		///M
		bool operator==(T* const ptr) const;
		bool operator!=(T* const ptr) const;
		bool operator==(const Pointer<T>& ptr) const;
		bool operator!=(const Pointer<T>& ptr) const;
		bool operator==(const WeakPointer<T>& ptr) const;
		bool operator!=(const WeakPointer<T>& ptr) const;
		///M

		///T Comparison operators
		/// Compares the stored pointer with another pointer
		///M
		bool operator<(const Pointer<T>& ptr) const;
		bool operator>(const Pointer<T>& ptr) const;
		bool operator<=(const Pointer<T>& ptr) const;
		bool operator>=(const Pointer<T>& ptr) const;
		bool operator<(const WeakPointer<T>& ptr) const;
		bool operator>(const WeakPointer<T>& ptr) const;
		bool operator<=(const WeakPointer<T>& ptr) const;
		bool operator>=(const WeakPointer<T>& ptr) const;
		///M

		///H Conversions

		///T Conversion to pointer
		/// Conversion to the stored pointer
		explicit operator T*() const;

		///T bool conversion
		///R bool: <code>true</code> if the pointer is not a <code>nullptr</code>
		/// <code>false</code> otherwise
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

	///B WeakPointer
	/// Contains a pointer from a <code>Pointer</code> but does not keep track of references or delete memory.
	///W The weak pointer will contain an invalid pointer if all references to the pointer by <code>Pointer</code> objects are removed
	/// Never assign a <code>WeakPointer</code> to a <code>Pointer</code>
	///M
	template <class T>
	class WeakPointer {
	///M
	public:
		///H Constructors

		///T Null WeakPointer
		/// Creates a <code>nullptr</code>
		WeakPointer();

		///T Pointer
		/// References the pointer stored in <code>ptr</code> but does not contribute to the reference count
		WeakPointer(const Pointer<T>& ptr);

		WeakPointer(const WeakPointer<T>& ptr);
		WeakPointer(WeakPointer<T>&& ptr);
		

		///T Subclass constructor
		/// Creates a reference to a pointer where template argument <code>U</code> is a subclass of <code>T</code>
		///M
		template <class U>
		WeakPointer(const Pointer<U>& ptr);
		template <class U>
		WeakPointer(const WeakPointer<U>& ptr);
		template <class U>
		WeakPointer(WeakPointer<U>&& ptr);
		///M

		~WeakPointer();

		///H Methods

		///T Cast
		/// Casts a pointer to a different type
		/// Returns a <code>nullptr</code> if the cast fails
		///M
		template<class U>
		WeakPointer<U> Cast() const;
		///M

		///T Is
		/// Checks if the pointer is of a specific type
		///M
		template<class U>
		bool Is() const;
		///M

		///H Operators

		///T Structure dereference
		/// Gets acces to the stored pointer
		T* operator->() const;

		///T Dereference
		/// Dereferences the stored pointer
		T& operator*() const;

		///T Assignment
		///M
		void operator=(const Pointer<T>& ptr);
		///M

		///T Subclass assignment
		/// Assigns a pointer where template argument <code>U</code> is a subclass of <code>T</code>
		///M
		template <class U>
		void operator=(const WeakPointer<U>& ptr);
		template <class U>
		void operator=(const Pointer<U>& ptr);
		template <class U>
		void operator=(WeakPointer<U>&& ptr);
		///M

		void operator=(const WeakPointer<T>& ptr);
		void operator=(WeakPointer<T>&& ptr);

		///T Equality operators
		/// Checks if the pointers point to the same object
		///M
		bool operator==(T* const ptr) const;
		bool operator!=(T* const ptr) const;
		bool operator==(const WeakPointer<T>& ptr) const;
		bool operator!=(const WeakPointer<T>& ptr) const;
		bool operator==(const Pointer<T>& ptr) const;
		bool operator!=(const Pointer<T>& ptr) const;
		///M

		///T Comparison operators
		/// Compares the stored pointer with another pointer
		///M
		bool operator<(const WeakPointer<T>& ptr) const;
		bool operator>(const WeakPointer<T>& ptr) const;
		bool operator<=(const WeakPointer<T>& ptr) const;
		bool operator>=(const WeakPointer<T>& ptr) const;
		bool operator<(const Pointer<T>& ptr) const;
		bool operator>(const Pointer<T>& ptr) const;
		bool operator<=(const Pointer<T>& ptr) const;
		bool operator>=(const Pointer<T>& ptr) const;
		///M

		///H Conversions

		///T Conversion to pointer
		/// Conversion to the stored pointer
		explicit operator T*() const;

		///T bool conversion
		///R bool: <code>true</code> if the pointer is not a <code>nullptr</code>
		/// <code>false</code> otherwise
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
