// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "foundation/color.h"
#include "foundation/math.h"
#include "foundation/vector3.h"
#include "foundation/vector4.h"

namespace hg {

const Color Color::Zero(0, 0, 0, 0);
const Color Color::One(1, 1, 1, 1);
const Color Color::White(1, 1, 1);
const Color Color::Grey(0.5F, 0.5F, 0.5F);
const Color Color::Black(0, 0, 0);
const Color Color::Red(1, 0, 0);
const Color Color::Green(0, 1, 0);
const Color Color::Blue(0, 0, 1);
const Color Color::Yellow(1, 1, 0);
const Color Color::Orange(1, 0.3F, 0);
const Color Color::Purple(1, 0, 1);
const Color Color::Transparent(0, 0, 0, 0);

Color operator+(const Color &a, const Color &b) {
	return Color(a.r + b.r, a.g + b.g, a.b + b.b, a.a + b.a);
}

Color operator+(const Color &a, const float v) {
	return Color(a.r + v, a.g + v, a.b + v, a.a + v);
}

Color operator-(const Color &a, const Color &b) {
	return Color(a.r - b.r, a.g - b.g, a.b - b.b, a.a - b.a);
}

Color operator-(const Color &a, const float v) {
	return Color(a.r - v, a.g - v, a.b - v, a.a - v);
}

Color operator*(const Color &a, const Color &b) {
	return Color(a.r * b.r, a.g * b.g, a.b * b.b, a.a * b.a);
}

Color operator*(const Color &a, const float v) {
	return Color(a.r * v, a.g * v, a.b * v, a.a * v);
}

Color operator*(const float v, const Color &a) {
	return a * v;
}

Color operator/(const Color &a, const Color &b) {
	return Color(a.r / b.r, a.g / b.g, a.b / b.b, a.a / b.a);
}

Color operator/(const Color &a, const float v) {
	return Color(a.r / v, a.g / v, a.b / v, a.a / v);
}

float ColorToGrayscale(const Color &c) {
	return 0.3F * c.r + 0.59F * c.g + 0.11F * c.b;
}

uint32_t ColorToRGBA32(const Color &c) {
	return static_cast<uint8_t>(Clamp(c.r, 0.F, 1.F) * 255.F) | (static_cast<uint8_t>(Clamp(c.g, 0.F, 1.F) * 255.F) << 8) |
		   (static_cast<uint8_t>(Clamp(c.b, 0.F, 1.F) * 255.F) << 16) | (static_cast<uint8_t>(Clamp(c.a, 0.F, 1.F) * 255.F) << 24);
}

uint32_t ColorToABGR32(const Color &c) {
	return static_cast<uint8_t>(Clamp(c.a, 0.F, 1.F) * 255.F) | (static_cast<uint8_t>(Clamp(c.b, 0.F, 1.F) * 255.F) << 8) |
		   (static_cast<uint8_t>(Clamp(c.g, 0.F, 1.F) * 255.F) << 16) | (static_cast<uint8_t>(Clamp(c.r, 0.F, 1.F) * 255.F) << 24);
}

Color ColorFromRGBA32(unsigned int value) {
	Color c;

	c.r = static_cast<float>((value)&0xff) / 255.F;
	c.g = static_cast<float>((value >> 8) & 0xff) / 255.F;
	c.b = static_cast<float>((value >> 16) & 0xff) / 255.F;
	c.a = static_cast<float>((value >> 24) & 0xff) / 255.F;

	return c;
}

Color ColorFromABGR32(unsigned int value) {
	Color c;

	c.a = static_cast<float>((value)&0xff) / 255.F;
	c.b = static_cast<float>((value >> 8) & 0xff) / 255.F;
	c.g = static_cast<float>((value >> 16) & 0xff) / 255.F;
	c.r = static_cast<float>((value >> 24) & 0xff) / 255.F;

	return c;
}

//
unsigned int ARGB32ToRGBA32(unsigned int argb) {
	return ((argb & 0xff) << 24) + (((argb >> 8) & 0xff) << 16) + (((argb >> 16) & 0xff) << 8) + ((argb >> 24) & 0xff);
}

static inline uint32_t pack_u32(uint8_t r, uint8_t s, uint8_t t, uint8_t p) {
	return (r << 24) | (s << 16) | (t << 8) | p;
}
//
uint32_t RGBA32(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	return pack_u32(a, b, g, r);
}

uint32_t ARGB32(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	return pack_u32(r, g, b, a);
}

/// Vector squared distance.
float Dist2(const Color &i, const Color &j) {
	return (j.r - i.r) * (j.r - i.r) + (j.g - i.g) * (j.g - i.g) + (j.b - i.b) * (j.b - i.b) + (j.a - i.a) * (j.a - i.a);
}
/// Vector distance.
float Dist(const Color &i, const Color &j) {
	return Sqrt(Dist2(i, j));
}

/// Compare two colors with a configurable threshold.
bool AlmostEqual(const Color &a, const Color &b, float e) {
	return AlmostEqual(a.r, b.r, e) && AlmostEqual(a.g, b.g, e) && AlmostEqual(a.b, b.b, e) && AlmostEqual(a.a, b.a, e);
}

/// Scale the chroma component of a color, return the result as a new color.
Color ChromaScale(const Color &c, float k) {
	return Color(c.r * k, c.g * k, c.b * k, c.a);
}
/// Scale the alpha component of a color, return the result as a new color.
Color AlphaScale(const Color &c, float k) {
	return Color(c.r, c.g, c.b, c.a * k);
}

//
Color Clamp(const Color &c, float min, float max) {
	return Color(Clamp(c.r, min, max), Clamp(c.g, min, max), Clamp(c.b, min, max), Clamp(c.a, min, max));
}
Color Clamp(const Color &c, const Color &min, const Color &max) {
	return Color(Clamp(c.r, min.r, max.r), Clamp(c.g, min.g, max.g), Clamp(c.b, min.b, max.b), Clamp(c.a, min.a, max.a));
}

//
Color ClampLen(const Color &c, float min, float max) {
	const float l2 = static_cast<float>(c.r * c.r + c.g * c.g + c.b * c.b);

	Color o = c;

	if ((l2 < (min * min) || l2 > (max * max)) && l2 > std::numeric_limits<float>::epsilon()) {
		const float l = Sqrt(l2);
		o = c * Clamp(l, min, max) / l;
	}

	return o;
}

Color ColorFromVector3(const Vec3 &v) {
	return Color(v.x, v.y, v.z, 1.F);
}
Color ColorFromVector4(const Vec4 &v) {
	return Color(v.x, v.y, v.z, v.w);
}

//
Color ToHLS(const Color &c) {
	const float min = Min(c.r, c.g, c.b);
	const float max = Max(c.r, c.g, c.b);

	const float diff = max - min;
	const float l = (max + min) / 2.F;

	float h = 0, s = 0;

	if (diff > 0.F) {
		if (l <= 0.5F) {
			s = diff / (max + min);
		} else {
			s = diff / (2.F - max - min);
		}

		const float r_dist = (max - c.r) / diff;
		const float g_dist = (max - c.g) / diff;
		const float b_dist = (max - c.b) / diff;

		if (Equal(c.r, max)) {
			h = b_dist - g_dist;
		} else if (Equal(c.g, max)) {
			h = 2.F + r_dist - b_dist;
		} else {
			h = 4.F + g_dist - r_dist;
		}

		h = h * 60.F;
		if (h < 0.F) {
			h += 360.F;
		}
	}

	return Color(h, l, s, c.a);
}

static float QqhToRgb(float q1, float q2, float hue) {
	if (hue > 360.F) {
		hue -= 360.F;
	} else {
		if (hue < 0.F) {
			hue += 360.F;
		}
	}

	float out;

	if (hue < 60.F) {
		out = q1 + (q2 - q1) * hue / 60.F;
	} else if (hue < 180.F) {
		out = q2;
	} else if (hue < 240.F) {
		out = q1 + (q2 - q1) * (240.F - hue) / 60.F;
	} else {
		out = q1;
	}

	return out;
}

Color FromHLS(const Color &c) {
	float r, g, b;

	if (EqualZero(c.b)) {
		r = c.g;
		g = c.g;
		b = c.g;
	} else {
		const float p2 = c.g <= 0.5F ? c.g * (1.F + c.b) : c.g + c.b - c.g * c.b;
		const float p1 = 2.F * c.g - p2;

		r = QqhToRgb(p1, p2, c.r + 120.F);
		g = QqhToRgb(p1, p2, c.r);
		b = QqhToRgb(p1, p2, c.r - 120.F);
	}

	return Color(r, g, b, c.a);
}

//
Color SetHue(const Color &c, float h) {
	Color hls = ToHLS(c);
	hls.r = h;
	return FromHLS(hls);
}

Color SetSaturation(const Color &c, float s) {
	Color hls = ToHLS(c);
	hls.b = s;
	return FromHLS(hls);
}

Color SetLuminance(const Color &c, float l) {
	Color hls = ToHLS(c);
	hls.g = l;
	return FromHLS(hls);
}

Color ScaleHue(const Color &c, float k) {
	Color hls = ToHLS(c);
	hls.r *= k;
	return FromHLS(hls);
}

Color ScaleSaturation(const Color &c, float k) {
	Color hls = ToHLS(c);
	hls.b *= k;
	return FromHLS(hls);
}

Color ScaleLuminance(const Color &c, float k) {
	Color hls = ToHLS(c);
	hls.g *= k;
	return FromHLS(hls);
}

} // namespace hg
