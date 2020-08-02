#pragma once

#include "Vector.h"
#include "Matrix.h"
#include "Types.h"
#include "Math.h"

///N Quaternion
namespace Boxx {
	///B Quaternion
	class Quaternion {
	public:
		///H Constructors

		///T Identity quaternion
		Quaternion();

		///T Components
		/// Creates a quaternion with the specified components
		explicit Quaternion(const float x, const float y, const float z, const float w);

		///T Vector
		/// Creates a quaternion from a vector
		explicit Quaternion(const Vector4& v);

		///H Operators

		///T Arithmetic
		/// Performs arithmetic with quaternions and floats
		///M
		Quaternion operator+(const float f) const;
		Quaternion operator+(const Quaternion& q) const;
		Quaternion operator-() const;
		Quaternion operator-(const float f) const;
		Quaternion operator-(const Quaternion& q) const;
		Quaternion operator*(const float f) const;
		Quaternion operator*(const Quaternion& q) const;
		Quaternion operator/(const float f) const;
		Quaternion operator/(const Quaternion& q) const;
		///M

		void operator*=(const Quaternion& q);

		///H Methods

		///T Length
		/// Gets the length of the quaternion
		float Length() const;

		///T Squared length
		/// Gets the squared length of the quaternion
		float SquaredLength() const;

		///T Normalize
		/// Normalizes the quaternion and returns the result
		Quaternion Normalize() const;

		///T 3x3 rotation matrix
		/// Creates a 3x3 rotation matrix from the quaternion
		Matrix3 RotationMatrix3() const;

		///T 4x4 rotation matrix
		/// Creates a 4x4 rotation matrix from the quaternion
		Matrix4 RotationMatrix4() const;

		///H Static functions

		///T Dot product
		/// Calculates the dot product between two quaternions
		static float Dot(const Quaternion& q1, const Quaternion& q2);

		///T Lerp
		/// Makes a linear interpolation between two quaternions
		///A const Quaternion& q1: The quaternion to start at
		///A const Quaternion& q2: The quaternion to end at
		///A const float t: The progress of the interpolation
		/// The value should be between <code>0.0</code> and <code>1.0</code>
		static Quaternion Lerp(const Quaternion& q1, const Quaternion& q2, const float t);

		///T Slerp
		/// Makes a spherical linear interpolation between two quaternions
		///A Quaternion q1: The quaternion to start at
		///A Quaternion q2: The quaternion to end at
		///A const float t: The progress of the interpolation
		/// The value should be between <code>0.0</code> and <code>1.0</code>
		static Quaternion Slerp(Quaternion q1, Quaternion q2, const float t);

		///T Euler
		/// Creates a quaternion from euler angles
		static Quaternion Euler(const Vector3& euler);

		///T Direction
		/// Creates a quaternion from a direction vector
		///A const Vector3& up: The up vector for the rotation
		static Quaternion Direction(const Vector3& direction, const Vector3& up = Vector3::Up());

		///T Axis angle
		/// Creates a quaternion from an axis and a rotation in radians
		static Quaternion AxisAngle(const Vector3& axis, const float rad);

		///H Components

		///T Components
		/// The components of the quaternion
		float x, y, z, w;
	};

	inline Quaternion::Quaternion() {x = 0.f; y = 0.f; z = 0.f; w = 1.f;}

	inline Quaternion::Quaternion(const float x, const float y, const float z, const float w) {
		this->x = x; this->y = y; this->z = z; this->w = w;
	}

	inline Quaternion::Quaternion(const Vector4& v) {
		x = v.x; y = v.y; z = v.z; w = v.w;
	}

	inline Quaternion Quaternion::operator+(const float f) const {
		return Quaternion(x + f, y + f, z + f, w + f);
	}

	inline Quaternion Quaternion::operator+(const Quaternion& q) const {
		return Quaternion(x + q.x, y + q.y, z + q.z, w + q.w);
	}

	inline Quaternion Quaternion::operator-() const {
		return Quaternion(-x, -y, -z, -w);
	}

	inline Quaternion Quaternion::operator-(const float f) const {
		return Quaternion(x - f, y - f, z - f, w - f);
	}

	inline Quaternion Quaternion::operator-(const Quaternion& q) const {
		return Quaternion(x - q.x, y - q.y, z - q.z, w - q.w);
	}

	inline Quaternion Quaternion::operator*(const float f) const {
		return Quaternion(x * f, y * f, z * f, w * f);
	}

	inline Quaternion Quaternion::operator*(const Quaternion& q) const {
		return Quaternion(
			w * q.x + x * q.w + y * q.z - z * q.y,
			w * q.y + y * q.w + z * q.x - x * q.z,
			w * q.z + z * q.w + x * q.y - y * q.x,
			w * q.w - x * q.x - y * q.y - z * q.z
		);
	}

	inline void Quaternion::operator*=(const Quaternion& q) {
		*this = *this * q;
	}

	inline Quaternion Quaternion::operator/(const float f) const {
		return Quaternion(x / f, y / f, z / f, w / f);
	}

	inline Quaternion Quaternion::operator/(const Quaternion& q) const {
		return Quaternion(x / q.x, y / q.y, z / q.z, w / q.w);
	}

	inline float Quaternion::Length() const {
		return sqrtf(x * x + y * y + z * z + w * w);
	}

	inline float Quaternion::SquaredLength() const {
		return x * x + y * y + z * z + w * w;
	}

	inline Quaternion Quaternion::Normalize() const {
		return *this / Length();
	}

	inline Matrix3 Quaternion::RotationMatrix3() const {
		return Matrix3(
			1.f - 2.f * (y * y + z * z), 2.f * (x * y - z * w), 2.f * (x * z + y * w),
			2.f * (x * y + z * w), 1.f - 2.f * (x * x + z * z), 2.f * (y * z - x * w),
			2.f * (x * z - y * w), 2.f * (y * z + x * w), 1.f - 2.f * (x * x + y * y)
		);
	}

	inline Matrix4 Quaternion::RotationMatrix4() const {
		return Matrix4(
			1.f - 2.f * (y * y + z * z), 2.f * (x * y - z * w), 2.f * (x * z + y * w), 0.f,
			2.f * (x * y + z * w), 1.f - 2.f * (x * x + z * z), 2.f * (y * z - x * w), 0.f,
			2.f * (x * z - y * w), 2.f * (y * z + x * w), 1.f - 2.f * (x * x + y * y), 0.f,
			0.f, 0.f, 0.f, 1.f
		);
	}

	inline float Quaternion::Dot(const Quaternion& q1, const Quaternion& q2) {
		return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
	}

	inline Quaternion Quaternion::Lerp(const Quaternion& q1, const Quaternion& q2, const float t) {
		const float it = 1.f - t;

		if (q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w > 0.f)
			return Quaternion(it * q1.x + t * q2.x, it * q1.y + t * q2.y, it * q1.z + t * q2.z, it * q1.w + t * q2.w).Normalize();
		else
			return Quaternion(it * q1.x - t * q2.x, it * q1.y - t * q2.y, it * q1.z - t * q2.z, it * q1.w - t * q2.w).Normalize();
	}

	inline Quaternion Quaternion::Slerp(Quaternion q1, Quaternion q2, const float t) {
		q1 = q1.Normalize();
		q2 = q2.Normalize();

		float d = Quaternion::Dot(q1, q2);

		if (d < 0.f) {
			q1 = -q1;
			d = -d;
		}

		if (d > 0.99f) {
			return (q1 + (q2 - q1) * t).Normalize();
		}

		const float a1 = Math::ACos(d);
		const float a2 = a1 * t;
		const float s1 = Math::Sin(a1);
		const float s2 = Math::Sin(a2);

		const float c1 = Math::Cos(a2) - d * s2 / s1;
		const float c2 = s2 / s1;

		return q1 * c1 + q2 * c2;
	}

	inline Quaternion Quaternion::Euler(const Vector3& euler) {
		const float cx = Math::Cos(euler.x * 0.5);
		const float sx = Math::Sin(euler.x * 0.5);
		const float cy = Math::Cos(euler.y * 0.5);
		const float sy = Math::Sin(euler.y * 0.5);
		const float cz = Math::Cos(euler.z * 0.5);
		const float sz = Math::Sin(euler.z * 0.5);

		Quaternion q;
		q.w = cz * cy * cx + sz * sy * sx;
		q.x = cz * cy * sx - sz * sy * cx;
		q.y = sz * cy * sx + cz * sy * cx;
		q.z = sz * cy * cx - cz * sy * sx;

		return q;
	}

	inline Quaternion Quaternion::Direction(const Vector3& direction, const Vector3& up) {
		const Vector3 v = Vector3::Cross(up, direction);
		const float dot = Vector3::Dot(up, direction);

		if (Math::Abs(dot) < 1.f)
			return Quaternion(v.x, v.y, v.z, 1.f + dot).Normalize();
		else if (dot >= 1.f)
			return Quaternion(0.f, 0.f, 0.f, 1.f).Normalize();

		return Quaternion(1.f, 0.f, 0.f, 0.f).Normalize();
	}

	inline Quaternion Quaternion::AxisAngle(const Vector3& axis, const float rad) {
		const float s = Math::Sin(rad / 2);

		return Quaternion(
			axis.x * s,
			axis.y * s,
			axis.z * s,
			Math::Cos(rad / 2)
		);
	}

	inline Quaternion operator+(const float f, const Quaternion& q) {
		return q + f;
	}

	inline Quaternion operator-(const float f, const Quaternion& q) {
		return Quaternion(f - q.x, f - q.y, f - q.z, f - q.w);
	}

	inline Quaternion operator*(const float f, const Quaternion& q) {
		return q * f;
	}

	inline Quaternion operator/(const float f, const Quaternion& q) {
		return Quaternion(f / q.x, f / q.y, f / q.z, f / q.w);
	}
}
