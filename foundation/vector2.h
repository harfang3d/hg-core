// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#pragma once

#include <cstddef>

namespace hg {

struct Vec3;
struct Vec4;
struct Mat3;

template <class T> struct tVec2 {
	static tVec2 Zero;
	static tVec2 One;

	tVec2<T>() {}
	tVec2<T>(T x_, T y_) : x(x_), y(y_) {}
	explicit tVec2<T>(const Vec3 &v);
	explicit tVec2<T>(const Vec4 &v);

	inline tVec2<T> &operator+=(const tVec2<T> &b) {
		x += b.x;
		y += b.y;
		return *this;
	}

	inline tVec2<T> &operator+=(const T v) {
		x += v;
		y += v;
		return *this;
	}

	inline tVec2<T> &operator-=(const tVec2<T> &b) {
		x -= b.x;
		y -= b.y;
		return *this;
	}

	inline tVec2<T> &operator-=(const T v) {
		x -= v;
		y -= v;
		return *this;
	}

	inline tVec2<T> &operator*=(const tVec2<T> &b) {
		x *= b.x;
		y *= b.y;
		return *this;
	}

	inline tVec2<T> &operator*=(const T v) {
		x *= v;
		y *= v;
		return *this;
	}

	inline tVec2<T> &operator/=(const tVec2<T> &b) {
		x /= b.x;
		y /= b.y;
		return *this;
	}

	inline tVec2<T> &operator/=(const T v) {
		x /= v;
		y /= v;
		return *this;
	}

	inline float operator[](size_t n) const { return (&x)[n]; }
	inline float &operator[](size_t n) { return (&x)[n]; }

	T x, y;
};

typedef tVec2<float> Vec2;
typedef tVec2<int> iVec2;

template <typename T> tVec2<T> tVec2<T>::Zero = tVec2<T>(0, 0);
template <typename T> tVec2<T> tVec2<T>::One = tVec2<T>(1, 1);

template <typename T> bool operator==(const tVec2<T> &a, const tVec2<T> &b) { return a.x == b.x && a.y == b.y; }
template <typename T> bool operator!=(const tVec2<T> &a, const tVec2<T> &b) { return a.x != b.x || a.y != b.y; }

template <typename T> bool AlmostEqual(const tVec2<T> &a, const tVec2<T> &b, float e) { return Abs(a.x - b.x) <= e && Abs(a.y - b.y) <= e; }

template <typename T> tVec2<T> operator+(const tVec2<T> &a, const tVec2<T> &b) { return tVec2<T>(a.x + b.x, a.y + b.y);}
template <typename T> tVec2<T> operator+(const tVec2<T> &v, const T k) { return tVec2<T>(v.x + k, v.y + k); }
template <typename T> tVec2<T> operator-(const tVec2<T> &a, const tVec2<T> &b) { return tVec2<T>(a.x - b.x, a.y - b.y); }
template <typename T> tVec2<T> operator-(const tVec2<T> &v, const T k) { return tVec2<T>(v.x - k, v.y - k); }
template <typename T> tVec2<T> operator*(const tVec2<T> &a, const tVec2<T> &b) { return tVec2<T>(a.x * b.x, a.y * b.y); }
template <typename T> tVec2<T> operator*(const tVec2<T> &v, const T k) { return tVec2<T>(v.x * k, v.y * k); }
template <typename T> tVec2<T> operator*(const T k, const tVec2<T> &v) { return v * k; }
template <typename T> tVec2<T> operator/(const tVec2<T> &a, const tVec2<T> &b) { return tVec2<T>(a.x / b.x, a.y / b.y); }
template <typename T> tVec2<T> operator/(const tVec2<T> &v, const T k) { return tVec2<T>(v.x / k, v.y / k); }

template <typename T> tVec2<T> operator*(const tVec2<T> &v, const Mat3 &m);

template <typename T> tVec2<T> Min(const tVec2<T> &v, const tVec2<T> &m) { return tVec2<T>(v.x < m.x ? v.x : m.x, v.y < m.y ? v.y : m.y); }
template <typename T> tVec2<T> Max(const tVec2<T> &v, const tVec2<T> &m) { return tVec2<T>(v.x > m.x ? v.x : m.x, v.y > m.y ? v.y : m.y); }

/// Squared vector length.
template <typename T> T Len2(const tVec2<T> &v) { return v.x * v.x + v.y * v.y; }
/// Vector length.
template <typename T> T Len(const tVec2<T> &v);

/// Dot product.
template <typename T> T Dot(const tVec2<T> &a, const tVec2<T> &b) { return a.x * b.x + a.y * b.y; }

/// Normalize vector.
template <typename T> tVec2<T> Normalize(const tVec2<T> &v) {
	const float l = Len(v);
	return l > 0.f ? v / l : v;
}

/// Reversed vector.
template <typename T> tVec2<T> Reverse(const tVec2<T> &v) { return tVec2<T>(-v.x, -v.y); }

/// Vector squared distance.
template <typename T> T Dist2(const tVec2<T> &a, const tVec2<T> &b) { return (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y); }
/// Vector distance.
template <typename T> T Dist(const tVec2<T> &a, const tVec2<T> &b);

} // namespace hg
