#pragma once

#include "Types.h"
#include "Vector.h"
#include "Math.h"

///N Color

namespace Boxx {
	struct ColorRGB;
	struct ColorRGBA;
	struct ColorRGBf;
	struct ColorRGBAf;
	struct ColorHSV;
	struct ColorHSVA;
	struct ColorHSVf;
	struct ColorHSVAf;

	///B ColorRGB
	/// Struct for storing an rgb color using bytes
	struct ColorRGB {
		///T Components
		UByte r, g, b;

		///H Constructors

		///T Black Constructor
		ColorRGB();

		///T Component Constructor
		explicit ColorRGB(const UByte r, const UByte g, const UByte b);

		///T Conversion constructors
		///M
		         ColorRGB(const   ColorRGB& color);
		explicit ColorRGB(const  ColorRGBA& color);
		explicit ColorRGB(const  ColorRGBf& color);
		explicit ColorRGB(const ColorRGBAf& color);
		explicit ColorRGB(const   ColorHSV& color);
		explicit ColorRGB(const  ColorHSVA& color);
		explicit ColorRGB(const  ColorHSVf& color);
		explicit ColorRGB(const ColorHSVAf& color);
		///M

		///H Operators

		///T Conversion operators
		///M
		explicit operator  ColorRGBA() const;
		explicit operator  ColorRGBf() const;
		explicit operator ColorRGBAf() const;
		explicit operator   ColorHSV() const;
		explicit operator  ColorHSVA() const;
		explicit operator  ColorHSVf() const;
		explicit operator ColorHSVAf() const;
		///M

		///T Equality operators
		///M
		bool operator==(const ColorRGB& color) const;
		bool operator!=(const ColorRGB& color) const;
		bool operator<(const ColorRGB& color) const;
		bool operator>(const ColorRGB& color) const;
		bool operator<=(const ColorRGB& color) const;
		bool operator>=(const ColorRGB& color) const;
		///M

	private:
		friend struct ColorRGBA;
		friend struct ColorRGBf;
		friend struct ColorRGBAf;
		friend struct ColorHSV;
		friend struct ColorHSVA;
		friend struct ColorHSVf;
		friend struct ColorHSVAf;

		static ColorHSV  RGBToHSV(UByte r, UByte g, UByte b);
		static ColorRGB  HSVToRGB(Short h, UByte s, UByte v);
		static ColorHSVf RGBToHSV(float r, float g, float b);
		static ColorRGBf HSVToRGB(float h, float s, float v);
	};

	///B ColorRGBA
	/// Struct for storing an rgba color using bytes
	struct ColorRGBA {
		///T Components
		UByte r, g, b, a;

		///H Constructors

		///T Black Constructor
		ColorRGBA();

		///T Component Constructor
		explicit ColorRGBA(const UByte r, const UByte g, const UByte b, const UByte a = 255);

		///T Conversion constructors
		///M
		explicit ColorRGBA(const   ColorRGB& color);
		         ColorRGBA(const  ColorRGBA& color);
		explicit ColorRGBA(const  ColorRGBf& color);
		explicit ColorRGBA(const ColorRGBAf& color);
		explicit ColorRGBA(const   ColorHSV& color);
		explicit ColorRGBA(const  ColorHSVA& color);
		explicit ColorRGBA(const  ColorHSVf& color);
		explicit ColorRGBA(const ColorHSVAf& color);
		///M

		///H Operators

		///T Conversion operators
		///M
		explicit operator   ColorRGB() const;
		explicit operator  ColorRGBf() const;
		explicit operator ColorRGBAf() const;
		explicit operator   ColorHSV() const;
		explicit operator  ColorHSVA() const;
		explicit operator  ColorHSVf() const;
		explicit operator ColorHSVAf() const;
		///M

		///T Equality operators
		///M
		bool operator==(const ColorRGBA& color) const;
		bool operator!=(const ColorRGBA& color) const;
		bool operator<(const ColorRGBA& color) const;
		bool operator>(const ColorRGBA& color) const;
		bool operator<=(const ColorRGBA& color) const;
		bool operator>=(const ColorRGBA& color) const;
		///M
	};

	///B ColorRGBf
	/// Struct for storing an rgb color using floats
	struct ColorRGBf {
		///T Components
		float r, g, b;

		///H Constructors

		///T Black Constructor
		ColorRGBf();

		///T Component Constructor
		explicit ColorRGBf(const float r, const float g, const float b);

		///T Vector Constructor
		ColorRGBf(const Vector3& vector);

		///T Conversion constructors
		///M
		explicit ColorRGBf(const   ColorRGB& color);
		explicit ColorRGBf(const  ColorRGBA& color);
		         ColorRGBf(const  ColorRGBf& color);
		explicit ColorRGBf(const ColorRGBAf& color);
		explicit ColorRGBf(const   ColorHSV& color);
		explicit ColorRGBf(const  ColorHSVA& color);
		explicit ColorRGBf(const  ColorHSVf& color);
		explicit ColorRGBf(const ColorHSVAf& color);
		///M

		///H Operators

		///T Conversion operators
		///M
		explicit operator   ColorRGB() const;
		explicit operator  ColorRGBA() const;
		explicit operator ColorRGBAf() const;
		explicit operator   ColorHSV() const;
		explicit operator  ColorHSVA() const;
		explicit operator  ColorHSVf() const;
		explicit operator ColorHSVAf() const;
		///M

		///T Equality operators
		///M
		bool operator==(const ColorRGBf& color) const;
		bool operator!=(const ColorRGBf& color) const;
		bool operator<(const ColorRGBf& color) const;
		bool operator>(const ColorRGBf& color) const;
		bool operator<=(const ColorRGBf& color) const;
		bool operator>=(const ColorRGBf& color) const;
		///M
	};

	///B ColorRGBAf
	/// Struct for storing an rgba color using floats
	struct ColorRGBAf {
		///T Components
		float r, g, b, a;
		
		///H Constructors

		///T Black Constructor
		ColorRGBAf();

		///T Component Constructor
		explicit ColorRGBAf(const float r, const float g, const float b, const float a = 1.f);

		///T Vector Constructors
		///M
		ColorRGBAf(const Vector3& vector);
		ColorRGBAf(const Vector4& vector);
		///M

		///T Conversion constructors
		///M
		explicit ColorRGBAf(const   ColorRGB& color);
		explicit ColorRGBAf(const  ColorRGBA& color);
		explicit ColorRGBAf(const  ColorRGBf& color);
		         ColorRGBAf(const ColorRGBAf& color);
		explicit ColorRGBAf(const   ColorHSV& color);
		explicit ColorRGBAf(const  ColorHSVA& color);
		explicit ColorRGBAf(const  ColorHSVf& color);
		explicit ColorRGBAf(const ColorHSVAf& color);
		///M

		///H Operators

		///T Conversion operators
		///M
		explicit operator   ColorRGB() const;
		explicit operator  ColorRGBA() const;
		explicit operator  ColorRGBf() const;
		explicit operator   ColorHSV() const;
		explicit operator  ColorHSVA() const;
		explicit operator  ColorHSVf() const;
		explicit operator ColorHSVAf() const;
		///M

		///T Equality operators
		///M
		bool operator==(const ColorRGBAf& color) const;
		bool operator!=(const ColorRGBAf& color) const;
		bool operator<(const ColorRGBAf& color) const;
		bool operator>(const ColorRGBAf& color) const;
		bool operator<=(const ColorRGBAf& color) const;
		bool operator>=(const ColorRGBAf& color) const;
		///M
	};

	///B ColorHSV
	/// Struct for storing an hsv color using bytes
	struct ColorHSV {
		///T Components
		///M
		Short h;
		UByte s, v;
		///M
		
		///H Constructors

		///T Black Constructor
		ColorHSV();

		///T Component Constructor
		explicit ColorHSV(const Short h, const UByte s, const UByte v);

		///T Conversion constructors
		///M
		explicit ColorHSV(const   ColorRGB& color);
		explicit ColorHSV(const  ColorRGBA& color);
		explicit ColorHSV(const  ColorRGBf& color);
		explicit ColorHSV(const ColorRGBAf& color);
		         ColorHSV(const   ColorHSV& color);
		explicit ColorHSV(const  ColorHSVA& color);
		explicit ColorHSV(const  ColorHSVf& color);
		explicit ColorHSV(const ColorHSVAf& color);
		///M

		///H Operators

		///T Conversion operators
		///M
		explicit operator   ColorRGB() const;
		explicit operator  ColorRGBA() const;
		explicit operator  ColorRGBf() const;
		explicit operator ColorRGBAf() const;
		explicit operator  ColorHSVA() const;
		explicit operator  ColorHSVf() const;
		explicit operator ColorHSVAf() const;
		///M

		///T Equality operators
		///M
		bool operator==(const ColorHSV& color) const;
		bool operator!=(const ColorHSV& color) const;
		bool operator<(const ColorHSV& color) const;
		bool operator>(const ColorHSV& color) const;
		bool operator<=(const ColorHSV& color) const;
		bool operator>=(const ColorHSV& color) const;
		///M
	};

	///B ColorHSVA
	/// Struct for storing an hsva color using bytes
	struct ColorHSVA {
		///T Components
		///M
		Short h;
		UByte s, v, a;
		///M
		
		///H Constructors

		///T Black Constructor
		ColorHSVA();

		///T Component Constructor
		explicit ColorHSVA(const Short h, const UByte s, const UByte v, const UByte a = 255);

		///T Conversion constructors
		///M
		explicit ColorHSVA(const   ColorRGB& color);
		explicit ColorHSVA(const  ColorRGBA& color);
		explicit ColorHSVA(const  ColorRGBf& color);
		explicit ColorHSVA(const ColorRGBAf& color);
		explicit ColorHSVA(const   ColorHSV& color);
		         ColorHSVA(const  ColorHSVA& color);
		explicit ColorHSVA(const  ColorHSVf& color);
		explicit ColorHSVA(const ColorHSVAf& color);
		///M

		///H Operators

		///T Conversion operators
		///M
		explicit operator   ColorRGB() const;
		explicit operator  ColorRGBA() const;
		explicit operator  ColorRGBf() const;
		explicit operator ColorRGBAf() const;
		explicit operator   ColorHSV() const;
		explicit operator  ColorHSVf() const;
		explicit operator ColorHSVAf() const;
		///M

		///T Equality operators
		///M
		bool operator==(const ColorHSVA& color) const;
		bool operator!=(const ColorHSVA& color) const;
		bool operator<(const ColorHSVA& color) const;
		bool operator>(const ColorHSVA& color) const;
		bool operator<=(const ColorHSVA& color) const;
		bool operator>=(const ColorHSVA& color) const;
		///M
	};

	///B ColorHSVf
	/// Struct for storing an hsv color using floats
	struct ColorHSVf {
		///T Components
		float h, s, v;
		
		///H Constructors

		///T Black Constructor
		ColorHSVf();

		///T Component Constructor
		explicit ColorHSVf(const float h, const float s, const float v);

		///T Conversion constructors
		///M
		explicit ColorHSVf(const   ColorRGB& color);
		explicit ColorHSVf(const  ColorRGBA& color);
		explicit ColorHSVf(const  ColorRGBf& color);
		explicit ColorHSVf(const ColorRGBAf& color);
		explicit ColorHSVf(const   ColorHSV& color);
		explicit ColorHSVf(const  ColorHSVA& color);
		         ColorHSVf(const  ColorHSVf& color);
		explicit ColorHSVf(const ColorHSVAf& color);
		///M

		///H Operators

		///T Conversion operators
		///M
		explicit operator   ColorRGB() const;
		explicit operator  ColorRGBA() const;
		explicit operator  ColorRGBf() const;
		explicit operator ColorRGBAf() const;
		explicit operator   ColorHSV() const;
		explicit operator  ColorHSVA() const;
		explicit operator ColorHSVAf() const;
		///M

		///T Equality operators
		///M
		bool operator==(const ColorHSVf& color) const;
		bool operator!=(const ColorHSVf& color) const;
		bool operator<(const ColorHSVf& color) const;
		bool operator>(const ColorHSVf& color) const;
		bool operator<=(const ColorHSVf& color) const;
		bool operator>=(const ColorHSVf& color) const;
		///M
	};

	///B ColorHSVAf
	/// Struct for storing an hsva color using floats
	struct ColorHSVAf {
		///T Components
		float h, s, v, a;
		
		///H Constructors

		///T Black Constructor
		ColorHSVAf();

		///T Component Constructor
		explicit ColorHSVAf(const float r, const float g, const float b, const float a = 1.f);

		///T Conversion constructors
		///M
		explicit ColorHSVAf(const   ColorRGB& color);
		explicit ColorHSVAf(const  ColorRGBA& color);
		explicit ColorHSVAf(const  ColorRGBf& color);
		explicit ColorHSVAf(const ColorRGBAf& color);
		explicit ColorHSVAf(const   ColorHSV& color);
		explicit ColorHSVAf(const  ColorHSVA& color);
		explicit ColorHSVAf(const  ColorHSVf& color);
		         ColorHSVAf(const ColorHSVAf& color);
		///M

		///H Operators

		///T Conversion operators
		///M
		explicit operator   ColorRGB() const;
		explicit operator  ColorRGBA() const;
		explicit operator  ColorRGBf() const;
		explicit operator ColorRGBAf() const;
		explicit operator   ColorHSV() const;
		explicit operator  ColorHSVA() const;
		explicit operator  ColorHSVf() const;
		///M

		///T Equality operators
		///M
		bool operator==(const ColorHSVAf& color) const;
		bool operator!=(const ColorHSVAf& color) const;
		bool operator<(const ColorHSVAf& color) const;
		bool operator>(const ColorHSVAf& color) const;
		bool operator<=(const ColorHSVAf& color) const;
		bool operator>=(const ColorHSVAf& color) const;
		///M
	};

	inline ColorRGB::ColorRGB() {
		r = 0;
		g = 0;
		b = 0;
	}

	inline ColorRGB::ColorRGB(const UByte r, const UByte g, const UByte b) {
		this->r = r;
		this->g = g;
		this->b = b;
	}

	inline ColorRGB::ColorRGB(const ColorRGB& color) {
		r = color.r;
		g = color.g;
		b = color.b;
	}

	inline ColorRGB::ColorRGB(const ColorRGBA& color) {
		r = color.r;
		g = color.g;
		b = color.b;
	}

	inline ColorRGB::ColorRGB(const ColorRGBf& color) {
		r = (UByte)(color.r * 255);
		g = (UByte)(color.g * 255);
		b = (UByte)(color.b * 255);
	}

	inline ColorRGB::ColorRGB(const ColorRGBAf& color) {
		r = (UByte)(color.r * 255);
		g = (UByte)(color.g * 255);
		b = (UByte)(color.b * 255);
	}

	inline ColorRGB::ColorRGB(const ColorHSV& color) {
		ColorRGB c = ColorRGB::HSVToRGB(color.h, color.s, color.v);
		r = c.r;
		g = c.g;
		b = c.b;
	}

	inline ColorRGB::ColorRGB(const ColorHSVA& color) {
		ColorRGB c = ColorRGB::HSVToRGB(color.h, color.s, color.v);
		r = c.r;
		g = c.g;
		b = c.b;
	}

	inline ColorRGB::ColorRGB(const ColorHSVf& color) {
		ColorRGBf c = ColorRGB::HSVToRGB(color.h, color.s, color.v);
		r = (UByte)(c.r * 255);
		g = (UByte)(c.g * 255);
		b = (UByte)(c.b * 255);
	}

	inline ColorRGB::ColorRGB(const ColorHSVAf& color) {
		ColorRGBf c = ColorRGB::HSVToRGB(color.h, color.s, color.v);
		r = (UByte)(c.r * 255);
		g = (UByte)(c.g * 255);
		b = (UByte)(c.b * 255);
	}

	inline ColorRGB::operator ColorRGBA() const {
		return ColorRGBA(*this);
	}

	inline ColorRGB::operator ColorRGBf() const {
		return ColorRGBf(*this);
	}

	inline ColorRGB::operator ColorRGBAf() const {
		return ColorRGBAf(*this);
	}

	inline ColorRGB::operator ColorHSV() const {
		return ColorHSV(*this);
	}

	inline ColorRGB::operator ColorHSVA() const {
		return ColorHSVA(*this);
	}

	inline ColorRGB::operator ColorHSVf() const {
		return ColorHSVf(*this);
	}

	inline ColorRGB::operator ColorHSVAf() const {
		return ColorHSVAf(*this);
	}

	inline bool ColorRGB::operator==(const ColorRGB& color) const {
		return r == color.r && g == color.g && b == color.b;
	}

	inline bool ColorRGB::operator!=(const ColorRGB& color) const {
		return r != color.r || g != color.g || b != color.b;
	}

	inline bool ColorRGB::operator<(const ColorRGB& color) const {
		if (r < color.r) return true;
		else if (r > color.r) return false;

		if (g < color.g) return true;
		else if (g > color.g) return false;

		if (b < color.b) return true;

		return false;
	}
	
	inline bool ColorRGB::operator>(const ColorRGB& color) const {
		if (r > color.r) return true;
		else if (r < color.r) return false;

		if (g > color.g) return true;
		else if (g < color.g) return false;

		if (b > color.b) return true;

		return false;
	}

	inline bool ColorRGB::operator<=(const ColorRGB& color) const {
		return *this == color || *this < color;
	}

	inline bool ColorRGB::operator>=(const ColorRGB& color) const {
		return *this == color || *this > color;
	}

	inline ColorHSV ColorRGB::RGBToHSV(UByte r, UByte g, UByte b) {
		ColorHSVf c = RGBToHSV(r / 255.f, g / 255.f, b / 255.f);
		return ColorHSV((Short)(c.h * 360), (UByte)(c.s * 100), (UByte)(c.v * 255));
	}

	inline ColorRGB ColorRGB::HSVToRGB(Short h, UByte s, UByte v) {
		ColorRGBf c = HSVToRGB(h / 360.f, s / 100.f, v / 100.f);
		return ColorRGB((UByte)(c.r * 255), (UByte)(c.g * 255), (UByte)(c.b * 255));
	}

	inline ColorHSVf ColorRGB::RGBToHSV(float r, float g, float b) {
		float min = Math::Min(r, g, b);
		float max = Math::Max(r, g, b);

		float h = 0.f, s = 0.f, v = 0.f;
		v = max;

		float d = max - min;
	
		if (max == 0.f) {
			s = 0.f;
		}
		else {
			s = d / max;
		}

		if (max == min) {
			h = 0.f;
		}
		else {
			if (max == r) {
				h = (g - b) / d;
			
				if (g < b) { 
					h += 6;
				}
			}
			else if (max == g) { 
				h = (b - r) / d + 2;
			}
			else if (max == b) { 
				h = (r - g) / d + 4;
			}
		
			h /= 6;
		}

		return ColorHSVf(h, s, v);
	}

	inline ColorRGBf ColorRGB::HSVToRGB(float h, float s, float v) {
		float r = 0.f, g = 0.f, b = 0.f;
	
		while (h < 0.f) h = h + 1.f;
		while (h >= 1.f) h = h - 1.f;
	
		Int i = (Int)Math::Floor(h * 6);
		float f = h * 6 - i;
		float p = v * (1 - s);
		float q = v * (1 - f * s);
		float t = v * (1 - (1 - f) * s);
	
		i = i % 6;
	
		if (i == 0) {
			r = v;
			g = t;
			b = p;
		}
		else if (i == 1) {
			r = q;
			g = v;
			b = p;
		}
		else if (i == 2) {
			r = p;
			g = v;
			b = t;
		}
		else if (i == 3) { 
			r, g, b = p, q, v;
			r = p;
			g = q;
			b = v;
		}
		else if (i == 4) { 
			r = t;
			g = p;
			b = v;
		}
		else if (i == 5) {
			r = v;
			g = p;
			b = q;
		}
		
		return ColorRGBf(r, g, b);
	}

	inline ColorRGBA::ColorRGBA() {
		r = 0;
		g = 0;
		b = 0;
		a = 255;
	}

	inline ColorRGBA::ColorRGBA(const UByte r, const UByte g, const UByte b, const UByte a) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	inline ColorRGBA::ColorRGBA(const ColorRGB& color) {
		r = color.r;
		g = color.g;
		b = color.b;
		a = 255;
	}

	inline ColorRGBA::ColorRGBA(const ColorRGBA& color) {
		r = color.r;
		g = color.g;
		b = color.b;
		a = color.a;
	}

	inline ColorRGBA::ColorRGBA(const ColorRGBf& color) {
		r = (UByte)(color.r * 255);
		g = (UByte)(color.g * 255);
		b = (UByte)(color.b * 255);
		a = 255;
	}

	inline ColorRGBA::ColorRGBA(const ColorRGBAf& color) {
		r = (UByte)(color.r * 255);
		g = (UByte)(color.g * 255);
		b = (UByte)(color.b * 255);
		a = (UByte)(color.a * 255);
	}

	inline ColorRGBA::ColorRGBA(const ColorHSV& color) {
		ColorRGB c = ColorRGB::HSVToRGB(color.h, color.s, color.v);
		r = c.r;
		g = c.g;
		b = c.b;
		a = 255;
	}

	inline ColorRGBA::ColorRGBA(const ColorHSVA& color) {
		ColorRGB c = ColorRGB::HSVToRGB(color.h, color.s, color.v);
		r = c.r;
		g = c.g;
		b = c.b;
		a = color.a;
	}

	inline ColorRGBA::ColorRGBA(const ColorHSVf& color) {
		ColorRGBf c = ColorRGB::HSVToRGB(color.h, color.s, color.v);
		r = (UByte)(c.r * 255);
		g = (UByte)(c.g * 255);
		b = (UByte)(c.b * 255);
		a = 255;
	}

	inline ColorRGBA::ColorRGBA(const ColorHSVAf& color) {
		ColorRGBf c = ColorRGB::HSVToRGB(color.h, color.s, color.v);
		r = (UByte)(c.r * 255);
		g = (UByte)(c.g * 255);
		b = (UByte)(c.b * 255);
		a = (UByte)(color.a * 255);
	}

	inline ColorRGBA::operator ColorRGB() const {
		return ColorRGB(*this);
	}

	inline ColorRGBA::operator ColorRGBf() const {
		return ColorRGBf(*this);
	}

	inline ColorRGBA::operator ColorRGBAf() const {
		return ColorRGBAf(*this);
	}

	inline ColorRGBA::operator ColorHSV() const {
		return ColorHSV(*this);
	}

	inline ColorRGBA::operator ColorHSVA() const {
		return ColorHSVA(*this);
	}

	inline ColorRGBA::operator ColorHSVf() const {
		return ColorHSVf(*this);
	}

	inline ColorRGBA::operator ColorHSVAf() const {
		return ColorHSVAf(*this);
	}

	inline bool ColorRGBA::operator==(const ColorRGBA& color) const {
		return r == color.r && g == color.g && b == color.b && a == color.a;
	}

	inline bool ColorRGBA::operator!=(const ColorRGBA& color) const {
		return r != color.r || g != color.g || b != color.b || a != color.a;
	}

	inline bool ColorRGBA::operator<(const ColorRGBA& color) const {
		if (r < color.r) return true;
		else if (r > color.r) return false;

		if (g < color.g) return true;
		else if (g > color.g) return false;

		if (b < color.b) return true;
		else if (b > color.b) return false;

		if (a < color.a) return true;

		return false;
	}
	
	inline bool ColorRGBA::operator>(const ColorRGBA& color) const {
		if (r > color.r) return true;
		else if (r < color.r) return false;

		if (g > color.g) return true;
		else if (g < color.g) return false;

		if (b > color.b) return true;
		else if (b < color.b) return false;

		if (a > color.a) return true;

		return false;
	}

	inline bool ColorRGBA::operator<=(const ColorRGBA& color) const {
		return *this == color || *this < color;
	}

	inline bool ColorRGBA::operator>=(const ColorRGBA& color) const {
		return *this == color || *this > color;
	}

	inline ColorRGBf::ColorRGBf() {
		r = 0.f;
		g = 0.f;
		b = 0.f;
	}

	inline ColorRGBf::ColorRGBf(const float r, const float g, const float b) {
		this->r = r;
		this->g = g;
		this->b = b;
	}

	inline ColorRGBf::ColorRGBf(const Vector3& vector) {
		r = vector.x;
		g = vector.y;
		b = vector.z;
	}

	inline ColorRGBf::ColorRGBf(const ColorRGB& color) {
		r = color.r / 255.f;
		g = color.g / 255.f;
		b = color.b / 255.f;
	}

	inline ColorRGBf::ColorRGBf(const ColorRGBA& color) {
		r = color.r / 255.f;
		g = color.g / 255.f;
		b = color.b / 255.f;
	}

	inline ColorRGBf::ColorRGBf(const ColorRGBf& color) {
		r = color.r;
		g = color.g;
		b = color.b;
	}

	inline ColorRGBf::ColorRGBf(const ColorRGBAf& color) {
		r = color.r;
		g = color.g;
		b = color.b;
	}

	inline ColorRGBf::ColorRGBf(const ColorHSV& color) {
		ColorRGBf c = ColorRGB::HSVToRGB(color.h / 360.f, color.s / 100.f, color.v / 100.f);
		r = c.r;
		g = c.g;
		b = c.b;
	}

	inline ColorRGBf::ColorRGBf(const ColorHSVA& color) {
		ColorRGBf c = ColorRGB::HSVToRGB(color.h / 360.f, color.s / 100.f, color.v / 100.f);
		r = c.r;
		g = c.g;
		b = c.b;
	}

	inline ColorRGBf::ColorRGBf(const ColorHSVf& color) {
		ColorRGBf c = ColorRGB::HSVToRGB(color.h, color.s, color.v);
		r = c.r;
		g = c.g;
		b = c.b;
	}

	inline ColorRGBf::ColorRGBf(const ColorHSVAf& color) {
		ColorRGBf c = ColorRGB::HSVToRGB(color.h, color.s, color.v);
		r = c.r;
		g = c.g;
		b = c.b;
	}

	inline ColorRGBf::operator ColorRGB() const {
		return ColorRGB(*this);
	}

	inline ColorRGBf::operator ColorRGBA() const {
		return ColorRGBA(*this);
	}

	inline ColorRGBf::operator ColorRGBAf() const {
		return ColorRGBAf(*this);
	}

	inline ColorRGBf::operator ColorHSV() const {
		return ColorHSV(*this);
	}

	inline ColorRGBf::operator ColorHSVA() const {
		return ColorHSVA(*this);
	}

	inline ColorRGBf::operator ColorHSVf() const {
		return ColorHSVf(*this);
	}

	inline ColorRGBf::operator ColorHSVAf() const {
		return ColorHSVAf(*this);
	}

	inline bool ColorRGBf::operator==(const ColorRGBf& color) const {
		return r == color.r && g == color.g && b == color.b;
	}

	inline bool ColorRGBf::operator!=(const ColorRGBf& color) const {
		return r != color.r || g != color.g || b != color.b;
	}

	inline bool ColorRGBf::operator<(const ColorRGBf& color) const {
		if (r < color.r) return true;
		else if (r > color.r) return false;

		if (g < color.g) return true;
		else if (g > color.g) return false;

		if (b < color.b) return true;

		return false;
	}
	
	inline bool ColorRGBf::operator>(const ColorRGBf& color) const {
		if (r > color.r) return true;
		else if (r < color.r) return false;

		if (g > color.g) return true;
		else if (g < color.g) return false;

		if (b > color.b) return true;

		return false;
	}

	inline bool ColorRGBf::operator<=(const ColorRGBf& color) const {
		return *this == color || *this < color;
	}

	inline bool ColorRGBf::operator>=(const ColorRGBf& color) const {
		return *this == color || *this > color;
	}

	inline ColorRGBAf::ColorRGBAf() {
		r = 0.f;
		g = 0.f;
		b = 0.f;
		a = 1.f;
	}

	inline ColorRGBAf::ColorRGBAf(const float r, const float g, const float b, const float a) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	inline ColorRGBAf::ColorRGBAf(const Vector3& vector) {
		r = vector.x;
		g = vector.y;
		b = vector.z;
		a = 1.f;
	}

	inline ColorRGBAf::ColorRGBAf(const Vector4& vector) {
		r = vector.x;
		g = vector.y;
		b = vector.z;
		a = vector.w;
	}

	inline ColorRGBAf::ColorRGBAf(const ColorRGB& color) {
		r = color.r / 255.f;
		g = color.g / 255.f;
		b = color.b / 255.f;
		a = 1.f;
	}

	inline ColorRGBAf::ColorRGBAf(const ColorRGBA& color) {
		r = color.r / 255.f;
		g = color.g / 255.f;
		b = color.b / 255.f;
		a = color.a / 255.f;
	}

	inline ColorRGBAf::ColorRGBAf(const ColorRGBf& color) {
		r = color.r;
		g = color.g;
		b = color.b;
		a = 1.f;
	}

	inline ColorRGBAf::ColorRGBAf(const ColorRGBAf& color) {
		r = color.r;
		g = color.g;
		b = color.b;
		a = color.a;
	}

	inline ColorRGBAf::ColorRGBAf(const ColorHSV& color) {
		ColorRGBf c = ColorRGB::HSVToRGB(color.h / 360.f, color.s / 100.f, color.v / 100.f);
		r = c.r;
		g = c.g;
		b = c.b;
		a = 1.f;
	}

	inline ColorRGBAf::ColorRGBAf(const ColorHSVA& color) {
		ColorRGBf c = ColorRGB::HSVToRGB(color.h / 360.f, color.s / 100.f, color.v / 100.f);
		r = c.r;
		g = c.g;
		b = c.b;
		a = color.a / 255.f;
	}

	inline ColorRGBAf::ColorRGBAf(const ColorHSVf& color) {
		ColorRGBf c = ColorRGB::HSVToRGB(color.h, color.s, color.v);
		r = c.r;
		g = c.g;
		b = c.b;
		a = 1.f;
	}

	inline ColorRGBAf::ColorRGBAf(const ColorHSVAf& color) {
		ColorRGBf c = ColorRGB::HSVToRGB(color.h, color.s, color.v);
		r = c.r;
		g = c.g;
		b = c.b;
		a = color.a;
	}

	inline ColorRGBAf::operator ColorRGB() const {
		return ColorRGB(*this);
	}

	inline ColorRGBAf::operator ColorRGBA() const {
		return ColorRGBA(*this);
	}

	inline ColorRGBAf::operator ColorRGBf() const {
		return ColorRGBf(*this);
	}

	inline ColorRGBAf::operator ColorHSV() const {
		return ColorHSV(*this);
	}

	inline ColorRGBAf::operator ColorHSVA() const {
		return ColorHSVA(*this);
	}

	inline ColorRGBAf::operator ColorHSVf() const {
		return ColorHSVf(*this);
	}

	inline ColorRGBAf::operator ColorHSVAf() const {
		return ColorHSVAf(*this);
	}

	inline bool ColorRGBAf::operator==(const ColorRGBAf& color) const {
		return r == color.r && g == color.g && b == color.b && a == color.a;
	}

	inline bool ColorRGBAf::operator!=(const ColorRGBAf& color) const {
		return r != color.r || g != color.g || b != color.b || a != color.a;
	}

	inline bool ColorRGBAf::operator<(const ColorRGBAf& color) const {
		if (r < color.r) return true;
		else if (r > color.r) return false;

		if (g < color.g) return true;
		else if (g > color.g) return false;

		if (b < color.b) return true;
		else if (b > color.b) return false;

		if (a < color.a) return true;

		return false;
	}
	
	inline bool ColorRGBAf::operator>(const ColorRGBAf& color) const {
		if (r > color.r) return true;
		else if (r < color.r) return false;

		if (g > color.g) return true;
		else if (g < color.g) return false;

		if (b > color.b) return true;
		else if (b < color.b) return false;

		if (a > color.a) return true;

		return false;
	}

	inline bool ColorRGBAf::operator<=(const ColorRGBAf& color) const {
		return *this == color || *this < color;
	}

	inline bool ColorRGBAf::operator>=(const ColorRGBAf& color) const {
		return *this == color || *this > color;
	}

	inline ColorHSV::ColorHSV() {
		h = 0;
		s = 0;
		v = 0;
	}

	inline ColorHSV::ColorHSV(const Short h, const UByte s, const UByte v) {
		this->h = h;
		this->s = s;
		this->v = v;
	}

	inline ColorHSV::ColorHSV(const ColorRGB& color) {
		ColorHSV c = ColorRGB::RGBToHSV(color.r, color.g, color.b);
		h = c.h;
		s = c.s;
		v = c.v;
	}

	inline ColorHSV::ColorHSV(const ColorRGBA& color) {
		ColorHSV c = ColorRGB::RGBToHSV(color.r, color.g, color.b);
		h = c.h;
		s = c.s;
		v = c.v;
	}

	inline ColorHSV::ColorHSV(const ColorRGBf& color) {
		ColorHSVf c = ColorRGB::RGBToHSV(color.r, color.g, color.b);
		h = (Short)(c.h * 360);
		s = (UByte)(c.s * 100);
		v = (UByte)(c.v * 100);
	}

	inline ColorHSV::ColorHSV(const ColorRGBAf& color) {
		ColorHSVf c = ColorRGB::RGBToHSV(color.r, color.g, color.b);
		h = (Short)(c.h * 360);
		s = (UByte)(c.s * 100);
		v = (UByte)(c.v * 100);
	}

	inline ColorHSV::ColorHSV(const ColorHSV& color) {
		h = color.h;
		s = color.s;
		v = color.v;
	}

	inline ColorHSV::ColorHSV(const ColorHSVA& color) {
		h = color.h;
		s = color.s;
		v = color.v;
	}

	inline ColorHSV::ColorHSV(const ColorHSVf& color) {
		h = (Short)(color.h * 360);
		s = (UByte)(color.s * 100);
		v = (UByte)(color.v * 100);
	}

	inline ColorHSV::ColorHSV(const ColorHSVAf& color) {
		h = (Short)(color.h * 360);
		s = (UByte)(color.s * 100);
		v = (UByte)(color.v * 100);
	}

	inline ColorHSV::operator ColorRGB() const {
		return ColorRGB(*this);
	}

	inline ColorHSV::operator ColorRGBA() const {
		return ColorRGBA(*this);
	}

	inline ColorHSV::operator ColorRGBf() const {
		return ColorRGBf(*this);
	}

	inline ColorHSV::operator ColorRGBAf() const {
		return ColorRGBAf(*this);
	}

	inline ColorHSV::operator ColorHSVA() const {
		return ColorHSVA(*this);
	}

	inline ColorHSV::operator ColorHSVf() const {
		return ColorHSVf(*this);
	}

	inline ColorHSV::operator ColorHSVAf() const {
		return ColorHSVAf(*this);
	}

	inline bool ColorHSV::operator==(const ColorHSV& color) const {
		return h == color.h && s == color.s && v == color.v;
	}

	inline bool ColorHSV::operator!=(const ColorHSV& color) const {
		return h != color.h || s != color.s || v != color.v;
	}

	inline bool ColorHSV::operator<(const ColorHSV& color) const {
		if (h < color.h) return true;
		else if (h > color.h) return false;

		if (s < color.s) return true;
		else if (s > color.s) return false;

		if (v < color.v) return true;

		return false;
	}
	
	inline bool ColorHSV::operator>(const ColorHSV& color) const {
		if (h > color.h) return true;
		else if (h < color.h) return false;

		if (s > color.s) return true;
		else if (s < color.s) return false;

		if (v > color.v) return true;

		return false;
	}

	inline bool ColorHSV::operator<=(const ColorHSV& color) const {
		return *this == color || *this < color;
	}

	inline bool ColorHSV::operator>=(const ColorHSV& color) const {
		return *this == color || *this > color;
	}

	inline ColorHSVA::ColorHSVA() {
		h = 0;
		s = 0;
		v = 0;
		a = 255;
	}

	inline ColorHSVA::ColorHSVA(const Short h, const UByte s, const UByte v, const UByte a) {
		this->h = h;
		this->s = s;
		this->v = v;
		this->a = a;
	}

	inline ColorHSVA::ColorHSVA(const ColorRGB& color) {
		ColorHSV c = ColorRGB::RGBToHSV(color.r, color.g, color.b);
		h = c.h;
		s = c.s;
		v = c.v;
		a = 255;
	}

	inline ColorHSVA::ColorHSVA(const ColorRGBA& color) {
		ColorHSV c = ColorRGB::RGBToHSV(color.r, color.g, color.b);
		h = c.h;
		s = c.s;
		v = c.v;
		a = color.a;
	}

	inline ColorHSVA::ColorHSVA(const ColorRGBf& color) {
		ColorHSVf c = ColorRGB::RGBToHSV(color.r, color.g, color.b);
		h = (Short)(c.h * 360);
		s = (UByte)(c.s * 100);
		v = (UByte)(c.v * 100);
		a = 255;
	}

	inline ColorHSVA::ColorHSVA(const ColorRGBAf& color) {
		ColorHSVf c = ColorRGB::RGBToHSV(color.r, color.g, color.b);
		h = (Short)(c.h * 360);
		s = (UByte)(c.s * 100);
		v = (UByte)(c.v * 100);
		a = (UByte)(color.a * 255);
	}

	inline ColorHSVA::ColorHSVA(const ColorHSV& color) {
		h = color.h;
		s = color.s;
		v = color.v;
		a = 255;
	}

	inline ColorHSVA::ColorHSVA(const ColorHSVA& color) {
		h = color.h;
		s = color.s;
		v = color.v;
		a = color.a;
	}

	inline ColorHSVA::ColorHSVA(const ColorHSVf& color) {
		h = (Short)(color.h * 360);
		s = (UByte)(color.s * 100);
		v = (UByte)(color.v * 100);
		a = 255;
	}

	inline ColorHSVA::ColorHSVA(const ColorHSVAf& color) {
		h = (Short)(color.h * 360);
		s = (UByte)(color.s * 100);
		v = (UByte)(color.v * 100);
		a = (UByte)(color.a * 255);
	}

	inline ColorHSVA::operator ColorRGB() const {
		return ColorRGB(*this);
	}

	inline ColorHSVA::operator ColorRGBA() const {
		return ColorRGBA(*this);
	}

	inline ColorHSVA::operator ColorRGBf() const {
		return ColorRGBf(*this);
	}

	inline ColorHSVA::operator ColorRGBAf() const {
		return ColorRGBAf(*this);
	}

	inline ColorHSVA::operator ColorHSV() const {
		return ColorHSV(*this);
	}

	inline ColorHSVA::operator ColorHSVf() const {
		return ColorHSVf(*this);
	}

	inline ColorHSVA::operator ColorHSVAf() const {
		return ColorHSVAf(*this);
	}

	inline bool ColorHSVA::operator==(const ColorHSVA& color) const {
		return h == color.h && s == color.s && v == color.v && a == color.a;
	}

	inline bool ColorHSVA::operator!=(const ColorHSVA& color) const {
		return h != color.h || s != color.s || v != color.v || a != color.a;
	}

	inline bool ColorHSVA::operator<(const ColorHSVA& color) const {
		if (h < color.h) return true;
		else if (h > color.h) return false;

		if (s < color.s) return true;
		else if (s > color.s) return false;

		if (v < color.v) return true;
		else if (v > color.v) return false;

		if (a < color.a) return true;

		return false;
	}
	
	inline bool ColorHSVA::operator>(const ColorHSVA& color) const {
		if (h > color.h) return true;
		else if (h < color.h) return false;

		if (s > color.s) return true;
		else if (s < color.s) return false;

		if (v > color.v) return true;
		else if (v < color.v) return false;

		if (a > color.a) return true;

		return false;
	}

	inline bool ColorHSVA::operator<=(const ColorHSVA& color) const {
		return *this == color || *this < color;
	}

	inline bool ColorHSVA::operator>=(const ColorHSVA& color) const {
		return *this == color || *this > color;
	}

	inline ColorHSVf::ColorHSVf() {
		h = 0.f;
		s = 0.f;
		v = 0.f;
	}
	
	inline ColorHSVf::ColorHSVf(const float h, const float s, const float v) {
		this->h = h;
		this->s = s;
		this->v = v;
	}
	
	inline ColorHSVf::ColorHSVf(const ColorRGB& color) {
		ColorHSVf c = ColorRGB::RGBToHSV(color.r / 255.f, color.g / 255.f, color.b / 255.f);
		h = c.h;
		s = c.s;
		v = c.v;
	}
	
	inline ColorHSVf::ColorHSVf(const ColorRGBA& color) {
		ColorHSVf c = ColorRGB::RGBToHSV(color.r / 255.f, color.g / 255.f, color.b / 255.f);
		h = c.h;
		s = c.s;
		v = c.v;
	}
	
	inline ColorHSVf::ColorHSVf(const ColorRGBf& color) {
		ColorHSVf c = ColorRGB::RGBToHSV(color.r, color.g, color.b);
		h = c.h;
		s = c.s;
		v = c.v;
	}
	
	inline ColorHSVf::ColorHSVf(const ColorRGBAf& color) {
		ColorHSVf c = ColorRGB::RGBToHSV(color.r, color.g, color.b);
		h = c.h;
		s = c.s;
		v = c.v;
	}
	
	inline ColorHSVf::ColorHSVf(const ColorHSV& color) {
		h = color.h / 360.f;
		s = color.s / 100.f;
		v = color.v / 100.f;
	}
	
	inline ColorHSVf::ColorHSVf(const ColorHSVA& color) {
		h = color.h / 360.f;
		s = color.s / 100.f;
		v = color.v / 100.f;
	}
	
	inline ColorHSVf::ColorHSVf(const ColorHSVf& color) {
		h = color.h;
		s = color.s;
		v = color.v;
	}
	
	inline ColorHSVf::ColorHSVf(const ColorHSVAf& color) {
		h = color.h;
		s = color.s;
		v = color.v;
	}
	
	inline ColorHSVf::operator ColorRGB() const {
		return ColorRGB(*this);
	}
	
	inline ColorHSVf::operator ColorRGBA() const {
		return ColorRGBA(*this);
	}
	
	inline ColorHSVf::operator ColorRGBf() const {
		return ColorRGBf(*this);
	}
	
	inline ColorHSVf::operator ColorRGBAf() const {
		return ColorRGBAf(*this);
	}
	
	inline ColorHSVf::operator ColorHSV() const {
		return ColorHSV(*this);
	}
	
	inline ColorHSVf::operator ColorHSVA() const {
		return ColorHSVA(*this);
	}
	
	inline ColorHSVf::operator ColorHSVAf() const {
		return ColorHSVAf(*this);
	}

	inline bool ColorHSVf::operator==(const ColorHSVf& color) const {
		return h == color.h && s == color.s && v == color.v;
	}

	inline bool ColorHSVf::operator!=(const ColorHSVf& color) const {
		return h != color.h || s != color.s || v != color.v;
	}

	inline bool ColorHSVf::operator<(const ColorHSVf& color) const {
		if (h < color.h) return true;
		else if (h > color.h) return false;

		if (s < color.s) return true;
		else if (s > color.s) return false;

		if (v < color.v) return true;

		return false;
	}
	
	inline bool ColorHSVf::operator>(const ColorHSVf& color) const {
		if (h > color.h) return true;
		else if (h < color.h) return false;

		if (s > color.s) return true;
		else if (s < color.s) return false;

		if (v > color.v) return true;

		return false;
	}

	inline bool ColorHSVf::operator<=(const ColorHSVf& color) const {
		return *this == color || *this < color;
	}

	inline bool ColorHSVf::operator>=(const ColorHSVf& color) const {
		return *this == color || *this > color;
	}
	
	inline ColorHSVAf::ColorHSVAf() {
		h = 0.f;
		s = 0.f;
		v = 0.f;
		a = 1.f;
	}
	
	inline ColorHSVAf::ColorHSVAf(const float h, const float s, const float v, const float a) {
		this->h = h;
		this->s = s;
		this->v = v;
		this->a = a;
	}
	
	inline ColorHSVAf::ColorHSVAf(const ColorRGB& color) {
		ColorHSVf c = ColorRGB::RGBToHSV(color.r / 255.f, color.g / 255.f, color.b / 255.f);
		h = c.h;
		s = c.s;
		v = c.v;
		a = 1.f;
	}
	
	inline ColorHSVAf::ColorHSVAf(const ColorRGBA& color) {
		ColorHSVf c = ColorRGB::RGBToHSV(color.r / 255.f, color.g / 255.f, color.b / 255.f);
		h = c.h;
		s = c.s;
		v = c.v;
		a = color.a / 255.f;
	}
	
	inline ColorHSVAf::ColorHSVAf(const ColorRGBf& color) {
		ColorHSVf c = ColorRGB::RGBToHSV(color.r, color.g, color.b);
		h = c.h;
		s = c.s;
		v = c.v;
		a = 1.f;
	}
	
	inline ColorHSVAf::ColorHSVAf(const ColorRGBAf& color) {
		ColorHSVf c = ColorRGB::RGBToHSV(color.r, color.g, color.b);
		h = c.h;
		s = c.s;
		v = c.v;
		a = color.a;
	}
	
	inline ColorHSVAf::ColorHSVAf(const ColorHSV& color) {
		h = color.h / 360.f;
		s = color.s / 100.f;
		v = color.v / 100.f;
		a = 1.f;
	}
	
	inline ColorHSVAf::ColorHSVAf(const ColorHSVA& color) {
		h = color.h / 360.f;
		s = color.s / 100.f;
		v = color.v / 100.f;
		a = color.a / 255.f;
	}
	
	inline ColorHSVAf::ColorHSVAf(const ColorHSVf& color) {
		h = color.h;
		s = color.s;
		v = color.v;
		a = 1.f;
	}
	
	inline ColorHSVAf::ColorHSVAf(const ColorHSVAf& color) {
		h = color.h;
		s = color.s;
		v = color.v;
		a = color.a;
	}
	
	inline ColorHSVAf::operator ColorRGB() const {
		return ColorRGB(*this);
	}
	
	inline ColorHSVAf::operator ColorRGBA() const {
		return ColorRGBA(*this);
	}
	
	inline ColorHSVAf::operator ColorRGBf() const {
		return ColorRGBf(*this);
	}
	
	inline ColorHSVAf::operator ColorRGBAf() const {
		return ColorRGBAf(*this);
	}
	
	inline ColorHSVAf::operator ColorHSV() const {
		return ColorHSV(*this);
	}
	
	inline ColorHSVAf::operator ColorHSVA() const {
		return ColorHSVA(*this);
	}
	
	inline ColorHSVAf::operator ColorHSVf() const {
		return ColorHSVf(*this);
	}

	inline bool ColorHSVAf::operator==(const ColorHSVAf& color) const {
		return h == color.h && s == color.s && v == color.v && a == color.a;
	}

	inline bool ColorHSVAf::operator!=(const ColorHSVAf& color) const {
		return h != color.h || s != color.s || v != color.v || a != color.a;
	}

	inline bool ColorHSVAf::operator<(const ColorHSVAf& color) const {
		if (h < color.h) return true;
		else if (h > color.h) return false;

		if (s < color.s) return true;
		else if (s > color.s) return false;

		if (v < color.v) return true;
		else if (v > color.v) return false;

		if (a < color.a) return true;

		return false;
	}
	
	inline bool ColorHSVAf::operator>(const ColorHSVAf& color) const {
		if (h > color.h) return true;
		else if (h < color.h) return false;

		if (s > color.s) return true;
		else if (s < color.s) return false;

		if (v > color.v) return true;
		else if (v < color.v) return false;

		if (a > color.a) return true;

		return false;
	}

	inline bool ColorHSVAf::operator<=(const ColorHSVAf& color) const {
		return *this == color || *this < color;
	}

	inline bool ColorHSVAf::operator>=(const ColorHSVAf& color) const {
		return *this == color || *this > color;
	}
}
