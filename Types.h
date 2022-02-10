#pragma once

#include <utility>

template<class T, class U>
concept PtrDerived = std::derived_from<T, U>;

template<class T, class U>
concept PtrConvert = std::derived_from<T, U> || std::derived_from<U, T>;

template <class T>
class Weak;

class A {public: virtual ~A(){}};
class B : public A {};

template <class T>
class Ptr {
public:
	Ptr() {
		ptr = nullptr;
	}

	Ptr(T* const ptr) {
		this->ptr = new _Ptr(ptr);
	}

	Ptr(Ptr<T>& ptr) {
		this->ptr = ptr.ptr;
		ptr.ptr = nullptr;
	}

	Ptr(const Ptr<T>& ptr) {
		this->ptr = new _Ptr(ptr.ptr->ptr);
		ptr.ptr->ptr = nullptr;
	}

	Ptr(Ptr<T>&& ptr) {
		this->ptr = ptr.ptr;
		ptr.ptr = nullptr;
	}

	template <PtrDerived<T> U>
	Ptr(const Ptr<U>& ptr) {
		this->ptr = new _Ptr(ptr.ptr->ptr);
		ptr.ptr->ptr = nullptr;
	}

	template <PtrDerived<T> U>
	Ptr(Ptr<U>&& ptr) {
		this->ptr = new _Ptr(ptr.ptr->ptr);
		ptr.ptr->ptr = nullptr;
		delete ptr.ptr;
		ptr.ptr = nullptr;
	}

	~Ptr() {
		FreeAll();
	}

	template <PtrConvert<T> Type>
	bool Is() const {
		return dynamic_cast<Type*>(ptr ? ptr->ptr : nullptr) != nullptr;
	}

	template <PtrConvert<T> Type>
	Weak<Type> As() const {
		return Weak<T>(*this).As<Type>();
	}

	template <PtrConvert<T> Type>
	Ptr<Type> AsPtr() {
		Type* t = dynamic_cast<Type*>(ptr ? ptr->ptr : nullptr);
		ptr = nullptr;
		return t;
	}

	void operator=(T* const ptr) {
		FreeInstance();

		if (this->ptr) 
			this->ptr->ptr = ptr;
		else
			this->ptr = new _Ptr(ptr);
	}

	void operator=(Ptr<T>& ptr) {
		FreeAll();
		this->ptr = ptr.ptr;
		ptr.ptr = nullptr;
	}

	void operator=(const Ptr<T>& ptr) {
		FreeInstance();

		if (this->ptr)
			this->ptr->ptr = ptr.ptr->ptr;
		else
			this->ptr = new _Ptr(ptr.ptr->ptr);

		ptr.ptr->ptr = nullptr;
	}

	void operator=(Ptr<T>&& ptr) {
		FreeAll();
		this->ptr = ptr.ptr;
		ptr.ptr = nullptr;
	}

	template <PtrDerived<T> U>
	void operator=(const Ptr<U>& ptr) {
		FreeInstance();

		if (this->ptr)
			this->ptr->ptr = ptr.ptr->ptr;
		else
			this->ptr = new _Ptr(ptr.ptr->ptr);

		ptr.ptr->ptr = nullptr;
	}

	template <PtrDerived<T> U>
	void operator=(Ptr<U>&& ptr) {
		FreeInstance();

		if (this->ptr)
			this->ptr->ptr = ptr.ptr->ptr;
		else
			this->ptr = new _Ptr(ptr.ptr->ptr);

		ptr.ptr->ptr = nullptr;
		ptr.FreeAll();
	}

	T* operator->() const {
		return ptr ? ptr->ptr : nullptr;
	}

	T* operator*() const {
		return ptr ? ptr->ptr : nullptr;
	}

	bool operator!() const {
		return ptr == nullptr || ptr->ptr == nullptr;
	}

	explicit operator bool() const {
		return ptr != nullptr && ptr->ptr != nullptr;
	}

private:
	template <class U>
	friend class Ptr;

	struct _Ptr {
		T* ptr;

		_Ptr(T* const t): ptr(t) {}
	};

	_Ptr* ptr;

	void FreeAll() {
		if (ptr) {
			if (ptr->ptr) {
				delete ptr->ptr;
				ptr->ptr = nullptr;
			}

			delete ptr;
			ptr = nullptr;
		}
	}

	void FreeInstance() {
		if (ptr && ptr->ptr) {
			delete ptr->ptr;
			ptr->ptr = nullptr;
		}
	}
};


template <class T>
class Fixed {
public:
	Fixed() {

	}

	Fixed(const T& value) : value(value) {

	}

	Fixed(T&& value) : value(value) {

	}

	~Fixed() {

	}

	template <PtrConvert<T> Type>
	bool Is() const {
		return dynamic_cast<Type*>(&value) != nullptr;
	}

	template <PtrConvert<T> Type>
	Weak<Type> As() const {
		return Weak<T>(*this).As<Type>();
	}

	void operator=(const T& value) {
		this->value = value;
	}

	void operator=(T&& value) {
		this->value = std::move(value);
	}

	void operator=(const Fixed<T>& ptr) {
		this->value = ptr.value;
	}

	void operator=(Fixed<T>&& ptr) {
		this->value = std::move(ptr.value);
	}

	T* operator->() {
		return &value;
	}

	T* operator*() {
		return &value;
	}

	bool operator!() const {
		return false;
	}

	explicit operator bool() const {
		return true;
	}

private:
	T value;
};


template <class T>
class Weak {
public:
	Weak() {
		ptr = nullptr;
	}

	Weak(const Ptr<T>& ptr) {
		this->ptr = *ptr;
	}

	Weak(Fixed<T>& ptr) {
		this->ptr = *ptr;
	}

	Weak(const Weak<T>& ptr) {
		this->ptr = *ptr;
	}

	Weak(Weak<T>&& ptr) {
		this->ptr = *ptr;
	}

	template <PtrDerived<T> U>
	Weak(const Ptr<U>& ptr) {
		this->ptr = *ptr;
	}

	template <PtrDerived<T> U>
	Weak(Fixed<U>& ptr) {
		this->ptr = *ptr;
	}

	template <PtrDerived<T> U>
	Weak(const Weak<U>& ptr) {
		this->ptr = *ptr;
	}

	template <PtrDerived<T> U>
	Weak(Weak<U>&& ptr) {
		this->ptr = *ptr;
	}

	template <PtrConvert<T> Type>
	bool Is() const {
		return dynamic_cast<Type*>(ptr) != nullptr;
	}

	template <PtrConvert<T> Type>
	Weak<Type> As() const {
		Weak<Type> w;
		w.ptr = dynamic_cast<Type*>(ptr);
		return w;
	}

	void operator=(const Ptr<T>& ptr) {
		this->ptr = *ptr;
	}

	void operator=(Fixed<T>& ptr) {
		this->ptr = *ptr;
	}

	void operator=(const Weak<T>& ptr) {
		this->ptr = *ptr;
	}

	void operator=(Weak<T>&& ptr) {
		this->ptr = *ptr;
	}

	template <PtrDerived<T> U>
	void operator=(const Ptr<U>& ptr) {
		this->ptr = *ptr;
	}

	template <PtrDerived<T> U>
	void operator=(Fixed<U>& ptr) {
		this->ptr = *ptr;
	}

	template <PtrDerived<T> U>
	void operator=(const Weak<U>& ptr) {
		this->ptr = *ptr;
	}

	template <PtrDerived<T> U>
	void operator=(Weak<U>&& ptr) {
		this->ptr = *ptr;
	}

	T* operator->() const {
		return ptr;
	}

	T* operator*() const {
		return ptr;
	}

	bool operator!() const {
		return ptr == nullptr;
	}

	explicit operator bool() const {
		return ptr != nullptr;
	}

	static Weak<T> Ref(T* const instance) {
		Weak<T> w;
		w.ptr = instance;
		return w;
	}

private:
	template <class U>
	friend class Weak;

	T* ptr;
};

