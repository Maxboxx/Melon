#pragma once

#include <stdint.h>
#include "Types.h"
#include <math.h>
#include <cstdlib>

///[Settings] block: indent

///[Namespace] Boxx
namespace Boxx {
	///[Heading] Math

	///[Title] Math
	/// Static class for math and number stuff.
	///[Block] Math
	class Math final {
	public:
		Math() = delete;

		///[Heading] Constants

		/// Returns pi.
		static constexpr float Pi();

		/// The minimum value of a byte.
		static constexpr Byte ByteMin();

		/// The maximum value of a byte.
		static constexpr Byte ByteMax();

		/// The minimum value of an unsigned byte.
		static constexpr UByte UByteMin();

		/// The maximum value of an unsigned byte.
		static constexpr UByte UByteMax();

		/// The minimum value of a short.
		static constexpr Short ShortMin();

		/// The maximum value of a short.
		static constexpr Short ShortMax();

		/// The minimum value of an unsigned short.
		static constexpr UShort UShortMin();

		/// The maximum value of an unsigned short.
		static constexpr UShort UShortMax();

		/// The minimum value of an int.
		static constexpr Int IntMin();

		/// The maximum value of an int.
		static constexpr Int IntMax();

		/// The minimum value of an unsigned int.
		static constexpr UInt UIntMin();

		/// The maximum value of an unsigned int.
		static constexpr UInt UIntMax();

		/// The minimum value of a long.
		static constexpr Long LongMin();

		/// The maximum value of a long.
		static constexpr Long LongMax();

		/// The minimum value of an unsigned long.
		static constexpr ULong ULongMin();

		/// The maximum value of an unsigned long.
		static constexpr ULong ULongMax();

		/// The minimum value of a float.
		static constexpr float FloatMin();

		/// The maximum value of a float.
		static constexpr float FloatMax();

		///[Heading] General

		/// Calculates the square root of {f}.
		static float Sqrt(const float f) {
			return sqrtf(f);
		}

		/// Calculates the absolute value of {f}.
		static float Abs(const float f) {
			return f < 0 ? -f : f;
		}

		/// Get the floor of {f}.
		static float Floor(const float f) {
			return floorf(f);
		}

		/// Get the ceiling of {f}.
		static float Ceil(const float f) {
			return ceilf(f);
		}

		/// Rounds the value of {f}.
		static float Round(const float f) {
			return roundf(f);
		}

		/// Calculates {base} to the power of {exponent}.
		static float Pow(const float base, const float exponent) {
			return powf(base, exponent);
		}

		/// Gets the sign of a number.
		///[Returns]
		///[item] {-1} if {value} is negative.
		///[item] {1} if {value} is positive.
		///[item] {0} if {value} is {0}.
		///M
		template <class T>
		static T Sign(const T value) {
		///M
			if (value < 0)
				return -1;
			if (value > 0)
				return 1;
			return 0;
		}

		///[Heading] Min and Max

		/// Returns the smallest of the two values.
		///M
		template <class T>
		static T Min(const T a, const T b) {
		///M
			return a < b ? a : b;
		}

		/// Returns the smallest of three values.
		///M
		template <class T>
		static T Min(const T a, const T b, const T c) {
		///M
			if (a < b)
				return a < c ? a : c;
			else
				return b < c ? b : c;
		}

		/// Returns the largest of the two values.
		///M
		template <class T>
		static T Max(const T a, const T b) {
		///M
			return a > b ? a : b;
		}

		/// Returns the largest of three values.
		///M
		template <class T>
		static T Max(const T a, const T b, const T c) {
		///M
			if (a > b)
				return a > c ? a : c;
			else
				return b > c ? b : c;
		}

		/// Clamps {value} between {min} and {max}.
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

		///[Heading] Random

		/// Sets the seed for the randomizer.
		static void RandomSeed(const UInt seed) {
			Seed() = seed;
		}

		/// Returns a random float value from {0.0} to {1.0}.
		static float Random() {
			return (float)NextSeed() / (float)UIntMax();
		}

		/// Returns a random integer value from {0} to {max - 1}.
		static Int Random(const Int max) {
			return NextSeed() % max;
		}

		/// Returns a random float value from {0.0} to {max}.
		static float Random(const float max) {
			return Random() * max;
		}

		/// Returns a random integer value from {min} to {max}.
		static Int Random(const Int min, const Int max) {
			return Random(max - min) + min;
		}

		/// Returns a random float value from {min} to {max}.
		static float Random(const float min, const float max) {
			return Random(max - min) + min;
		}

		/// Returns either {1} or {-1}.
		static Int RandomSign() {
			return Random(2) == 1 ? -1 : 1;
		}

		///[Heading] Angles and Trigonometry

		/// Converts radians to degrees.
		static float Deg(const float rad) {
			return rad * 57.2957795131f;
		}

		/// Converts degrees to radians.
		static float Rad(const float deg) {
			return deg * 0.01745329251f;
		}

		/// Returns the sine of an angle in radians.
		static float Sin(const float rad) {
			return sinf(rad);
		}

		/// Returns the arcsine in radians of a value.
		static float ASin(const float f) {
			return asinf(f);
		}

		/// Returns the cosine of an angle in radians.
		static float Cos(const float rad) {
			return cosf(rad);
		}

		/// Returns the arccosine in radians of a value.
		static float ACos(const float f) {
			return acosf(f);
		}

		/// Returns the tangent of an angle in radians.
		static float Tan(const float rad) {
			return tanf(rad);
		}

		/// Returns the arctangent in radians of a value.
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
