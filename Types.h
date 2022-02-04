#pragma once

template <class T>
class Weak;

template <class T>
class Ptr {
public:
	Ptr() {
		ptr = nullptr;
	}

	Ptr(T* const ptr) {
		this->ptr = ptr;
	}

	Ptr(Ptr<T>& ptr) {
		this->ptr = *ptr;
		ptr.ptr = nullptr;
	}

	Ptr(Ptr<T>&& ptr) {
		this->ptr = *ptr;
		ptr.ptr = nullptr;
	}

	template <class U>
	Ptr(Ptr<U>& ptr) {
		this->ptr = *ptr;
		ptr.ptr = nullptr;
	}

	template <class U>
	Ptr(Ptr<U>&& ptr) {
		this->ptr = *ptr;
		ptr.ptr = nullptr;
	}

	~Ptr() {
		Free();
	}

	template <class Type>
	bool Is() const {
		return dynamic_cast<Type*>(ptr) != nullptr;
	}

	template <class Type>
	Weak<Type> As() const {
		return Weak<T>(*this).As<Type>();
	}

	void operator=(T* const ptr) {
		Free();
		this->ptr = ptr;
	}

	void operator=(Ptr<T>& ptr) {
		Free();
		this->ptr = *ptr;
		ptr.ptr = nullptr;
	}

	void operator=(Ptr<T>&& ptr) {
		Free();
		this->ptr = *ptr;
		ptr.ptr = nullptr;
	}

	template <class U>
	void operator=(Ptr<U>& ptr) {
		Free();
		this->ptr = *ptr;
		ptr.ptr = nullptr;
	}

	template <class U>
	void operator=(Ptr<U>&& ptr) {
		Free();
		this->ptr = *ptr;
		ptr.ptr = nullptr;
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

private:
	T* ptr;

	void Free() {
		if (ptr) {
			delete ptr;
			ptr = nullptr;
		}
	}
};


template <class T>
class Fixed {
public:
	Fixed() {

	}

	Fixed(const T& value) {
		this->value = value;
	}

	Fixed(T&& value) {
		this->value = value;
	}

	~Fixed() {

	}

	template <class Type>
	bool Is() const {
		return dynamic_cast<Type*>(&value) != nullptr;
	}

	template <class Type>
	Weak<Type> As() const {
		return Weak<T>(*this).As<Type>();
	}

	void operator=(const T& value) {
		this->value = value;
	}

	void operator=(T&& value) {
		this->value = value;
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

	template <class U>
	Weak(const Ptr<U>& ptr) {
		this->ptr = *ptr;
	}

	template <class U>
	Weak(Fixed<U>& ptr) {
		this->ptr = *ptr;
	}

	template <class U>
	Weak(const Weak<U>& ptr) {
		this->ptr = *ptr;
	}

	template <class U>
	Weak(Weak<U>&& ptr) {
		this->ptr = *ptr;
	}

	template <class Type>
	bool Is() const {
		return dynamic_cast<Type*>(ptr) != nullptr;
	}

	template <class Type>
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

	template <class U>
	void operator=(const Ptr<U>& ptr) {
		this->ptr = *ptr;
	}

	template <class U>
	void operator=(Fixed<U>& ptr) {
		this->ptr = *ptr;
	}

	template <class U>
	void operator=(const Weak<U>& ptr) {
		this->ptr = *ptr;
	}

	template <class U>
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

