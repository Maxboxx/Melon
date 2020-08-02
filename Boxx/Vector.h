#pragma once

#include "Types.h"
#include <math.h>

///N Vector
namespace Boxx {
	class Vector2;
	class Vector3;
	class Vector4;

	///B Vector2
	/// A class for 2D vectors
	class Vector2 {
	public:
		///H Values

		///T xy
		/// Used to get/set the components of the vector
		float x, y;

		///H Constructors

		///T Zero vector
		/// Creates a vector full of zeros
		Vector2();

		///T Float constructor
		/// Creates a vector with floats
		explicit Vector2(const float x, const float y);

		///T Fill
		/// Fills the entire vector with the same value
		explicit Vector2(const float v);

		Vector2(const Vector2& vector);

		///H Methods

		///T Length
		/// Gets the length of the vector
		float Length() const;

		///T Squared length
		/// Gets the squared length the vecor
		float SquaredLength() const;

		///T Normalize
		/// Normalizes the vector and returns the result
		Vector2 Normalize() const;

		///H Operators

		Vector2 operator=(const Vector2& v);

		///T Componentwise Addition
		///M
		Vector2 operator+(const float f) const;
		Vector2 operator+(const Vector2& v) const;
		void operator+=(const float f);
		void operator+=(const Vector2& v);
		///M

		///T Negation
		Vector2 operator-() const;

		///T Componentwise Subtraction
		///M
		Vector2 operator-(const float f) const;
		Vector2 operator-(const Vector2& v) const;
		void operator-=(const float f);
		void operator-=(const Vector2& v);
		///M

		///T Componentwise Multiplication
		///M
		Vector2 operator*(const float f) const;
		Vector2 operator*(const Vector2& v) const;
		void operator*=(const float f);
		void operator*=(const Vector2& v);
		///M

		///T Componentwise Division
		///M
		Vector2 operator/(const float f) const;
		Vector2 operator/(const Vector2& v) const;
		void operator/=(const float f);
		void operator/=(const Vector2& v);
		///M

		///T Componentwise Mod
		///M
		Vector2 operator%(const float f) const;
		Vector2 operator%(const Vector2& v) const;
		void operator%=(const float f);
		void operator%=(const Vector2& v);
		///M

		///T Equality
		/// Checks if vectors have the same value
		///M
		bool operator==(const Vector2& v) const;
		bool operator!=(const Vector2& v) const;
		///M

		///T Comparison
		/// Compares the length of vectors
		///M
		bool operator<(const Vector2& v) const;
		bool operator>(const Vector2& v) const;
		bool operator<=(const Vector2& v) const;
		bool operator>=(const Vector2& v) const;
		///M

		///T bool conversion
		/// Checks if the vector is full of zeros
		///M
		explicit operator bool() const;
		bool operator!() const;
		///M

		///T Conversion to different size
		/// Converts the vector to a different size of vector
		///M
		explicit operator Vector3() const;
		explicit operator Vector4() const;
		///M

		///H Static functions

		///T Dot product
		/// Calculates the dot product of two vectors
		static float Dot(const Vector2& vector1, const Vector2& vector2);

		///T Lerp
		/// Makes a linear interpolation between two vectors
		///A const Vector2& v1: The vector to start at
		///A const Vector2& v2: The vector to end at
		///A const float t: The progress of the interpolation
		/// The value should be between <code>0.0</code> and <code>1.0</code>
		static Vector2 Lerp(const Vector2& v1, const Vector2& v2, const float t);

		///T Up vector
		static Vector2 Up();

		///T Down vector
		static Vector2 Down();

		///T Left vector
		static Vector2 Left();

		///T Right vector
		static Vector2 Right();
	};

	///B Vector3
	/// A class for 3D vectors
	class Vector3 {
	public:
		///H Values

		///T xyz
		/// Used to get/set the components of the vector
		float x, y, z;

		///H Constructors

		///T Zero vector
		/// Creates a vector full of zeros
		Vector3();

		///T Float constructor
		/// Creates a vector with floats
		explicit Vector3(const float x, const float y, const float z);

		///T Fill
		/// Fills the entire vector with the same value
		explicit Vector3(const float v);

		///T Vector2 and float
		/// Creates a vector from a Vector2 and a float
		explicit Vector3(const Vector2& vector, const float z);

		Vector3(const Vector3& vector);

		///H Methods

		///T Length
		/// Gets the length of the vector
		float Length() const;

		///T Squared length
		/// Gets the squared length the vecor
		float SquaredLength() const;

		///T Normalize
		/// Normalizes the vector and returns the result
		Vector3 Normalize() const;
	
		///H Operators

		Vector3 operator=(const Vector3& v);

		///T Componentwise Addition
		///M
		Vector3 operator+(const float f) const;
		Vector3 operator+(const Vector3& v) const;
		void operator+=(const float f);
		void operator+=(const Vector3& v);
		///M

		///T Negation
		Vector3 operator-() const;

		///T Componentwise Subtraction
		///M
		Vector3 operator-(const float f) const;
		Vector3 operator-(const Vector3& v) const;
		void operator-=(const float f);
		void operator-=(const Vector3& v);
		///M

		///T Componentwise Multiplication
		///M
		Vector3 operator*(const float f) const;
		Vector3 operator*(const Vector3& v) const;
		void operator*=(const float f);
		void operator*=(const Vector3& v);
		///M

		///T Componentwise Division
		///M
		Vector3 operator/(const float f) const;
		Vector3 operator/(const Vector3& v) const;
		void operator/=(const float f);
		void operator/=(const Vector3& v);
		///M

		///T Componentwise Mod
		///M
		Vector3 operator%(const float f) const;
		Vector3 operator%(const Vector3& v) const;
		void operator%=(const float f);
		void operator%=(const Vector3& v);
		///M

		///T Equality
		/// Checks if vectors have the same value
		///M
		bool operator==(const Vector3& v) const;
		bool operator!=(const Vector3& v) const;
		///M

		///T Comparison
		/// Compares the length of vectors
		///M
		bool operator<(const Vector3& v) const;
		bool operator>(const Vector3& v) const;
		bool operator<=(const Vector3& v) const;
		bool operator>=(const Vector3& v) const;
		///M

		///T bool conversion
		/// Checks if the vector is full of zeros
		///M
		explicit operator bool() const;
		bool operator!() const;
		///M

		///T Conversion to different size
		/// Converts the vector to a different size of vector
		///M
		explicit operator Vector2() const;
		explicit operator Vector4() const;
		///M

		///H Static functions

		///T Dot product
		/// Calculates the dot product of two vectors
		static float Dot(const Vector3& v1, const Vector3& v2);

		///T Cross product
		/// Calculates the cross product of two vectors
		static Vector3 Cross(const Vector3& v1, const Vector3& v2);

		///T Lerp
		/// Makes a linear interpolation between two vectors
		///A const Vector3& v1: The vector to start at
		///A const Vector3& v2: The vector to end at
		///A const float t: The progress of the interpolation
		/// The value should be between <code>0.0</code> and <code>1.0</code>
		static Vector3 Lerp(const Vector3& v1, const Vector3& v2, const float t);

		///T Up vector
		static Vector3 Up();

		///T Down vector
		static Vector3 Down();

		///T Left vector
		static Vector3 Left();

		///T Right vector
		static Vector3 Right();

		///T Forward vector
		static Vector3 Forward();

		///T Backward vector
		static Vector3 Backward();
	};

	///B Vector4
	/// A class for 4D vectors
	class Vector4 {
	public:
		///H Values

		///T xyzw
		/// Used to get/set the components of the vector
		float x, y, z, w;

		///H Constructors

		///T Zero vector
		/// Creates a vector full of zeros
		Vector4();

		///T Float constructor
		/// Creates a vector with floats
		explicit Vector4(const float x, const float y, const float z, const float w);

		///T Fill
		/// Fills the entire vector with the same value
		explicit Vector4(const float v);

		///T Vector2 and floats
		/// Creates a vector from a Vector2 and two floats
		explicit Vector4(const Vector2& v, const float z, const float w);

		///T Vector3 and float
		/// Creates a vector from a Vector3 and a float
		explicit Vector4(const Vector3& v, const float w);

		Vector4(const Vector4& vector);

		///H Methods

		///T Length
		/// Gets the length of the vector
		float Length() const;

		///T Squared length
		/// Gets the squared length the vecor
		float SquaredLength() const;

		///T Normalize
		/// Normalizes the vector and returns the result
		Vector4 Normalize() const;

		///H Operators

		Vector4 operator=(const Vector4& v);

		///T Componentwise Addition
		///M
		Vector4 operator+(const float f) const;
		Vector4 operator+(const Vector4& v) const;
		void operator+=(const float f);
		void operator+=(const Vector4& v);
		///M

		///T Negation
		Vector4 operator-() const;

		///T Componentwise Subtraction
		///M
		Vector4 operator-(const float f) const;
		Vector4 operator-(const Vector4& v) const;
		void operator-=(const float f);
		void operator-=(const Vector4& v);
		///M

		///T Componentwise Multiplication
		///M
		Vector4 operator*(const float f) const;
		Vector4 operator*(const Vector4& v) const;
		void operator*=(const float f);
		void operator*=(const Vector4& v);
		///M

		///T Componentwise Division
		///M
		Vector4 operator/(const float f) const;
		Vector4 operator/(const Vector4& v) const;
		void operator/=(const float f);
		void operator/=(const Vector4& v);
		///M

		///T Componentwise Mod
		///M
		Vector4 operator%(const float f) const;
		Vector4 operator%(const Vector4& v) const;
		void operator%=(const float f);
		void operator%=(const Vector4& v);
		///M

		///T Equality
		/// Checks if vectors have the same value
		///M
		bool operator==(const Vector4& v) const;
		bool operator!=(const Vector4& v) const;
		///M

		///T Comparison
		/// Compares the length of vectors
		///M
		bool operator<(const Vector4& v) const;
		bool operator>(const Vector4& v) const;
		bool operator<=(const Vector4& v) const;
		bool operator>=(const Vector4& v) const;
		///M

		///T bool conversion
		/// Checks if the vector is full of zeros
		///M
		explicit operator bool() const;
		bool operator!() const;
		///M

		///T Conversion to different size
		/// Converts the vector to a different size of vector
		///M
		explicit operator Vector2() const;
		explicit operator Vector3() const;
		///M

		///H Static functions

		///T Dot product
		/// Calculates the dot product of two vectors
		static float Dot(const Vector4& v1, const Vector4& v2);

		///T Lerp
		/// Makes a linear interpolation between two vectors
		///A const Vector4& v1: The vector to start at
		///A const Vector4& v2: The vector to end at
		///A const float t: The progress of the interpolation
		/// The value should be between <code>0.0</code> and <code>1.0</code>
		static Vector4 Lerp(const Vector4& v1, const Vector4& v2, const float t);
	};

	inline Vector2::Vector2() {x = 0.f; y = 0.f;}
	inline Vector2::Vector2(const float x, const float y) {this->x = x; this->y = y;}
	inline Vector2::Vector2(const float v) {this->x = v; this->y = v;}
	inline Vector2::Vector2(const Vector2& vector) {x = vector.x; y = vector.y;}

	inline Vector2 Vector2::operator=(const Vector2& v) {x = v.x; y = v.y; return *this;}

	inline Vector2 Vector2::operator+(const float f) const {return Vector2(x + f, y + f);}
	inline Vector2 Vector2::operator+(const Vector2& v) const {return Vector2(x + v.x, y + v.y);}
	inline void Vector2::operator+=(const float f) {x += f; y += f;}
	inline void Vector2::operator+=(const Vector2& v) {x += v.x; y += v.y;}

	inline Vector2 Vector2::operator-() const {return Vector2(-x, -y);}
	inline Vector2 Vector2::operator-(const float f) const {return Vector2(x - f, y - f);}
	inline Vector2 Vector2::operator-(const Vector2& v) const {return Vector2(x - v.x, y - v.y);}
	inline void Vector2::operator-=(const float f) {x -= f; y -= f;}
	inline void Vector2::operator-=(const Vector2& v) {x -= v.x; y -= v.y;}

	inline Vector2 Vector2::operator*(const float f) const {return Vector2(x * f, y * f);}
	inline Vector2 Vector2::operator*(const Vector2& v) const {return Vector2(x * v.x, y * v.y);}
	inline void Vector2::operator*=(const float f) {x *= f; y *= f;}
	inline void Vector2::operator*=(const Vector2& v) {x *= v.x; y *= v.y;}

	inline Vector2 Vector2::operator/(const float f) const {return Vector2(x / f, y / f);}
	inline Vector2 Vector2::operator/(const Vector2& v) const {return Vector2(x / v.x, y / v.y);}
	inline void Vector2::operator/=(const float f) {x /= f; y /= f;}
	inline void Vector2::operator/=(const Vector2& v) {x /= v.x; y /= v.y;}

	inline Vector2 Vector2::operator%(const float f) const {return Vector2(fmod(x, f), fmod(y, f));}
	inline Vector2 Vector2::operator%(const Vector2& v) const {return Vector2(fmod(x, v.x), fmod(y, v.y));}
	inline void Vector2::operator%=(const float f) {x = fmod(x, f); y = fmod(y, f);}
	inline void Vector2::operator%=(const Vector2& v) {x = fmod(x, v.x); y = fmod(y, v.y);}

	inline bool Vector2::operator==(const Vector2& v) const {return x == v.x && y == v.y;}
	inline bool Vector2::operator!=(const Vector2& v) const {return x != v.x || y != v.y;}

	inline bool Vector2::operator<(const Vector2& v) const {return SquaredLength() < v.SquaredLength();}
	inline bool Vector2::operator>(const Vector2& v) const {return SquaredLength() > v.SquaredLength();}
	inline bool Vector2::operator<=(const Vector2& v) const {return SquaredLength() <= v.SquaredLength();}
	inline bool Vector2::operator>=(const Vector2& v) const {return SquaredLength() >= v.SquaredLength();}

	inline Vector2::operator bool() const {return x != 0.f || y != 0.f;}
	inline bool Vector2::operator!() const {return x == 0.f && y == 0.f;}

	inline float Vector2::Length() const {return sqrtf(x * x + y * y);}
	inline float Vector2::SquaredLength() const {return x * x + y * y;}
	inline Vector2 Vector2::Normalize() const {return *this / Length();}

	inline float Vector2::Dot(const Vector2& vector1, const Vector2& vector2) {return vector1.x * vector2.x + vector1.y * vector2.y;}
	inline Vector2 Vector2::Lerp(const Vector2& v1, const Vector2& v2, const float t) {const float it = 1.f - t; return Vector2(it * v1.x + t * v2.x, it * v1.y + t * v2.y);}

	inline Vector2 Vector2::Up() {return Vector2(0.f, 1.f);}
	inline Vector2 Vector2::Down() {return Vector2(0.f, -1.f);}
	inline Vector2 Vector2::Left() {return Vector2(-1.f, 0.f);}
	inline Vector2 Vector2::Right() {return Vector2(1.f, 0.f);}

	inline Vector3::Vector3() {x = 0.f; y = 0.f; z = 0.f;}
	inline Vector3::Vector3(const float x, const float y, const float z) {this->x = x; this->y = y; this->z = z;}
	inline Vector3::Vector3(const float v) {this->x = v; this->y = v; this->z = v;}
	inline Vector3::Vector3(const Vector2& vector, const float z) {x = vector.x; y = vector.y; this->z = z;}
	inline Vector3::Vector3(const Vector3& vector) {x = vector.x; y = vector.y; z = vector.z;}
	
	inline Vector3 Vector3::operator=(const Vector3& v) {x = v.x; y = v.y; z = v.z; return *this;}

	inline Vector3 Vector3::operator+(const float f) const {return Vector3(x + f, y + f, z + f);}
	inline Vector3 Vector3::operator+(const Vector3& v) const {return Vector3(x + v.x, y + v.y, z + v.z);}
	inline void Vector3::operator+=(const float f) {x += f; y += f; z += f;}
	inline void Vector3::operator+=(const Vector3& v) {x += v.x; y += v.y; z += v.z;}

	inline Vector3 Vector3::operator-() const {return Vector3(-x, -y, -z);}
	inline Vector3 Vector3::operator-(const float f) const {return Vector3(x - f, y - f, z - f);}
	inline Vector3 Vector3::operator-(const Vector3& v) const {return Vector3(x - v.x, y - v.y, z - v.z);}
	inline void Vector3::operator-=(const float f) {x -= f; y -= f; z -= f;}
	inline void Vector3::operator-=(const Vector3& v) {x -= v.x; y -= v.y; z -= v.z;}

	inline Vector3 Vector3::operator*(const float f) const {return Vector3(x * f, y * f, z * f);}
	inline Vector3 Vector3::operator*(const Vector3& v) const {return Vector3(x * v.x, y * v.y, z * v.z);}
	inline void Vector3::operator*=(const float f) {x *= f; y *= f; z *= f;}
	inline void Vector3::operator*=(const Vector3& v) {x *= v.x; y *= v.y; z *= v.z;}

	inline Vector3 Vector3::operator/(const float f) const {return Vector3(x / f, y / f, z / f);}
	inline Vector3 Vector3::operator/(const Vector3& v) const {return Vector3(x / v.x, y / v.y, z / v.z);}
	inline void Vector3::operator/=(const float f) {x /= f; y /= f; z /= f;}
	inline void Vector3::operator/=(const Vector3& v) {x /= v.x; y /= v.y; z /= v.z;}

	inline Vector3 Vector3::operator%(const float f) const {return Vector3(fmod(x, f), fmod(y, f), fmod(z, f));}
	inline Vector3 Vector3::operator%(const Vector3& v) const {return Vector3(fmod(x, v.x), fmod(y, v.y), fmod(z, v.z));}
	inline void Vector3::operator%=(const float f) {x = fmod(x, f); y = fmod(y, f); z = fmod(z, f);}
	inline void Vector3::operator%=(const Vector3& v) {x = fmod(x, v.x); y = fmod(y, v.y); z = fmod(z, v.z);}

	inline bool Vector3::operator==(const Vector3& v) const {return x == v.x && y == v.y && z == v.z;}
	inline bool Vector3::operator!=(const Vector3& v) const {return x != v.x || y != v.y || z != v.z;}

	inline bool Vector3::operator<(const Vector3& v) const {return SquaredLength() < v.SquaredLength();}
	inline bool Vector3::operator>(const Vector3& v) const {return SquaredLength() > v.SquaredLength();}
	inline bool Vector3::operator<=(const Vector3& v) const {return SquaredLength() <= v.SquaredLength();}
	inline bool Vector3::operator>=(const Vector3& v) const {return SquaredLength() >= v.SquaredLength();}

	inline Vector3::operator bool() const {return x != 0.f || y != 0.f || z != 0.f;}
	inline bool Vector3::operator!() const {return x == 0.f && y == 0.f && z == 0.f;}

	inline float Vector3::Length() const {return sqrtf(x * x + y * y + z * z);}
	inline float Vector3::SquaredLength() const {return x * x + y * y + z * z;}
	inline Vector3 Vector3::Normalize() const {return *this / Length();}

	inline float Vector3::Dot(const Vector3& v1, const Vector3& v2) {return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;}
	inline Vector3 Vector3::Cross(const Vector3& v1, const Vector3& v2) {return Vector3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);}
	inline Vector3 Vector3::Lerp(const Vector3& v1, const Vector3& v2, const float t) {const float it = 1.f - t; return Vector3(it * v1.x + t * v2.x, it * v1.y + t * v2.y, it * v1.z + t * v2.z);}

	inline Vector3 Vector3::Up() {return Vector3(0.f, 1.f, 0.f);}
	inline Vector3 Vector3::Down() {return Vector3(0.f, -1.f, 0.f);}
	inline Vector3 Vector3::Left() {return Vector3(-1.f, 0.f, 0.f);}
	inline Vector3 Vector3::Right() {return Vector3(1.f, 0.f, 0.f);}
	inline Vector3 Vector3::Forward() {return Vector3(0.f, 0.f, -1.f);}
	inline Vector3 Vector3::Backward() {return Vector3(0.f, 0.f, 1.f);}

	inline Vector4::Vector4() {x = 0.f; y = 0.f; z = 0.f; w = 0.f;}
	inline Vector4::Vector4(const float x, const float y, const float z, const float w) {this->x = x; this->y = y; this->z = z; this->w = w;}
	inline Vector4::Vector4(const float v) {this->x = v; this->y = v; this->z = v; this->w = v;}
	inline Vector4::Vector4(const Vector2& v, const float z, const float w) {x = v.x; y = v.y; this->z = z; this->w = w;}
	inline Vector4::Vector4(const Vector3& v, const float w) {x = v.x; y = v.y; z = v.z; this->w = w;}
	inline Vector4::Vector4(const Vector4& vector) {x = vector.x; y = vector.y; z = vector.z; w = vector.w;}

	inline Vector4 Vector4::operator =(const Vector4& v) {x = v.x; y = v.y; z = v.z; w = v.w; return *this;}

	inline Vector4 Vector4::operator+(const float f) const {return Vector4(x + f, y + f, z + f, w + f);}
	inline Vector4 Vector4::operator+(const Vector4& v) const {return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);}
	inline void Vector4::operator+=(const float f) {x += f; y += f; z += f; w += f;}
	inline void Vector4::operator+=(const Vector4& v) {x += v.x; y += v.y; z += v.z; w += v.w;}

	inline Vector4 Vector4::operator-() const {return Vector4(-x, -y, -z, -w);}
	inline Vector4 Vector4::operator-(const float f) const {return Vector4(x - f, y - f, z - f, w - f);}
	inline Vector4 Vector4::operator-(const Vector4& v) const {return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);}
	inline void Vector4::operator-=(const float f) {x -= f; y -= f; z -= f; w -= f;}
	inline void Vector4::operator-=(const Vector4& v) {x -= v.x; y -= v.y; z -= v.z; w -= v.w;}

	inline Vector4 Vector4::operator*(const float f) const {return Vector4(x * f, y * f, z * f, w * f);}
	inline Vector4 Vector4::operator*(const Vector4& v) const {return Vector4(x * v.x, y * v.y, z * v.z, w * v.w);}
	inline void Vector4::operator*=(const float f) {x *= f; y *= f; z *= f; w *= f;}
	inline void Vector4::operator*=(const Vector4& v) {x *= v.x; y *= v.y; z *= v.z; w *= v.w;}

	inline Vector4 Vector4::operator/(const float f) const {return Vector4(x / f, y / f, z / f, w / f);}
	inline Vector4 Vector4::operator/(const Vector4& v) const {return Vector4(x / v.x, y / v.y, z / v.z, w / v.w);}
	inline void Vector4::operator/=(const float f) {x /= f; y /= f; z /= f; w /= f;}
	inline void Vector4::operator/=(const Vector4& v) {x /= v.x; y /= v.y; z /= v.z; w /= v.w;}

	inline Vector4 Vector4::operator%(const float f) const {return Vector4(fmod(x, f), fmod(y, f), fmod(z, f), fmod(w, f));}
	inline Vector4 Vector4::operator%(const Vector4& v) const {return Vector4(fmod(x, v.x), fmod(y, v.y), fmod(z, v.z), fmod(w, v.w));}
	inline void Vector4::operator%=(const float f) {x = fmod(x, f); y = fmod(y, f); z = fmod(z, f); w = fmod(w, f);}
	inline void Vector4::operator%=(const Vector4& v) {x = fmod(x, v.x); y = fmod(y, v.y); z = fmod(z, v.z); w = fmod(w, v.w);}

	inline bool Vector4::operator==(const Vector4& v) const {return x == v.x && y == v.y && z == v.z && w == v.w;}
	inline bool Vector4::operator!=(const Vector4& v) const {return x != v.x || y != v.y || z != v.z || w != v.w;}

	inline bool Vector4::operator<(const Vector4& v) const {return SquaredLength() < v.SquaredLength();}
	inline bool Vector4::operator>(const Vector4& v) const {return SquaredLength() > v.SquaredLength();}
	inline bool Vector4::operator<=(const Vector4& v) const {return SquaredLength() <= v.SquaredLength();}
	inline bool Vector4::operator>=(const Vector4& v) const {return SquaredLength() >= v.SquaredLength();}

	inline Vector4::operator bool() const {return x != 0.f || y != 0.f || z != 0.f || w != 0.f;}
	inline bool Vector4::operator!() const {return x == 0.f && y == 0.f && z == 0.f && w == 0.f;}

	inline float Vector4::Length() const {return sqrtf(x * x + y * y + z * z + w * w);}
	inline float Vector4::SquaredLength() const {return x * x + y * y + z * z + w * w;}
	inline Vector4 Vector4::Normalize() const {return *this / Length();}

	inline float Vector4::Dot(const Vector4& v1, const Vector4& v2) {return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;}
	inline Vector4 Vector4::Lerp(const Vector4& v1, const Vector4& v2, const float t) {const float it = 1.f - t; return Vector4(it * v1.x + t * v2.x, it * v1.y + t * v2.y, it * v1.z + t * v2.z, it * v1.w + t * v2.w);}

	inline Vector2::operator Vector3() const {return Vector3(x, y, 0.f);}
	inline Vector2::operator Vector4() const {return Vector4(x, y, 0.f, 0.f);}

	inline Vector3::operator Vector2() const {return Vector2(x, y);}
	inline Vector3::operator Vector4() const {return Vector4(x, y, z, 0.f);}

	inline Vector4::operator Vector2() const {return Vector2(x, y);}
	inline Vector4::operator Vector3() const {return Vector3(x, y, z);}

	inline Vector2 operator+(const float f, const Vector2& v) {return v + f;}
	inline Vector3 operator+(const float f, const Vector3& v) {return v + f;}
	inline Vector4 operator+(const float f, const Vector4& v) {return v + f;}

	inline Vector2 operator*(const float f, const Vector2& v) {return v * f;}
	inline Vector3 operator*(const float f, const Vector3& v) {return v * f;}
	inline Vector4 operator*(const float f, const Vector4& v) {return v * f;}

	inline Vector2 operator-(const float f, const Vector2& v) {return Vector2(f - v.x, f - v.y);}
	inline Vector3 operator-(const float f, const Vector3& v) {return Vector3(f - v.x, f - v.y, f - v.z);}
	inline Vector4 operator-(const float f, const Vector4& v) {return Vector4(f - v.x, f - v.y, f - v.z, f - v.w);}

	inline Vector2 operator/(const float f, const Vector2& v) {return Vector2(f / v.x, f / v.y);}
	inline Vector3 operator/(const float f, const Vector3& v) {return Vector3(f / v.x, f / v.y, f / v.z);}
	inline Vector4 operator/(const float f, const Vector4& v) {return Vector4(f / v.x, f / v.y, f / v.z, f / v.w);}

	inline Vector2 operator%(const float f, const Vector2& v) {return Vector2(fmod(f, v.x), fmod(f, v.y));}
	inline Vector3 operator%(const float f, const Vector3& v) {return Vector3(fmod(f, v.x), fmod(f, v.y), fmod(f, v.z));}
	inline Vector4 operator%(const float f, const Vector4& v) {return Vector4(fmod(f, v.x), fmod(f, v.y), fmod(f, v.z), fmod(f, v.w));}
}
