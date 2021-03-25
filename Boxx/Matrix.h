#pragma once

#include "Types.h"
#include "Vector.h"
#include "Error.h"
#include "String.h"

///[Settings] block: indent

///[Namespace] Boxx
namespace Boxx {
	struct Matrix2;
	struct Matrix3;
	struct Matrix4;

	///[Heading] Math

	///[Title] Matrix2
	/// A struct for 2x2 matrices.
	///[Block] Matrix2
	struct Matrix2 {
		public:
			///[Heading] Constructors

			/// Creates an identity matrix.
			Matrix2();

			/// Creates a matrix with values.
			///M
			explicit Matrix2(
				const float v1, const float v2,
				const float v3, const float v4
			);
			///M

			Matrix2(const Matrix2& m);

			///[Heading] Methods

			/// Returns the transpose of the matrix.
			Matrix2 Transpose() const;

			/// Returns the inverse of the matrix.
			///[Error] DeterminantIsZeroError: Thrown if the determinant is zero.
			Matrix2 Inverse() const;

			///[Heading] Operators

			/// Get/set the value at a specific position in the matrix.
			///[Code] float operator[][](const UInt row, const UInt column)
			float* operator[](const UInt i);
			const float* operator[](const UInt i) const;

			/// Multiplies the matrix with a vector.
			Vector2 operator*(const Vector2& v) const;

			/// Multiplies the matrix with another matrix.
			Matrix2 operator*(const Matrix2& m) const;

			/// Multiplies the matrix with the inverse of {m}.
			///[Error] DeterminantIsZeroError: Thrown if the determinant of {m} is zero.
			Matrix2 operator/(const Matrix2& m) const;

			///[Heading] Conversions

			/// Converts the matrix to a 3x3 matrix.
			/// The extra values are filled like an identity matrix.
			explicit operator Matrix3() const;

			/// Converts the matrix to a 4x4 matrix.
			/// The extra values are filled like an identity matrix.
			explicit operator Matrix4() const;

			///[Heading] Static functions

			/// Gets a rotation matrix.
			///[Arg] r: Rotation in radians.
			static Matrix2 Rotation(const float r);

		private:
			float values[4];

			friend struct Matrix3;
			friend struct Matrix4;
	};

	///[Title] Matrix3
	/// A struct for 3x3 matrices.
	///[Block] Matrix3
	struct Matrix3 {
		public:
			///[Heading] Constructors

			/// Creates an identity matrix.
			Matrix3();

			/// Create a matrix with values.
			///M
			explicit Matrix3(
				const float v1, const float v2, const float v3,
				const float v4, const float v5, const float v6,
				const float v7, const float v8, const float v9
			);
			///M

			Matrix3(const Matrix3& m);

			///[Heading] Methods

			/// Returns the transpose of the matrix.
			Matrix3 Transpose() const;

			/// Returns the inverse of the matrix.
			///[Error] DeterminantIsZeroError: Thrown if the determinant is zero.
			Matrix3 Inverse() const;

			/// Sets the top left 2x2 sub-matrix to {m}.
			void SetSubMatrix(const Matrix2& m);

			/// Sets the translation part of the matrix.
			void SetTranslation(const Vector2& v);

			/// Gets the translation part of the matrix.
			Vector2 GetTranslation() const;

			/// Sets the scale part of the matrix.
			void SetScale(const Vector2& v);

			/// Gets the scale part of the matrix.
			Vector2 GetScale() const;

			///[Heading] Operators

			/// Get/set the value at a specific position in the matrix.
			///[Code] float operator[][](const UInt row, const UInt column)
			float* operator[](const UInt i);
			const float* operator[](const UInt i) const;

			/// Multiplies the matrix with a vector.
			Vector3 operator*(const Vector3& v) const;

			/// Multiplies the matrix with another matrix.
			Matrix3 operator*(const Matrix3& m) const;

			/// Multiplies the matrix with the inverse of {m}.
			///[Error] DeterminantIsZeroError: Thrown if the determinant of {m} is zero.
			Matrix3 operator/(const Matrix3& m) const;

			///[Heading] Conversions

			/// Converts the matrix to a 2x2 matrix.
			explicit operator Matrix2() const;

			/// Converts the matrix to a 4x4 matrix.
			/// The extra values are filled like an indentity matrix.
			explicit operator Matrix4() const;

			///[Heading] Static functions

			/// Creates a rotation matrix around the x-axis.
			///[Arg] r: Rotation in radians.
			static Matrix3 RotationX(const float r);

			/// Creates a rotation matrix around the y-axis.
			///[Arg] r: Rotation in radians.
			static Matrix3 RotationY(const float r);

			/// Creates a rotation matrix around the z-axis.
			///[Arg] r: Rotation in radians.
			static Matrix3 RotationZ(const float r);

			/// Creates a rotation matrix around an axis.
			///[Arg] r: Rotation in radians.
			///[Arg] axis: The axis to rotate around.
			static Matrix3 Rotation(const float r, const Vector3 &axis);

			/// Creates a translation matrix from a vector.
			static Matrix3 Translation(const Vector2& v);

			/// Creates a scale matrix from a vector.
			static Matrix3 Scale(const Vector2& v);

		private:
			float values[9];

			friend struct Matrix4;
	};

	///[Title] Matrix4
	/// A struct for 4x4 matrices.
	///[Block] Matrix4
	struct Matrix4 {
		public:
			///[Heading] Constructors

			/// Creates an identity matrix.
			Matrix4();

			/// Creates a matrix with values.
			///M
			explicit Matrix4(
				const float  v1, const float  v2, const float  v3, const float  v4,
				const float  v5, const float  v6, const float  v7, const float  v8,
				const float  v9, const float v10, const float v11, const float v12,
				const float v13, const float v14, const float v15, const float v16
			);
			///M
			
			Matrix4(const Matrix4& m);

			///[Heading] Methods

			/// Returns the transpose of the matrix.
			Matrix4 Transpose() const;

			/// Returns the inverse of the matrix.
			///[Error] DeterminantIsZeroError: Thrown if the determinant is zero.
			Matrix4 Inverse() const;

			/// Sets the top left sub-matrix to {m}.
			void SetSubMatrix(const Matrix2& m);

			/// Sets the top left sub-matrix to {m}.
			void SetSubMatrix(const Matrix3& m);

			/// Sets the translation part of the matrix.
			void SetTranslation(const Vector3& v);

			/// Gets the translation part of the matrix.
			Vector3 GetTranslation() const;

			/// Sets the scale part of the matrix.
			void SetScale(const Vector3& v);

			/// Gets the scale part of the matrix.
			Vector3 GetScale() const;

			/// Get/set the value at a specific position in the matrix.
			///[Code] float operator[][](const UInt row, const UInt column)
			float* operator[](const UInt i);
			const float* operator[](const UInt i) const;

			/// Multiplies the matrix with a vector.
			Vector4 operator*(const Vector4& v) const;

			/// Multiplies the matrix with another matrix.
			Matrix4 operator*(const Matrix4& m) const;
			
			/// Multiplies the matrix with the inverse of {m}.
			///[Error] DeterminantIsZeroError: Thrown if the determinant of {m} is zero.
			Matrix4 operator/(const Matrix4& m) const;

			///[Heading] Conversions

			/// Converts the matrix to a 2x2 matrix.
			explicit operator Matrix2() const;

			/// Converts the matrix to a 3x3 matrix.
			explicit operator Matrix3() const;
		
			///[Heading] Static functions

			/// Creates a rotation matrix around the x-axis.
			///[Arg] r: Rotation in radians.
			static Matrix4 RotationX(const float r);

			/// Creates a rotation matrix around the y-axis.
			///[Arg] r: Rotation in radians.
			static Matrix4 RotationY(const float r);

			/// Creates a rotation matrix around the z-axis.
			///[Arg] r: Rotation in radians.
			static Matrix4 RotationZ(const float r);

			/// Creates a rotation matrix around an axis.
			///[Arg] r: Rotation in radians.
			///[Arg] axis: The axis to rotate around.
			static Matrix4 Rotation(const float r, const Vector3 &v);

			/// Creates a translation matrix from a vector.
			static Matrix4 Translation(const Vector3& v);

			/// Creates a scale matrix from a vector.
			static Matrix4 Scale(const Vector3& v);

			/// Creates a perspective matrix.
			///[Arg] fov: The filed of view in radians.
			///[Arg] aspect: The aspect ratio of the view.
			///[Arg] near: The near plane distance.
			///[Arg] far: The far plane distance.
			static Matrix4 Perspective(const float fov, const float aspect, const float near, const float far);

			/// Creates a view matrix.
			///[Arg] lookAt: The point the camera is looking at.
			///[Arg] camera: The position of the camera.
			///[Arg] up: The up vector for the camera.
			static Matrix4 View(const Vector3& lookAt, const Vector3& camera, const Vector3& up = Vector3::Up());

		private:
			float values[16];
	};

	///[Heading] Errors

	///[Title] DeterminantIsZeroError
	/// Thrown if the determinant of a matrix is zero.
	///[Block] DeterminantIsZeroError: Error
	class DeterminantIsZeroError : public Error {
	public: 
		DeterminantIsZeroError() : Error("Determinant is zero") {}

		virtual String Name() const override {
			return "DeterminantIsZeroError";
		}
	};

	inline Matrix2::Matrix2() {values[0] = 1.f; values[1] = 0.f; values[2] = 0.f; values[3] = 1.f;}

	inline Matrix2::Matrix2(const float v1, const float v2, const float v3, const float v4) {values[0] = v1; values[1] = v2; values[2] = v3; values[3] = v4;}

	inline Matrix2::Matrix2(const Matrix2& m) {values[0] = m.values[0]; values[1] = m.values[1]; values[2] = m.values[2]; values[3] = m.values[3];}

	inline float* Matrix2::operator[](const UInt i) {return &values[i * 2];}

	inline const float* Matrix2::operator[](const UInt i) const {
		return &values[i * 2];
	}

	inline Matrix2 Matrix2::operator/(const Matrix2& m) const {return *this * m.Inverse();}

	inline Matrix2 Matrix2::Transpose() const {return Matrix2(values[0], values[2], values[1], values[3]);}
	inline Matrix2 Matrix2::Inverse() const {float det = values[0] * values[3] - values[1] * values[2]; if (det == 0) throw DeterminantIsZeroError(); det = 1.f / det; return Matrix2(values[3] * det, -values[1] * det, -values[2] * det, values[0] * det);}

	inline Matrix2::operator Matrix3() const {return Matrix3(values[0], values[1], 0.f, values[2], values[3], 0.f, 0.f, 0.f, 1.f);}
	inline Matrix2::operator Matrix4() const {return Matrix4(values[0], values[1], 0.f, 0.f, values[2], values[3], 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f);}

	inline Matrix2 Matrix2::Rotation(const float r) {const float s = sinf(r); const float c = cosf(r); return Matrix2(c, s, -s, c);}

	inline Matrix3::Matrix3() {values[0] = 1.f; values[1] = 0.f; values[2] = 0.f; values[3] = 0.f; values[4] = 1.f; values[5] = 0.f; values[6] = 0.f; values[7] = 0.f; values[8] = 1.f;}
	inline Matrix3::Matrix3(const float v1, const float v2, const float v3, const float v4, const float v5, const float v6, const float v7, const float v8, const float v9) {values[0] = v1; values[1] = v2; values[2] = v3; values[3] = v4; values[4] = v5; values[5] = v6; values[6] = v7; values[7] = v8; values[8] = v9;}
	inline Matrix3::Matrix3(const Matrix3& m) {values[0] = m.values[0]; values[1] = m.values[1]; values[2] = m.values[2]; values[3] = m.values[3]; values[4] = m.values[4]; values[5] = m.values[5]; values[6] = m.values[6]; values[7] = m.values[7]; values[8] = m.values[8];}

	inline float* Matrix3::operator[](const UInt i) {return &values[i * 3];}
	inline const float* Matrix3::operator[](const UInt i) const {return &values[i * 3];}

	inline Vector3 Matrix3::operator*(const Vector3& v) const {return Vector3(
		values[0] * v.x + values[1] * v.y + values[2] * v.z,
		values[3] * v.x + values[4] * v.y + values[5] * v.z,
		values[6] * v.x + values[6] * v.y + values[8] * v.z
	);}

	inline Matrix3 Matrix3::operator*(const Matrix3& m) const {return Matrix3(
		values[0] * m.values[0] + values[1] * m.values[3] + values[2] * m.values[6],
		values[0] * m.values[1] + values[1] * m.values[4] + values[2] * m.values[7],
		values[0] * m.values[2] + values[1] * m.values[5] + values[2] * m.values[8],
		values[3] * m.values[0] + values[4] * m.values[3] + values[5] * m.values[6],
		values[3] * m.values[1] + values[4] * m.values[4] + values[5] * m.values[7],
		values[3] * m.values[2] + values[4] * m.values[5] + values[5] * m.values[8],
		values[6] * m.values[0] + values[7] * m.values[3] + values[8] * m.values[6],
		values[6] * m.values[1] + values[7] * m.values[4] + values[8] * m.values[7],
		values[6] * m.values[2] + values[7] * m.values[5] + values[8] * m.values[8]
	);}

	inline Matrix3 Matrix3::operator/(const Matrix3& m) const {return *this * m.Inverse();}

	inline Matrix3 Matrix3::Transpose() const {return Matrix3(values[0], values[3], values[6], values[1], values[4], values[7], values[2], values[5], values[8]);}
	inline Matrix3 Matrix3::Inverse() const {
		float det = values[0] * values[4] * values[8] + values[1] * values[5] * values[6] + values[2] * values[3] * values[7] - values[2] * values[4] * values[6] - values[1] * values[3] * values[8] - values[0] * values[5] * values[7]; 
		if (det == 0) throw DeterminantIsZeroError(); 
		det = 1.f / det;
		return Matrix3(
			(values[4] * values[8] - values[7] * values[5]) * det,
			(values[7] * values[2] - values[1] * values[8]) * det,
			(values[1] * values[5] - values[4] * values[2]) * det,
			(values[6] * values[5] - values[3] * values[8]) * det,
			(values[0] * values[8] - values[6] * values[2]) * det,
			(values[3] * values[2] - values[0] * values[5]) * det,
			(values[3] * values[7] - values[6] * values[4]) * det,
			(values[6] * values[1] - values[0] * values[7]) * det,
			(values[0] * values[4] - values[3] * values[1]) * det
		);
	}

	inline void Matrix3::SetSubMatrix(const Matrix2& m) {
		values[0] = m.values[0];
		values[1] = m.values[1];
		values[3] = m.values[2];
		values[4] = m.values[3];
	}

	inline void Matrix3::SetTranslation(const Vector2& v) {values[2] = v.x; values[5] = v.y;}
	inline Vector2 Matrix3::GetTranslation() const {return Vector2(values[2], values[5]);}

	inline void Matrix3::SetScale(const Vector2& v) {values[0] = v.x; values[4] = v.y;}
	inline Vector2 Matrix3::GetScale() const {return Vector2(values[0], values[4]);}

	inline Matrix3::operator Matrix2() const {return Matrix2(values[0], values[1], values[3], values[4]);}
	inline Matrix3::operator Matrix4() const {return Matrix4(values[0], values[1], values[2], 0.f, values[3], values[4], values[5], 0.f, values[6], values[7], values[8], 0.f, 0.f, 0.f, 0.f, 1.f);}

	inline Matrix3 Matrix3::RotationX(const float r) {const float s = sinf(r); const float c = cosf(r); return Matrix3(1.f, 0.f, 0.f, 0.f, c, s, 0.f, -s, c);}
	inline Matrix3 Matrix3::RotationY(const float r) {const float s = sinf(r); const float c = cosf(r); return Matrix3(c, 0.f, -s, 0.f, 1.f, 0.f, s, 0.f, c);}
	inline Matrix3 Matrix3::RotationZ(const float r) {const float s = sinf(r); const float c = cosf(r); return Matrix3(c, s, 0.f, -s, c, 0.f, 0.f, 0.f, 1.f);}

	inline Matrix3 Matrix3::Rotation(const float r, const Vector3 &v) {
		const float s = sinf(r); const float c1 = cosf(r); const float c2 = 1 - c1; const Vector3 n = v.Normalize(); const float x = -n.x, y = -n.y, z = -n.z;
		return Matrix3(c1 + x * x * c2, x * y * c2 - z * s, x * z * c2 + y * s, y * x * c2 + z * s, c1 + y * y * c2, y * z * c2 - x * s, z * x * c2 - y * s, z * y * c2 + x * s, c1 + z * z * c2);
	}

	inline Matrix3 Matrix3::Translation(const Vector2& v) {return Matrix3(1.f, 0.f, v.x, 0.f, 1.f, v.y, 0.f, 0.f, 1.f);}
	inline Matrix3 Matrix3::Scale(const Vector2& v) {return Matrix3(v.x, 0.f, 0.f, 0.f, v.y, 0.f, 0.f, 0.f, 1.f);}

	inline Matrix4::Matrix4() {values[0] = 1.f; values[1] = 0.f; values[2] = 0.f; values[3] = 0.f; values[4] = 0.f; values[5] = 1.f; values[6] = 0.f; values[7] = 0.f; values[8] = 0.f; values[9] = 0.f; values[10] = 1.f; values[11] = 0.f; values[12] = 0.f; values[13] = 0.f; values[14] = 0.f; values[15] = 1.f;}
	inline Matrix4::Matrix4(const float v1, const float v2, const float v3, const float v4, const float v5, const float v6, const float v7, const float v8, const float v9, const float v10, const float v11, const float v12, const float v13, const float v14, const float v15, const float v16) {values[0] = v1; values[1] = v2; values[2] = v3; values[3] = v4; values[4] = v5; values[5] = v6; values[6] = v7; values[7] = v8; values[8] = v9; values[9] = v10; values[10] = v11; values[11] = v12; values[12] = v13; values[13] = v14; values[14] = v15; values[15] = v16;}
	inline Matrix4::Matrix4(const Matrix4& m) {values[0] = m.values[0]; values[1] = m.values[1]; values[2] = m.values[2]; values[3] = m.values[3]; values[4] = m.values[4]; values[5] = m.values[5]; values[6] = m.values[6]; values[7] = m.values[7]; values[8] = m.values[8]; values[9] = m.values[9]; values[10] = m.values[10]; values[11] = m.values[11]; values[12] = m.values[12]; values[13] = m.values[13]; values[14] = m.values[14]; values[15] = m.values[15];}

	inline float* Matrix4::operator[](const UInt i) {return &values[i * 4];}
	inline const float* Matrix4::operator[](const UInt i) const {return &values[i * 4];}

	inline Vector4 Matrix4::operator*(const Vector4& v) const {return Vector4(
		values[ 0] * v.x + values[ 1] * v.y + values[ 2] * v.z + values[ 3] * v.w,
		values[ 4] * v.x + values[ 5] * v.y + values[ 6] * v.z + values[ 7] * v.w,
		values[ 8] * v.x + values[ 9] * v.y + values[10] * v.z + values[11] * v.w,
		values[12] * v.x + values[13] * v.y + values[14] * v.z + values[15] * v.w
	);}

	inline Matrix4 Matrix4::operator*(const Matrix4& m) const {return Matrix4(
		values[0] * m.values[0] + values[1] * m.values[4] + values[2] * m.values[8] + values[3] * m.values[12],
		values[0] * m.values[1] + values[1] * m.values[5] + values[2] * m.values[9] + values[3] * m.values[13],
		values[0] * m.values[2] + values[1] * m.values[6] + values[2] * m.values[10] + values[3] * m.values[14],
		values[0] * m.values[3] + values[1] * m.values[7] + values[2] * m.values[11] + values[3] * m.values[15],
		values[4] * m.values[0] + values[5] * m.values[4] + values[6] * m.values[8] + values[7] * m.values[12],
		values[4] * m.values[1] + values[5] * m.values[5] + values[6] * m.values[9] + values[7] * m.values[13],
		values[4] * m.values[2] + values[5] * m.values[6] + values[6] * m.values[10] + values[7] * m.values[14],
		values[4] * m.values[3] + values[5] * m.values[7] + values[6] * m.values[11] + values[7] * m.values[15],
		values[8] * m.values[0] + values[9] * m.values[4] + values[10] * m.values[8] + values[11] * m.values[12],
		values[8] * m.values[1] + values[9] * m.values[5] + values[10] * m.values[9] + values[11] * m.values[13],
		values[8] * m.values[2] + values[9] * m.values[6] + values[10] * m.values[10] + values[11] * m.values[14],
		values[8] * m.values[3] + values[9] * m.values[7] + values[10] * m.values[11] + values[11] * m.values[15],
		values[12] * m.values[0] + values[13] * m.values[4] + values[14] * m.values[8] + values[15] * m.values[12],
		values[12] * m.values[1] + values[13] * m.values[5] + values[14] * m.values[9] + values[15] * m.values[13],
		values[12] * m.values[2] + values[13] * m.values[6] + values[14] * m.values[10] + values[15] * m.values[14],
		values[12] * m.values[3] + values[13] * m.values[7] + values[14] * m.values[11] + values[15] * m.values[15]
	);}

	inline Matrix4 Matrix4::operator/(const Matrix4& m) const {return *this * m.Inverse();}

	inline Matrix4 Matrix4::Transpose() const {return Matrix4(values[0], values[4], values[8], values[12], values[1], values[5], values[9], values[13], values[2], values[6], values[10], values[14], values[3], values[7], values[11], values[15]);}
	inline Matrix4 Matrix4::Inverse() const {
		const float det1 = values[ 5] * (values[10] * values[15] - values[14] * values[11]) - values[ 6] * (values[ 9] * values[15] - values[13] * values[11]) + values[ 7] * (values[ 9] * values[14] - values[13] * values[10]);
		const float det2 = values[ 2] * (values[ 9] * values[15] - values[13] * values[11]) - values[ 1] * (values[10] * values[15] - values[14] * values[11]) - values[ 3] * (values[ 9] * values[14] - values[13] * values[10]);
		const float det3 = values[ 1] * (values[ 6] * values[15] - values[14] * values[ 7]) - values[ 2] * (values[ 5] * values[15] - values[13] * values[ 7]) + values[ 3] * (values[ 5] * values[14] - values[13] * values[ 6]);
		const float det4 = values[ 2] * (values[ 5] * values[11] - values[ 9] * values[ 7]) - values[ 1] * (values[ 6] * values[11] - values[10] * values[ 7]) - values[ 3] * (values[ 5] * values[10] - values[ 9] * values[ 6]);

		float det = values[0] * det1 + values[4] * det2 + values[8] * det3 + values[12] * det4; 
		if (det == 0) throw DeterminantIsZeroError(); 
		det = 1.f / det;

		return Matrix4(
			(det1) * det,
			(det2) * det,
			(det3) * det,
			(det4) * det,
					
			(values[ 6] * (values[ 8] * values[15] - values[12] * values[11]) - values[ 4] * (values[10] * values[15] - values[14] * values[11]) - values[ 7] * (values[ 8] * values[14] - values[12] * values[10])) * det,
			(values[ 0] * (values[10] * values[15] - values[14] * values[11]) - values[ 2] * (values[ 8] * values[15] - values[12] * values[11]) + values[ 3] * (values[ 8] * values[14] - values[12] * values[10])) * det,
			(values[ 2] * (values[ 4] * values[15] - values[12] * values[ 7]) - values[ 0] * (values[ 6] * values[15] - values[14] * values[ 7]) - values[ 3] * (values[ 4] * values[14] - values[12] * values[ 6])) * det,
			(values[ 0] * (values[ 6] * values[11] - values[10] * values[ 7]) - values[ 2] * (values[ 4] * values[11] - values[ 8] * values[ 7]) + values[ 3] * (values[ 4] * values[10] - values[ 8] * values[ 6])) * det,

			(values[ 4] * (values[ 9] * values[15] - values[13] * values[11]) - values[ 5] * (values[ 8] * values[15] - values[12] * values[11]) + values[ 7] * (values[ 8] * values[13] - values[12] * values[ 9])) * det,
			(values[ 1] * (values[ 8] * values[15] - values[12] * values[11]) - values[ 0] * (values[ 9] * values[15] - values[13] * values[11]) - values[ 3] * (values[ 8] * values[13] - values[12] * values[ 9])) * det,
			(values[ 0] * (values[ 5] * values[15] - values[13] * values[ 7]) - values[ 1] * (values[ 4] * values[15] - values[12] * values[ 7]) + values[ 3] * (values[ 4] * values[13] - values[12] * values[ 5])) * det,
			(values[ 1] * (values[ 4] * values[11] - values[ 8] * values[ 7]) - values[ 0] * (values[ 5] * values[11] - values[ 9] * values[ 7]) - values[ 3] * (values[ 4] * values[ 9] - values[ 8] * values[ 5])) * det,

			(values[ 5] * (values[ 8] * values[14] - values[12] * values[10]) - values[ 4] * (values[ 9] * values[14] - values[13] * values[10]) - values[ 6] * (values[ 8] * values[13] - values[12] * values[ 9])) * det,
			(values[ 0] * (values[ 9] * values[14] - values[13] * values[10]) - values[ 1] * (values[ 8] * values[14] - values[12] * values[10]) + values[ 2] * (values[ 8] * values[13] - values[12] * values[ 9])) * det,
			(values[ 1] * (values[ 4] * values[14] - values[12] * values[ 6]) - values[ 0] * (values[ 5] * values[14] - values[13] * values[ 6]) - values[ 2] * (values[ 4] * values[13] - values[12] * values[ 5])) * det,
			(values[ 0] * (values[ 5] * values[10] - values[ 9] * values[ 6]) - values[ 1] * (values[ 4] * values[10] - values[ 8] * values[ 6]) + values[ 2] * (values[ 4] * values[ 9] - values[ 8] * values[ 5])) * det
		);
	}

	inline void Matrix4::SetSubMatrix(const Matrix2& m) {
		values[0] = m.values[0];
		values[1] = m.values[1];
		values[4] = m.values[2];
		values[5] = m.values[3];
	}

	inline void Matrix4::SetSubMatrix(const Matrix3& m) {
		values[0] = m.values[0];
		values[1] = m.values[1];
		values[2] = m.values[2];
		values[4] = m.values[3];
		values[5] = m.values[4];
		values[6] = m.values[5];
		values[8] = m.values[6];
		values[9] = m.values[7];
		values[10] = m.values[8];
	}

	inline void Matrix4::SetTranslation(const Vector3& v) {values[3] = v.x; values[7] = v.y; values[11] = v.z;}
	inline Vector3 Matrix4::GetTranslation() const {return Vector3(values[3], values[7], values[11]);}

	inline void Matrix4::SetScale(const Vector3& v) {values[0] = v.x; values[5] = v.y; values[10] = v.z;}
	inline Vector3 Matrix4::GetScale() const {return Vector3(values[0], values[5], values[10]);}

	inline Matrix4::operator Matrix2() const {return Matrix2(values[0], values[1], values[4], values[5]);}
	inline Matrix4::operator Matrix3() const {return Matrix3(values[0], values[1], values[2], values[4], values[5], values[6], values[8], values[9], values[10]);}
		
	inline Matrix4 Matrix4::RotationX(const float r) {const float s = sinf(r); const float c = cosf(r); return Matrix4(1.f, 0.f, 0.f, 0.f, 0.f, c, s, 0.f, 0.f, -s, c, 0.f, 0.f, 0.f, 0.f, 1.f);}
	inline Matrix4 Matrix4::RotationY(const float r) {const float s = sinf(r); const float c = cosf(r); return Matrix4(c, 0.f, -s, 0.f, 0.f, 1.f, 0.f, 0.f, s, 0.f, c, 0.f, 0.f, 0.f, 0.f, 1.f);}
	inline Matrix4 Matrix4::RotationZ(const float r) {const float s = sinf(r); const float c = cosf(r); return Matrix4(c, s, 0.f, 0.f, -s, c, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f);}

	inline Matrix4 Matrix4::Rotation(const float r, const Vector3 &v) {
		const float s = sinf(r); const float c1 = cosf(r); const float c2 = 1 - c1; const Vector3 n = v.Normalize(); const float x = -n.x, y = -n.y, z = -n.z;
		return Matrix4(c1 + x * x * c2, x * y * c2 - z * s, x * z * c2 + y * s, 0.f, y * x * c2 + z * s, c1 + y * y * c2, y * z * c2 - x * s, 0.f, z * x * c2 - y * s, z * y * c2 + x * s, c1 + z * z * c2, 0.f, 0.f, 0.f, 0.f, 1.f);
	}

	inline Matrix4 Matrix4::Translation(const Vector3& v) {return Matrix4(1.f, 0.f, 0.f, v.x, 0.f, 1.f, 0.f, v.y, 0.f, 0.f, 1.f, v.z, 0.f, 0.f, 0.f, 1.f);}
	inline Matrix4 Matrix4::Scale(const Vector3& v) {return Matrix4(v.x, 0.f, 0.f, 0.f, 0.f, v.y, 0.f, 0.f, 0.f, 0.f, v.z, 0.f, 0.f, 0.f, 0.f, 1.f);}

	inline Matrix4 Matrix4::Perspective(const float fov, const float aspect, const float near, const float far) {
		const float h = cosf(fov) / sinf(fov); const float w = h / aspect; const float d = far / (2 * (near - far)); 
		return Matrix4(w, 0.f, 0.f, 0.f, 0.f, h, 0.f, 0.f, 0.f, 0.f, d, d * near, 0.f, 0.f, -1.f, 0.f);
	}

	inline Matrix4 Matrix4::View(const Vector3& lookAt, const Vector3& camera, const Vector3& up) {
		const Vector3 direction = (camera - lookAt).Normalize();
		const Vector3 right = Vector3::Cross(up, direction).Normalize();
		const Vector3 up2 = Vector3::Cross(direction, right);
		const Vector3 c = -camera;

		return Matrix4(
			right.x, right.y, right.z, right.x * c.x + right.y * c.y + right.z * c.z,
			up2.x, up2.y, up2.z, up2.x * c.x + up2.y * c.y + up2.z * c.z,
			direction.x, direction.y, direction.z, direction.x * c.x + direction.y * c.y + direction.z * c.z,
			0.f, 0.f, 0.f, 1.f
		);
	}
}
