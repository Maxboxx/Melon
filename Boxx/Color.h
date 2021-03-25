#pragma once

#include "Types.h"
#include "Vector.h"
#include "Math.h"

///[Settings] block: indent

///[Namespace] Boxx
namespace Boxx {
	struct ColorRGB;
	struct ColorRGBA;
	struct ColorRGBf;
	struct ColorRGBAf;
	struct ColorHSV;
	struct ColorHSVA;
	struct ColorHSVf;
	struct ColorHSVAf;

	///[Heading] Colors

	///[Title] ColorRGB
	/// Struct for storing an rgb color using bytes.
	///[Block] ColorRGB
	struct ColorRGB {
		///[Heading] Components
		
		/// The red component.
		UByte r;

		/// The green component.
		UByte g;

		/// The blue component.
		UByte b;

		///[Heading] Constructors

		/// Creates a black color
		ColorRGB();

		/// Creates a color with the specified components.
		ColorRGB(const UByte r, const UByte g, const UByte b);

		ColorRGB(const ColorRGB& color);

		///[Heading] Operators

		/// Converts the color to {ColorRGBA}.
		explicit operator ColorRGBA() const;

		/// Converts the color to {ColorRGBf}.
		explicit operator ColorRGBf() const;

		/// Converts the color to {ColorRGBAf}.
		explicit operator ColorRGBAf() const;

		/// Converts the color to {ColorHSV}.
		explicit operator ColorHSV() const;

		/// Converts the color to {ColorHSVA}.
		explicit operator ColorHSVA() const;

		/// Converts the color to {ColorHSVf}.
		explicit operator ColorHSVf() const;

		/// Converts the color to {ColorHSVAf}.
		explicit operator ColorHSVAf() const;

		bool operator==(const ColorRGB& color) const;
		bool operator!=(const ColorRGB& color) const;
		bool operator<(const ColorRGB& color) const;
		bool operator>(const ColorRGB& color) const;
		bool operator<=(const ColorRGB& color) const;
		bool operator>=(const ColorRGB& color) const;

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

	///[Title] ColorRGBA
	/// Struct for storing an rgba color using bytes.
	///[Block] ColorRGBA
	struct ColorRGBA {
		///[Heading] Components

		/// The red component.
		UByte r;

		/// The green component.
		UByte g;

		/// The blue component.
		UByte b;

		/// The alpha component.
		UByte a;

		///[Heading] Constructors

		/// Creates a black color.
		ColorRGBA();

		/// Creates a color with the specified comonents.
		ColorRGBA(const UByte r, const UByte g, const UByte b, const UByte a = 255);

		ColorRGBA(const ColorRGBA& color);

		///[Heading] Operators

		/// Converts the color to {ColorRGB}.
		explicit operator ColorRGB() const;

		/// Converts the color to {ColorRGBf}.
		explicit operator ColorRGBf() const;

		/// Converts the color to {ColorRGBAf}.
		explicit operator ColorRGBAf() const;

		/// Converts the color to {ColorHSV}.
		explicit operator ColorHSV() const;

		/// Converts the color to {ColorHSVA}.
		explicit operator ColorHSVA() const;

		/// Converts the color to {ColorHSVf}.
		explicit operator ColorHSVf() const;

		/// Converts the color to {ColorHSVAf}.
		explicit operator ColorHSVAf() const;

		bool operator==(const ColorRGBA& color) const;
		bool operator!=(const ColorRGBA& color) const;
		bool operator<(const ColorRGBA& color) const;
		bool operator>(const ColorRGBA& color) const;
		bool operator<=(const ColorRGBA& color) const;
		bool operator>=(const ColorRGBA& color) const;
	};

	///[Title] ColorRGBf
	/// Struct for storing an rgb color using floats.
	///[Block] ColorRGBf
	struct ColorRGBf {
		///[Heading] Components

		/// The red component.
		float r;

		/// The green component.
		float g;

		/// The blue component.
		float b;

		///[Heading] Constructors

		/// Creates a black color.
		ColorRGBf();

		/// Creates a color with the specified components.
		ColorRGBf(const float r, const float g, const float b);

		/// Creates a color from a {Vector3}.
		ColorRGBf(const Vector3& vector);

		ColorRGBf(const  ColorRGBf& color);

		///[Heading] Operators

		/// Converts the color to {ColorRGB}.
		explicit operator ColorRGB() const;

		/// Converts the color to {ColorRGBA}.
		explicit operator ColorRGBA() const;

		/// Converts the color to {ColorRGBAf}.
		explicit operator ColorRGBAf() const;

		/// Converts the color to {ColorHSV}.
		explicit operator ColorHSV() const;

		/// Converts the color to {ColorHSVA}.
		explicit operator ColorHSVA() const;

		/// Converts the color to {ColorHSVf}.
		explicit operator ColorHSVf() const;

		/// Converts the color to {ColorHSVAf}.
		explicit operator ColorHSVAf() const;

		bool operator==(const ColorRGBf& color) const;
		bool operator!=(const ColorRGBf& color) const;
		bool operator<(const ColorRGBf& color) const;
		bool operator>(const ColorRGBf& color) const;
		bool operator<=(const ColorRGBf& color) const;
		bool operator>=(const ColorRGBf& color) const;
	};

	///[Title] ColorRGBAf
	/// Struct for storing an rgba color using floats.
	///[Block] ColorRGBAf
	struct ColorRGBAf {
		///[Heading] Components

		/// The red component.
		float r;
		
		/// The green component.
		float g;

		/// The blue component.
		float b;

		/// The alpha component.
		float a;
		
		///[Heading] Constructors

		/// Creates a black color.
		ColorRGBAf();

		/// Creates a color with the specified components.
		ColorRGBAf(const float r, const float g, const float b, const float a = 1.f);

		/// Creates a color from a {Vector3}.
		ColorRGBAf(const Vector3& vector);

		/// Creates a color from a {Vector4}.
		ColorRGBAf(const Vector4& vector);

		ColorRGBAf(const ColorRGBAf& color);

		///[Heading] Operators

		/// Converts the color to {ColorRGB}.
		explicit operator ColorRGB() const;

		/// Converts the color to {ColorRGBA}.
		explicit operator ColorRGBA() const;

		/// Converts the color to {ColorRGBf}.
		explicit operator ColorRGBf() const;

		/// Converts the color to {ColorHSV}.
		explicit operator ColorHSV() const;

		/// Converts the color to {ColorHSVA}.
		explicit operator ColorHSVA() const;

		/// Converts the color to {ColorHSVf}.
		explicit operator ColorHSVf() const;

		/// Converts the color to {ColorHSVAf}.
		explicit operator ColorHSVAf() const;

		bool operator==(const ColorRGBAf& color) const;
		bool operator!=(const ColorRGBAf& color) const;
		bool operator<(const ColorRGBAf& color) const;
		bool operator>(const ColorRGBAf& color) const;
		bool operator<=(const ColorRGBAf& color) const;
		bool operator>=(const ColorRGBAf& color) const;
	};

	///[Title] ColorHSV
	/// Struct for storing an hsv color using bytes.
	///[Block] ColorHSV
	struct ColorHSV {
		///[Heading] Components
		
		/// The hue of the color.
		Short h;

		/// The saturation of the color.
		UByte s;

		/// The value of the color.
		UByte v;
		
		///[Heading] Constructors

		/// Creates a black color.
		ColorHSV();

		/// Creates a color with the specified components.
		ColorHSV(const Short h, const UByte s, const UByte v);

		ColorHSV(const   ColorHSV& color);

		///[Heading] Operators

		/// Converts the color to {ColorRGB}.
		explicit operator ColorRGB() const;

		/// Converts the color to {ColorRGBA}.
		explicit operator ColorRGBA() const;

		/// Converts the color to {ColorRGBf}.
		explicit operator ColorRGBf() const;

		/// Converts the color to {ColorRGBAf}.
		explicit operator ColorRGBAf() const;

		/// Converts the color to {ColorHSVA}.
		explicit operator ColorHSVA() const;

		/// Converts the color to {ColorHSVf}.
		explicit operator ColorHSVf() const;

		/// Converts the color to {ColorHSVAf}.
		explicit operator ColorHSVAf() const;

		bool operator==(const ColorHSV& color) const;
		bool operator!=(const ColorHSV& color) const;
		bool operator<(const ColorHSV& color) const;
		bool operator>(const ColorHSV& color) const;
		bool operator<=(const ColorHSV& color) const;
		bool operator>=(const ColorHSV& color) const;
	};

	///[Title] ColorHSVA
	/// Struct for storing an hsva color using bytes.
	///[Block] ColorHSVA
	struct ColorHSVA {
		///[Heading] Components
		
		/// The hue of the color.
		Short h;

		/// The saturation of the color.
		UByte s;

		/// The value of the color.
		UByte v;

		/// The alpha value of the color.
		UByte a;
		
		///[Heading] Constructors

		/// Creates a black color.
		ColorHSVA();

		/// Creates a color with the specified components.
		ColorHSVA(const Short h, const UByte s, const UByte v, const UByte a = 255);

		ColorHSVA(const  ColorHSVA& color);

		///[Heading] Operators

		/// Converts the color to {ColorRGB}.
		explicit operator ColorRGB() const;

		/// Converts the color to {ColorRGBA}.
		explicit operator ColorRGBA() const;

		/// Converts the color to {ColorRGBf}.
		explicit operator ColorRGBf() const;

		/// Converts the color to {ColorRGBAf}.
		explicit operator ColorRGBAf() const;

		/// Converts the color to {ColorHSV}.
		explicit operator ColorHSV() const;

		/// Converts the color to {ColorHSVf}.
		explicit operator ColorHSVf() const;

		/// Converts the color to {ColorHSVAf}.
		explicit operator ColorHSVAf() const;

		bool operator==(const ColorHSVA& color) const;
		bool operator!=(const ColorHSVA& color) const;
		bool operator<(const ColorHSVA& color) const;
		bool operator>(const ColorHSVA& color) const;
		bool operator<=(const ColorHSVA& color) const;
		bool operator>=(const ColorHSVA& color) const;
	};

	///[Title] ColorHSVf
	/// Struct for storing an hsv color using floats.
	///[Block] ColorHSVf
	struct ColorHSVf {
		///[Heading] Components

		/// The hue of the color.
		float h;

		/// The saturation of the color.
		float s;

		/// The value of the color.
		float v;
		
		///[Heading] Constructors

		/// Creates a black color.
		ColorHSVf();

		/// Creates a color with the specified components.
		ColorHSVf(const float h, const float s, const float v);

		ColorHSVf(const  ColorHSVf& color);

		///[Heading] Operators

		/// Converts the color to {ColorRGB}.
		explicit operator ColorRGB() const;

		/// Converts the color to {ColorRGBA}.
		explicit operator ColorRGBA() const;

		/// Converts the color to {ColorRGBf}.
		explicit operator ColorRGBf() const;

		/// Converts the color to {ColorRGBAf}.
		explicit operator ColorRGBAf() const;

		/// Converts the color to {ColorHSV}.
		explicit operator ColorHSV() const;

		/// Converts the color to {ColorHSVA}.
		explicit operator ColorHSVA() const;

		/// Converts the color to {ColorHSVAf}.
		explicit operator ColorHSVAf() const;

		bool operator==(const ColorHSVf& color) const;
		bool operator!=(const ColorHSVf& color) const;
		bool operator<(const ColorHSVf& color) const;
		bool operator>(const ColorHSVf& color) const;
		bool operator<=(const ColorHSVf& color) const;
		bool operator>=(const ColorHSVf& color) const;
	};

	///[Title] ColorHSVAf
	/// Struct for storing an hsva color using floats.
	///[Block] ColorHSVAf
	struct ColorHSVAf {
		///[Heading] Components

		/// The hue of the color.
		float h;

		/// The saturation of the color.
		float s;

		/// The value of the color.
		float v;

		/// The alpha value of the color.
		float a;
		
		///[Heading] Constructors

		/// Creates a black color.
		ColorHSVAf();

		/// Creates a color with the specified components.
		ColorHSVAf(const float r, const float g, const float b, const float a = 1.f);

		ColorHSVAf(const ColorHSVAf& color);

		///[Heading] Operators

		/// Converts the color to {ColorRGB}.
		explicit operator ColorRGB() const;

		/// Converts the color to {ColorRGBA}.
		explicit operator ColorRGBA() const;

		/// Converts the color to {ColorRGBf}.
		explicit operator ColorRGBf() const;

		/// Converts the color to {ColorRGBAf}.
		explicit operator ColorRGBAf() const;

		/// Converts the color to {ColorHSV}.
		explicit operator ColorHSV() const;

		/// Converts the color to {ColorHSVA}.
		explicit operator ColorHSVA() const;

		/// Converts the color to {ColorHSVf}.
		explicit operator ColorHSVf() const;

		bool operator==(const ColorHSVAf& color) const;
		bool operator!=(const ColorHSVAf& color) const;
		bool operator<(const ColorHSVAf& color) const;
		bool operator>(const ColorHSVAf& color) const;
		bool operator<=(const ColorHSVAf& color) const;
		bool operator>=(const ColorHSVAf& color) const;
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

	inline ColorRGB::operator ColorRGBA() const {
		return ColorRGBA(r, g, b, 255);
	}

	inline ColorRGB::operator ColorRGBf() const {
		return ColorRGBf(r / 255.f, g / 255.f, b / 255.f);
	}

	inline ColorRGB::operator ColorRGBAf() const {
		return ColorRGBAf(r / 255.f, g / 255.f, b / 255.f, 1.f);
	}

	inline ColorRGB::operator ColorHSV() const {
		return ColorRGB::RGBToHSV(r, g, b);
	}

	inline ColorRGB::operator ColorHSVA() const {
		ColorHSV c = ColorRGB::RGBToHSV(r, g, b);
		return ColorHSVA(c.h, c.s, c.v, 255);
	}

	inline ColorRGB::operator ColorHSVf() const {
		return ColorRGB::RGBToHSV(r / 255.f, g / 255.f, b / 255.f);
	}

	inline ColorRGB::operator ColorHSVAf() const {
		ColorHSVf c = ColorRGB::RGBToHSV(r / 255.f, g / 255.f, b / 255.f);
		return ColorHSVAf(c.h, c.s, c.v, 1.f);
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

	inline ColorRGBA::ColorRGBA(const ColorRGBA& color) {
		r = color.r;
		g = color.g;
		b = color.b;
		a = color.a;
	}

	inline ColorRGBA::operator ColorRGB() const {
		return ColorRGB(r, g, b);
	}

	inline ColorRGBA::operator ColorRGBf() const {
		return ColorRGBf(r / 255.f, g / 255.f, b / 255.f);
	}

	inline ColorRGBA::operator ColorRGBAf() const {
		return ColorRGBAf(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
	}

	inline ColorRGBA::operator ColorHSV() const {
		return ColorRGB::RGBToHSV(r, g, b);
	}

	inline ColorRGBA::operator ColorHSVA() const {
		ColorHSV c = ColorRGB::RGBToHSV(r, g, b);
		return ColorHSVA(c.h, c.s, c.v, a);
	}

	inline ColorRGBA::operator ColorHSVf() const {
		return ColorRGB::RGBToHSV(r / 255.f, g / 255.f, b / 255.f);
	}

	inline ColorRGBA::operator ColorHSVAf() const {
		ColorHSVf c = ColorRGB::RGBToHSV(r / 255.f, g / 255.f, b / 255.f);
		return ColorHSVAf(c.h, c.s, c.v, a / 255.f);
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

	inline ColorRGBf::ColorRGBf(const ColorRGBf& color) {
		r = color.r;
		g = color.g;
		b = color.b;
	}

	inline ColorRGBf::operator ColorRGB() const {
		return ColorRGB(r * 255, g * 255, b * 255);
	}

	inline ColorRGBf::operator ColorRGBA() const {
		return ColorRGBA(r * 255, g * 255, b * 255, 255);
	}

	inline ColorRGBf::operator ColorRGBAf() const {
		return ColorRGBAf(r, g, b, 1.f);
	}

	inline ColorRGBf::operator ColorHSV() const {
		ColorHSVf c = ColorRGB::RGBToHSV(r, g, b);
		return ColorHSV(c.h * 360, c.s * 100, c.v * 100);
	}

	inline ColorRGBf::operator ColorHSVA() const {
		ColorHSVf c = ColorRGB::RGBToHSV(r, g, b);
		return ColorHSVA(c.h * 360, c.s * 100, c.v * 100, 255);
	}

	inline ColorRGBf::operator ColorHSVf() const {
		return ColorRGB::RGBToHSV(r, g, b);
	}

	inline ColorRGBf::operator ColorHSVAf() const {
		ColorHSVf c = ColorRGB::RGBToHSV(r, g, b);
		return ColorHSVAf(c.h, c.s, c.v, 1.f);
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

	inline ColorRGBAf::ColorRGBAf(const ColorRGBAf& color) {
		r = color.r;
		g = color.g;
		b = color.b;
		a = color.a;
	}

	inline ColorRGBAf::operator ColorRGB() const {
		return ColorRGB(r * 255, g * 255, b * 255);
	}

	inline ColorRGBAf::operator ColorRGBA() const {
		return ColorRGBA(r * 255, g * 255, b * 255, a * 255);
	}

	inline ColorRGBAf::operator ColorRGBf() const {
		return ColorRGBf(r, g, b);
	}

	inline ColorRGBAf::operator ColorHSV() const {
		ColorHSVf c = ColorRGB::RGBToHSV(r, g, b);
		return ColorHSV(c.h * 360, c.s * 100, c.v * 100);
	}

	inline ColorRGBAf::operator ColorHSVA() const {
		ColorHSVf c = ColorRGB::RGBToHSV(r, g, b);
		return ColorHSVA(c.h * 360, c.s * 100, c.v * 100, a * 255);
	}

	inline ColorRGBAf::operator ColorHSVf() const {
		return ColorRGB::RGBToHSV(r, g, b);
	}

	inline ColorRGBAf::operator ColorHSVAf() const {
		ColorHSVf c = ColorRGB::RGBToHSV(r, g, b);
		return ColorHSVAf(c.h, c.s, c.v, a);
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

	inline ColorHSV::ColorHSV(const ColorHSV& color) {
		h = color.h;
		s = color.s;
		v = color.v;
	}

	inline ColorHSV::operator ColorRGB() const {
		return ColorRGB::HSVToRGB(h, s, v);
	}

	inline ColorHSV::operator ColorRGBA() const {
		ColorRGB c = ColorRGB::HSVToRGB(h, s, v);
		return ColorRGBA(c.r, c.g, c.b, 255);
	}

	inline ColorHSV::operator ColorRGBf() const {
		return ColorRGB::HSVToRGB(h / 360.f, s / 100.f, v / 100.f);
	}

	inline ColorHSV::operator ColorRGBAf() const {
		ColorRGBf c = ColorRGB::HSVToRGB(h / 360.f, s / 100.f, v / 100.f);
		return ColorRGBAf(c.r, c.g, c.b, 1.f);
	}

	inline ColorHSV::operator ColorHSVA() const {
		return ColorHSVA(h, s, v, 255);
	}

	inline ColorHSV::operator ColorHSVf() const {
		return ColorHSVf(h / 360.f, s / 100.f, v / 100.f);
	}

	inline ColorHSV::operator ColorHSVAf() const {
		return ColorHSVAf(h / 360.f, s / 100.f, v / 100.f, 1.f);
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

	inline ColorHSVA::ColorHSVA(const ColorHSVA& color) {
		h = color.h;
		s = color.s;
		v = color.v;
		a = color.a;
	}

	inline ColorHSVA::operator ColorRGB() const {
		return ColorRGB::HSVToRGB(h, s, v);
	}

	inline ColorHSVA::operator ColorRGBA() const {
		ColorRGB c = ColorRGB::HSVToRGB(h, s, v);
		return ColorRGBA(c.r, c.g, c.b, a);
	}

	inline ColorHSVA::operator ColorRGBf() const {
		return ColorRGB::HSVToRGB(h / 360.f, s / 100.f, v / 100.f);
	}

	inline ColorHSVA::operator ColorRGBAf() const {
		ColorRGBf c = ColorRGB::HSVToRGB(h / 360.f, s / 100.f, v / 100.f);
		return ColorRGBAf(c.r, c.g, c.b, a / 255.f);
	}

	inline ColorHSVA::operator ColorHSV() const {
		return ColorHSV(h, s, v);
	}

	inline ColorHSVA::operator ColorHSVf() const {
		return ColorHSVf(h / 360.f, s / 100.f, v / 100.f);
	}

	inline ColorHSVA::operator ColorHSVAf() const {
		return ColorHSVAf(h / 360.f, s / 100.f, v / 100.f, a / 255.f);
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
	
	inline ColorHSVf::ColorHSVf(const ColorHSVf& color) {
		h = color.h;
		s = color.s;
		v = color.v;
	}
	
	inline ColorHSVf::operator ColorRGB() const {
		ColorRGBf c = ColorRGB::HSVToRGB(h, s, v);
		return ColorRGB(c.r * 255, c.g * 255, c.b * 255);
	}
	
	inline ColorHSVf::operator ColorRGBA() const {
		ColorRGBf c = ColorRGB::HSVToRGB(h, s, v);
		return ColorRGBA(c.r * 255, c.g * 255, c.b * 255, 255);
	}
	
	inline ColorHSVf::operator ColorRGBf() const {
		return ColorRGB::HSVToRGB(h, s, v);
	}
	
	inline ColorHSVf::operator ColorRGBAf() const {
		ColorRGBf c = ColorRGB::HSVToRGB(h, s, v);
		return ColorRGBAf(c.r, c.g, c.b, 1.f);
	}
	
	inline ColorHSVf::operator ColorHSV() const {
		return ColorHSV(h * 360, s * 100, v * 100);
	}
	
	inline ColorHSVf::operator ColorHSVA() const {
		return ColorHSVA(h * 360, s * 100, v * 100, 255);
	}
	
	inline ColorHSVf::operator ColorHSVAf() const {
		return ColorHSVAf(h, s, v, 1.f);
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
	
	inline ColorHSVAf::ColorHSVAf(const ColorHSVAf& color) {
		h = color.h;
		s = color.s;
		v = color.v;
		a = color.a;
	}
	
	inline ColorHSVAf::operator ColorRGB() const {
		ColorRGBf c = ColorRGB::HSVToRGB(h, s, v);
		return ColorRGB(c.r * 255, c.g * 255, c.b * 255);
	}
	
	inline ColorHSVAf::operator ColorRGBA() const {
		ColorRGBf c = ColorRGB::HSVToRGB(h, s, v);
		return ColorRGBA(c.r * 255, c.r * 255, c.r * 255, a * 255);
	}
	
	inline ColorHSVAf::operator ColorRGBf() const {
		return ColorRGB::HSVToRGB(h, s, v);
	}
	
	inline ColorHSVAf::operator ColorRGBAf() const {
		ColorRGBf c = ColorRGB::HSVToRGB(h, s, v);
		return ColorRGBAf(c.r, c.g, c.b, a);
	}
	
	inline ColorHSVAf::operator ColorHSV() const {
		return ColorHSV(h * 360, s * 100, v * 100);
	}
	
	inline ColorHSVAf::operator ColorHSVA() const {
		return ColorHSVA(h * 360, s * 100, v * 100, a * 255);
	}
	
	inline ColorHSVAf::operator ColorHSVf() const {
		return ColorHSVf(h, s, v);
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
