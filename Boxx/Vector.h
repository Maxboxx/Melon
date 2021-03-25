#pragma once

#include "Types.h"
#include <math.h>

///[Settings] block: indent

///[Namespace] Boxx
namespace Boxx {
	struct Vector2;
	struct Vector3;
	struct Vector4;

	///[Heading] Math

	///[Title] Vector2
	/// A struct for 2D vectors.
	///[Block] Vector2
	struct Vector2 {
	public:
		///[Heading] Components

		///[Title] x
		float x;
		
		///[Title] y
		float y;

		///[Heading] Constructors

		/// Creates a vector full of zeros.
		Vector2();

		/// Creates a vector with floats.
		Vector2(const float x, const float y);

		/// Fills the entire vector with the same value.
		explicit Vector2(const float v);

		Vector2(const Vector2& vector);

		///[Heading] Methods

		/// Gets the length of the vector.
		float Length() const;

		/// Gets the squared length the vector.
		float SquaredLength() const;

		/// Normalizes the vector and returns the result.
		Vector2 Normalize() const;

		///[Heading] Operators

		Vector2 operator=(const Vector2& v);

		/// Adds each component with {f}.
		Vector2 operator+(const float f) const;

		/// Componentwise addition between vectors.
		Vector2 operator+(const Vector2& v) const;

		/// Adds each component with {f}.
		void operator+=(const float f);

		/// Componentwise addition between vectors.
		void operator+=(const Vector2& v);

		/// Negates each component.
		Vector2 operator-() const;

		/// Subtracts each component by {f}.
		Vector2 operator-(const float f) const;

		/// Componentwise subtraction between vectors.
		Vector2 operator-(const Vector2& v) const;

		/// Subtracts each component by {f}.
		void operator-=(const float f);

		/// Componentwise subtraction between vectors.
		void operator-=(const Vector2& v);

		/// Multiplies each component by {f}.
		Vector2 operator*(const float f) const;

		/// Componentwise multiplication between vectors.
		Vector2 operator*(const Vector2& v) const;

		/// Multiplies each component by {f}.
		void operator*=(const float f);

		/// Componentwise multiplication between vectors.
		void operator*=(const Vector2& v);

		/// Divides each component by {f}.
		Vector2 operator/(const float f) const;

		/// Componentwise division between vectors.
		Vector2 operator/(const Vector2& v) const;

		/// Divides each component by {f}.
		void operator/=(const float f);

		/// Componentwise division between vectors.
		void operator/=(const Vector2& v);

		/// Calculates the mod of each component with {f}.
		Vector2 operator%(const float f) const;

		/// Componentwise mod between vectors.
		Vector2 operator%(const Vector2& v) const;

		/// Calculates the mod of each component with {f}.
		void operator%=(const float f);

		/// Componentwise mod between vectors.
		void operator%=(const Vector2& v);

		/// Checks if vectors are equal.
		bool operator==(const Vector2& v) const;

		/// Checks if vectors are not equal.
		bool operator!=(const Vector2& v) const;

		bool operator<(const Vector2& v) const;
		bool operator>(const Vector2& v) const;
		bool operator<=(const Vector2& v) const;
		bool operator>=(const Vector2& v) const;

		/// Checks if the vector is full of zeros.
		explicit operator bool() const;

		/// Checks if the vector is not full of zeros.
		bool operator!() const;

		/// Converts the vector to a {Vector3}.
		explicit operator Vector3() const;

		/// Converts the vector to a {Vector4}.
		explicit operator Vector4() const;

		///[Heading] Static functions

		/// Calculates the dot product of two vectors.
		static float Dot(const Vector2& vector1, const Vector2& vector2);

		/// Makes a linear interpolation between two vectors.
		///[Arg] v1: The vector to start at.
		///[Arg] v2: The vector to end at.
		///[Arg] t: The progress of the interpolation.
		///[para] The value should be between {0.0} and {1.0}.
		static Vector2 Lerp(const Vector2& v1, const Vector2& v2, const float t);

		///[Title] Up vector
		static Vector2 Up();

		///[Title] Down vector
		static Vector2 Down();

		///[Title] Left vector
		static Vector2 Left();

		///[Title] Right vector
		static Vector2 Right();
	};

	///[Title] Vector3
	/// A struct for 3D vectors.
	///[Block] Vector3
	struct Vector3 {
	public:
		///[Heading] Components
		
		///[Title] x
		float x;

		///[Title] y
		float y;

		///[Title] z
		float z;

		///[Heading] Constructors

		/// Creates a vector full of zeros.
		Vector3();

		/// Creates a vector with floats.
		Vector3(const float x, const float y, const float z);

		/// Fills the entire vector with the same value.
		explicit Vector3(const float v);

		/// Creates a vector from a {Vector2} and a {float}.
		explicit Vector3(const Vector2& vector, const float z);

		Vector3(const Vector3& vector);

		///[Heading] Methods

		/// Gets the length of the vector.
		float Length() const;

		/// Gets the squared length the vector.
		float SquaredLength() const;

		/// Normalizes the vector and returns the result.
		Vector3 Normalize() const;
	
		///[Heading] Operators

		Vector3 operator=(const Vector3& v);

		/// Adds each component with {f}.
		Vector3 operator+(const float f) const;

		/// Componentwise addition between vectors.
		Vector3 operator+(const Vector3& v) const;

		/// Adds each component with {f}.
		void operator+=(const float f);

		/// Componentwise addition between vectors.
		void operator+=(const Vector3& v);

		/// Negates each component.
		Vector3 operator-() const;

		/// Subtracts each component with {f}.
		Vector3 operator-(const float f) const;

		/// Componentwise subtraction between vectors.
		Vector3 operator-(const Vector3& v) const;

		/// Subtracts each component with {f}.
		void operator-=(const float f);

		/// Componentwise subtraction between vectors.
		void operator-=(const Vector3& v);

		/// Multiplies each component with {f}.
		Vector3 operator*(const float f) const;

		/// Componentwise multiplication between vectors.
		Vector3 operator*(const Vector3& v) const;

		/// Multiplies each component with {f}.
		void operator*=(const float f);

		/// Componentwise multiplication between vectors.
		void operator*=(const Vector3& v);

		/// Divides each component with {f}.
		Vector3 operator/(const float f) const;

		/// Componentwise division between vectors.
		Vector3 operator/(const Vector3& v) const;

		/// Divides each component with {f}.
		void operator/=(const float f);

		/// Componentwise division between vectors.
		void operator/=(const Vector3& v);

		/// Calculates the mod of each component with {f}.
		Vector3 operator%(const float f) const;

		/// Componentwise mod between vectors.
		Vector3 operator%(const Vector3& v) const;

		/// Calculates the mod of each component with {f}.
		void operator%=(const float f);

		/// Componentwise mod between vectors.
		void operator%=(const Vector3& v);

		/// Checks if vectors are equal.
		bool operator==(const Vector3& v) const;

		/// Checks if vectors are not equal.
		bool operator!=(const Vector3& v) const;

		bool operator<(const Vector3& v) const;
		bool operator>(const Vector3& v) const;
		bool operator<=(const Vector3& v) const;
		bool operator>=(const Vector3& v) const;

		/// Checks if the vector is full of zeros.
		explicit operator bool() const;

		/// Checks if the vector is not full of zeros.
		bool operator!() const;

		/// Converts the vector to a {Vector2}.
		explicit operator Vector2() const;

		/// Converts the vector to a {Vector4}.
		explicit operator Vector4() const;

		///[Heading] Static functions

		/// Calculates the dot product of two vectors.
		static float Dot(const Vector3& v1, const Vector3& v2);

		/// Calculates the cross product of two vectors.
		static Vector3 Cross(const Vector3& v1, const Vector3& v2);

		/// Makes a linear interpolation between two vectors.
		///[Arg] v1: The vector to start at.
		///[Arg] v2: The vector to end at.
		///[Arg] t: The progress of the interpolation.
		///[para] The value should be between {0.0} and {1.0}.
		static Vector3 Lerp(const Vector3& v1, const Vector3& v2, const float t);

		///[Title] Up vector
		static Vector3 Up();

		///[Title] Down vector
		static Vector3 Down();

		///[Title] Left vector
		static Vector3 Left();

		///[Title] Right vector
		static Vector3 Right();

		///[Title] Forward vector
		static Vector3 Forward();

		///[Title] Backward vector
		static Vector3 Backward();
	};

	///[Title] Vector4
	/// A struct for 4D vectors.
	///[Block] Vector4
	struct Vector4 {
	public:
		///[Heading] Components

		///[Title] x
		float x;

		///[Title] y
		float y;

		///[Title] z
		float z;

		///[Title] w
		float w;

		///[Heading] Constructors

		/// Creates a vector full of zeros.
		Vector4();

		/// Creates a vector with floats.
		explicit Vector4(const float x, const float y, const float z, const float w);

		/// Fills the entire vector with the same value.
		explicit Vector4(const float v);

		/// Creates a vector from a {Vector2} and two {floats}.
		explicit Vector4(const Vector2& v, const float z, const float w);

		/// Creates a vector from a {Vector3} and a {float}.
		explicit Vector4(const Vector3& v, const float w);

		Vector4(const Vector4& vector);

		///[Heading] Methods

		/// Gets the length of the vector.
		float Length() const;

		/// Gets the squared length the vector.
		float SquaredLength() const;

		/// Normalizes the vector and returns the result.
		Vector4 Normalize() const;

		///[Heading] Operators

		Vector4 operator=(const Vector4& v);

		/// Adds each component with {f}.
		Vector4 operator+(const float f) const;

		/// Componentwise addition between vectors.
		Vector4 operator+(const Vector4& v) const;

		/// Adds each component with {f}.
		void operator+=(const float f);

		/// Componentwise addition between vectors.
		void operator+=(const Vector4& v);

		/// Negates each component.
		Vector4 operator-() const;

		/// Subtracts each component with {f}.
		Vector4 operator-(const float f) const;

		/// Componentwise subtraction between vectors.
		Vector4 operator-(const Vector4& v) const;

		/// Subtracts each component with {f}.
		void operator-=(const float f);

		/// Componentwise subtraction between vectors.
		void operator-=(const Vector4& v);

		/// Multiplies each component with {f}.
		Vector4 operator*(const float f) const;

		/// Componentwise multiplication between vectors.
		Vector4 operator*(const Vector4& v) const;

		/// Multiplies each component with {f}.
		void operator*=(const float f);

		/// Componentwise multiplication between vectors.
		void operator*=(const Vector4& v);

		/// Divides each component with {f}.
		Vector4 operator/(const float f) const;

		/// Componentwise division between vectors.
		Vector4 operator/(const Vector4& v) const;

		/// Divides each component with {f}.
		void operator/=(const float f);

		/// Componentwise division between vectors.
		void operator/=(const Vector4& v);

		/// Calculates the mod of each component with {f}.
		Vector4 operator%(const float f) const;

		/// Componentwise mod between vectors.
		Vector4 operator%(const Vector4& v) const;

		/// Calculates the mod of each component with {f}.
		void operator%=(const float f);

		/// Componentwise mod between vectors.
		void operator%=(const Vector4& v);

		/// Checks if vectors are equal.
		bool operator==(const Vector4& v) const;

		/// Checks if vectors are not equal.
		bool operator!=(const Vector4& v) const;

		bool operator<(const Vector4& v) const;
		bool operator>(const Vector4& v) const;
		bool operator<=(const Vector4& v) const;
		bool operator>=(const Vector4& v) const;

		/// Checks if the vector is full of zeros.
		explicit operator bool() const;

		/// Checks if the vector is not full of zeros.
		bool operator!() const;

		/// Converts the vector to a {Vector2}.
		explicit operator Vector2() const;

		/// Converts the vector to a {Vector3}.
		explicit operator Vector3() const;

		///[Heading] Static functions

		/// Calculates the dot product of two vectors.
		static float Dot(const Vector4& v1, const Vector4& v2);

		/// Makes a linear interpolation between two vectors.
		///[Arg] v1: The vector to start at.
		///[Arg] v2: The vector to end at.
		///[Arg] t: The progress of the interpolation.
		///[para] The value should be between {0.0} and {1.0}.
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
