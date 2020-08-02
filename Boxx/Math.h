#pragma once

#include <stdint.h>
#include "Types.h"
#include <math.h>
#include <cstdlib>

///N Math
namespace Boxx {
	///B Math
	/// Static class for math and number stuff
	class Math {
	public:
		///H Constants

		///T Pi
		static constexpr float Pi();

		///T Minimum and maximum byte values
		/// Constants for the minumum and maximum value 8 bit integers can have
		///M
		static constexpr  Byte  ByteMin();
		static constexpr  Byte  ByteMax();
		static constexpr UByte UByteMin();
		static constexpr UByte UByteMax();
		///M

		///T Minimum and maximum short values
		/// Constants for the minumum and maximum value 16 bit integers can have
		///M
		static constexpr  Short  ShortMin();
		static constexpr  Short  ShortMax();
		static constexpr UShort UShortMin();
		static constexpr UShort UShortMax();
		///M

		///T Minimum and maximum int values
		/// Constants for the minumum and maximum value 32 bit integers can have
		///M
		static constexpr  Int  IntMin();
		static constexpr  Int  IntMax();
		static constexpr UInt UIntMin();
		static constexpr UInt UIntMax();
		///M

		///T Minimum and maximum long values
		/// Constants for the minumum and maximum value 64 bit integers can have
		///M
		static constexpr  Long  LongMin();
		static constexpr  Long  LongMax();
		static constexpr ULong ULongMin();
		static constexpr ULong ULongMax();
		///M

		///T Minimum and maximum float values
		/// Constants for the minumum and maximum value floats can have
		///M
		static constexpr float FloatMin();
		static constexpr float FloatMax();
		///M

		///H General

		///T Square root
		static float Sqrt(const float f) {
			return sqrtf(f);
		}

		///T Absolute value
		static float Abs(const float f) {
			return f < 0 ? -f : f;
		}

		///T Floor
		static float Floor(const float f) {
			return floorf(f);
		}

		///T Ceiling
		static float Ceil(const float f) {
			return ceilf(f);
		}

		///T Round
		static float Round(const float f) {
			return roundf(f);
		}

		///T Power
		static float Pow(const float base, const float exponent) {
			return powf(base, exponent);
		}

		///T Sign
		/// Returns the sign of a number
		///A const T d: The value to return if <code>value</code> is <code>0</code>
		///M
		template <class T>
		static T Sign(const T value, const T d = 0) {
			///M
			if (value < 0)
				return -1;
			if (value > 0)
				return 1;
			return d;
		}

		///H Min and Max

		///T Min
		/// Returns the smallest of the two values
		///M
		template <class T>
		static T Min(const T a, const T b) {
		///M
			return a < b ? a : b;
		}

		///T Min with three values
		/// Returns the smallest of three values
		///M
		template <class T>
		static T Min(const T a, const T b, const T c) {
		///M
			if (a < b)
				return a < c ? a : c;
			else
				return b < c ? b : c;
		}

		///T Max
		/// Returns the largest of the two values
		///M
		template <class T>
		static T Max(const T a, const T b) {
		///M
			return a > b ? a : b;
		}

		///T Max with three values
		/// Returns the largest of three values
		///M
		template <class T>
		static T Max(const T a, const T b, const T c) {
		///M
			if (a > b)
				return a > c ? a : c;
			else
				return b > c ? b : c;
		}

		///T Clamp
		/// Clamps a value between a minimum and maximum value
		///M
		template <class T>
		static T Clamp(const T value, const T min, const T max) {
		///M
			if (value < min)
				return min;
			if (value > max)
				return max;
			return value;
		}

		///H Random

		///T Random seed
		/// Sets the seed for the randomizer
		static void RandomSeed(const UInt seed) {
			Seed() = seed;
		}

		///T Random
		/// Returns a random float value between <code>0.0</code> and <code>1.0</code>
		static float Random() {
			return (float)NextSeed() / (float)UIntMax();
		}

		///T Random int
		/// Returns a random integer value from <code>0</code> to <code>max - 1</code>
		static Int Random(const Int max) {
			return NextSeed() % max;
		}

		///T Random float
		/// Returns a random float value from <code>0.0</code> to <code>max</code>
		static float Random(const float max) {
			return Random() * max;
		}

		///T Random range
		/// Returns a random integer value between <code>min</code> and <code>max</code>
		static Int Random(const Int min, const Int max) {
			return Random(max - min) + min;
		}

		///T Random float range
		/// Returns a random float value between <code>min</code> and <code>max</code>
		static float Random(const float min, const float max) {
			return Random(max - min) + min;
		}

		///T Random sign
		/// Returns either <code>1</code> or <code>-1</code>
		static Int RandomSign() {
			return Random(2) == 1 ? -1 : 1;
		}

		///H Angles and Trigonometry

		///T Radians to degrees
		/// Converts radians to degrees
		static float Deg(const float rad) {
			return rad * 57.2957795131f;
		}

		///T Degrees to radians
		/// Converts degrees to radians
		static float Rad(const float deg) {
			return deg * 0.01745329251f;
		}

		///T Sine
		/// Returns the sine of an angle in radians
		static float Sin(const float rad) {
			return sinf(rad);
		}

		///T Arcsine
		/// Returns the arcsine in radians of a value
		static float ASin(const float f) {
			return asinf(f);
		}

		///T Cosine
		/// Returns the cosine of an angle in radians
		static float Cos(const float rad) {
			return cosf(rad);
		}

		///T Arccosine
		/// Returns the arccosine in radians of a value
		static float ACos(const float f) {
			return acosf(f);
		}

		///T Tangent
		/// Returns the tangent of an angle in radians
		static float Tan(const float rad) {
			return tanf(rad);
		}

		///T Arctangent
		/// Returns the arctangent in radians of a value
		static float ATan(const float f) {
			return atanf(f);
		}

	private:
		static UInt& Seed() {
			static UInt seed = 1;
			return seed;
		}

		static UInt NextSeed() {
			UInt& seed = Seed();
			seed ^= seed << 13;
			seed ^= seed >> 17;
			seed ^= seed << 5;
			return seed;
		}
	};

	inline constexpr float Math::Pi() {return 3.14159265358979f;}

	inline constexpr Byte Math::ByteMin() {return INT8_MIN;}
	inline constexpr Byte Math::ByteMax() {return INT8_MAX;}
	inline constexpr UByte Math::UByteMin() {return 0;}
	inline constexpr UByte Math::UByteMax() {return UINT8_MAX;}
	inline constexpr Short Math::ShortMin() {return INT16_MIN;}
	inline constexpr Short Math::ShortMax() {return INT16_MAX;}
	inline constexpr UShort Math::UShortMin() {return 0;}
	inline constexpr UShort Math::UShortMax() {return UINT16_MAX;}
	inline constexpr Int Math::IntMin() {return INT32_MIN;}
	inline constexpr Int Math::IntMax() {return INT32_MAX;}
	inline constexpr UInt Math::UIntMin() {return 0;}
	inline constexpr UInt Math::UIntMax() {return UINT32_MAX;}
	inline constexpr Long Math::LongMin() {return INT64_MIN;}
	inline constexpr Long Math::LongMax() {return INT64_MAX;}
	inline constexpr ULong Math::ULongMin() {return 0;}
	inline constexpr ULong Math::ULongMax() {return UINT64_MAX;}
	inline constexpr float Math::FloatMin() {return -INFINITY;}
	inline constexpr float Math::FloatMax() {return INFINITY;}
}
