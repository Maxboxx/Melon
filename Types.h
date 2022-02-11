#pragma once

#include <utility>

template<class T, class U>
concept PtrDerived = std::derived_from<T, U>;

template<class T, class U>
concept PtrConvert = std::derived_from<T, U> || std::derived_from<U, T>;

struct _Ref {
	void* ptr;
	int   ref;

	_Ref() : ptr(nullptr), ref(0) {}
	_Ref(void* ptr): ptr(ptr), ref(0) {}
	_Ref(void* ptr, int ref): ptr(ptr), ref(ref) {}
};

struct _Ptr {
	_Ref* ref;

	_Ptr() : ref(nullptr) {}
	_Ptr(_Ref* ref) : ref(ref) {}
	_Ptr(void* ptr, int ref) : ref(new _Ref(ptr, ref)) {}
};

template <class T>
class Weak;

template <class T>
class Ptr {
public:
	Ptr() {
		ptr = nullptr;
	}

	Ptr(T* ptr) {
		if (ptr == nullptr) return;
		this->ptr = new _Ptr(ptr, 0);
	}

	Ptr(Ptr<T>& ptr) {
		this->ptr = ptr.ptr;
		ptr.ptr = nullptr;
	}

	Ptr(const Ptr<T>& ptr) {
		if (ptr.ptr && ptr.ptr->ref) {
			this->ptr = new _Ptr(ptr.ptr->ref);
			ptr.ptr->ref = nullptr;
		}
	}

	Ptr(Ptr<T>&& ptr) {
		this->ptr = ptr.ptr;
		ptr.ptr = nullptr;
	}

	template <PtrDerived<T> U>
	Ptr(Ptr<U>& ptr) {
		this->ptr = ptr.ptr;
		ptr.ptr = nullptr;
	}

	template <PtrDerived<T> U>
	Ptr(const Ptr<U>& ptr) {
		if (ptr.ptr && ptr.ptr->ref) {
			this->ptr = new _Ptr(ptr.ptr->ref);
			ptr.ptr->ref = nullptr;
		}
	}

	template <PtrDerived<T> U>
	Ptr(Ptr<U>&& ptr) {
		this->ptr = ptr.ptr;
		ptr.ptr = nullptr;
	}

	~Ptr() {
		FreeAll();
	}

	template <PtrConvert<T> Type>
	bool Is() const {
		return dynamic_cast<Type*>(ptr && ptr->ref ? (T*)ptr->ref->ptr : nullptr) != nullptr;
	}

	template <PtrConvert<T> Type>
	Weak<Type> As() const {
		return Weak<T>(*this).As<Type>();
	}

	template <PtrConvert<T> Type>
	Ptr<Type> AsPtr() {
		if (Is<Type>()) {
			_Ref* ref = ptr->ref;
			ptr->ref = nullptr;

			Ptr<Type> t = nullptr;
			t.ptr = new _Ptr(ref);
			return t;
		}

		return nullptr;
	}

	void operator=(T* ptr) {
		FreeInstance();

		if (ptr == nullptr) return;

		if (this->ptr) {
			this->ptr->ref = new _Ref(ptr);
		}
		else {
			this->ptr = new _Ptr(ptr, 0);
		}
	}

	void operator=(Ptr<T>& ptr) {
		FreeAll();
		this->ptr = ptr.ptr;
		ptr.ptr = nullptr;
	}

	void operator=(const Ptr<T>& ptr) {
		FreeInstance();

		if (ptr.ptr && ptr.ptr->ref) {
			if (this->ptr) {
				this->ptr->ref = ptr.ptr->ref;
			}
			else {
				this->ptr = new _Ptr(ptr.ptr->ref);
			}

			ptr.ptr->ref = nullptr;
		}
	}

	void operator=(Ptr<T>&& ptr) {
		FreeAll();
		this->ptr = ptr.ptr;
		ptr.ptr = nullptr;
	}

	template <PtrDerived<T> U>
	void operator=(Ptr<U>& ptr) {
		FreeAll();
		this->ptr = ptr.ptr;
		ptr.ptr = nullptr;
	}

	template <PtrDerived<T> U>
	void operator=(const Ptr<U>& ptr) {
		FreeInstance();

		if (ptr.ptr && ptr.ptr->ref) {
			if (this->ptr) {
				this->ptr->ref = ptr.ptr->ref;
			}
			else {
				this->ptr = new _Ptr(ptr.ptr->ref);
			}

			ptr.ptr->ref = nullptr;
		}
	}

	template <PtrDerived<T> U>
	void operator=(Ptr<U>&& ptr) {
		FreeAll();
		this->ptr = ptr.ptr;
		ptr.ptr = nullptr;
	}

	T* operator->() const {
		return ptr && ptr->ref ? (T*)ptr->ref->ptr : nullptr;
	}

	T* operator*() const {
		return ptr && ptr->ref ? (T*)ptr->ref->ptr : nullptr;
	}

	bool operator!() const {
		return !ptr || !ptr->ref || !ptr->ref->ptr;
	}

	explicit operator bool() const {
		return ptr && ptr->ref && ptr->ref->ptr;
	}

private:
	template <class U>
	friend class Ptr;

	template <class U>
	friend class Weak;

	_Ptr* ptr = nullptr;

	void FreeAll() {
		if (ptr) {
			if (ptr->ref) {
				delete ptr->ref->ptr;
				ptr->ref->ptr = nullptr;

				if (ptr->ref->ref == 0) {
					delete ptr->ref;
				}

				ptr->ref = nullptr;
			}

			delete ptr;
			ptr = nullptr;
		}
	}

	void FreeInstance() {
		if (ptr && ptr->ref) {
			delete ptr->ref->ptr;
			ptr->ref->ptr = nullptr;

			if (ptr->ref->ref == 0) {
				delete ptr->ref;
			}

			ptr->ref = nullptr;
		}
	}
};


template <class T>
class Weak {
public:
	Weak() {
		ref = nullptr;
	}

	Weak(std::nullptr_t) {
		ref = nullptr;
	}

	Weak(const Ptr<T>& ptr) {
		if (ptr.ptr) {
			ref = ptr.ptr->ref;
			if (ref) ref->ref++;
		}
	}

	Weak(const Weak<T>& ptr) {
		ref = ptr.ref;

		if (ref) {
			ref->ref++;
		}
	}

	Weak(Weak<T>&& ptr) {
		ref = ptr.ref;
		ptr.ref = nullptr;
	}

	template <PtrDerived<T> U>
	Weak(const Ptr<U>& ptr) {
		if (ptr.ptr) {
			ref = ptr.ptr->ref;
			if (ref) ref->ref++;
		}
	}

	template <PtrDerived<T> U>
	Weak(const Weak<U>& ptr) {
		ref = ptr.ref;

		if (ref) {
			ref->ref++;
		}
	}

	template <PtrDerived<T> U>
	Weak(Weak<U>&& ptr) {
		ref = ptr.ref;
		ptr.ref = nullptr;
	}

	~Weak() {
		Free();
	}

	template <PtrConvert<T> Type>
	bool Is() const {
		return dynamic_cast<Type*>(ref ? (T*)ref->ptr : nullptr) != nullptr;
	}

	template <PtrConvert<T> Type>
	Weak<Type> As() const {
		if (Is<Type>()) {
			Weak<Type> t = nullptr;
			t.ref = ref;

			if (t.ref) {
				t.ref->ref++;
			}

			return t;
		}

		return nullptr;
	}

	void operator=(const Ptr<T>& ptr) {
		Free();

		if (ptr.ptr) {
			ref = ptr.ptr->ref;
			if (ref) ref->ref++;
		}
	}

	void operator=(const Weak<T>& ptr) {
		Free();

		ref = ptr.ref;

		if (ref) {
			ref->ref++;
		}
	}

	void operator=(Weak<T>&& ptr) {
		Free();
		ref = ptr.ref;
		ptr.ref = nullptr;
	}

	template <PtrDerived<T> U>
	void operator=(const Ptr<U>& ptr) {
		Free();

		if (ptr.ptr) {
			ref = ptr.ptr->ref;
			if (ref) ref->ref++;
		}
	}

	template <PtrDerived<T> U>
	void operator=(const Weak<U>& ptr) {
		Free();

		ref = ptr.ref;

		if (ref) {
			ref->ref++;
		}
	}

	template <PtrDerived<T> U>
	void operator=(Weak<U>&& ptr) {
		Free();
		ref = ptr.ref;
		ptr.ref = nullptr;
	}

	T* operator->() const {
		return ref ? (T*)ref->ptr : nullptr;
	}

	T* operator*() const {
		return ref ? (T*)ref->ptr : nullptr;
	}

	bool operator!() const {
		return !ref || !ref->ptr;
	}

	explicit operator bool() const {
		return ref && ref->ptr;
	}

private:
	template <class U>
	friend class Weak;

	_Ref* ref;

	void Free() {
		if (ref && ref->ref) {
			ref->ref--;

			if (ref->ref == 0 && !ref->ptr) {
				delete ref;
			}
		}

		ref = nullptr;
	}
};

