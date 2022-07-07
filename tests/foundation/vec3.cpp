// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.
#include <math.h>

#define TEST_NO_MAIN 
#include "acutest.h"

#include "foundation/math.h"
#include "foundation/vector2.h"
#include "foundation/vector3.h"
#include "foundation/vector4.h"

using namespace hg;

void test_vec3() {
	{
		Vec2 v2f(0.111f, 2.222f);
		Vec3 vf(v2f);
		TEST_CHECK(TestEqual(vf.x, 0.111f));
		TEST_CHECK(TestEqual(vf.y, 2.222f));
		TEST_CHECK(TestEqual(vf.z, 0.f));
		iVec2 v2i(-2, 55);
		Vec3 vi(v2i);
		TEST_CHECK(TestEqual(vi.x, -2.f));
		TEST_CHECK(TestEqual(vi.y, 55.f));
		TEST_CHECK(TestEqual(vi.z, 0.f));
	}
	{
		Vec3 v(3.333f);
		TEST_CHECK(TestEqual(v.x, 3.333f));
		TEST_CHECK(TestEqual(v.y, 3.333f));
		TEST_CHECK(TestEqual(v.z, 3.333f));
	}
	{
		Vec4 v4(1.f, 2.f, 3.f, 4.f);
		Vec3 v(v4);
		TEST_CHECK(TestEqual(v.x, 1.f));
		TEST_CHECK(TestEqual(v.y, 2.f));
		TEST_CHECK(TestEqual(v.z, 3.f));
	}
	{
		Vec3 u(0.22f, 1.4f, -2.75f);
		u += Vec3(0.042f, -0.2f, 5.25);
		TEST_CHECK(TestEqual(u.x, 0.262f));
		TEST_CHECK(TestEqual(u.y, 1.2f));
		TEST_CHECK(TestEqual(u.z, 2.5f));
	}
	{
		Vec3 u(20.97f, -3.1f, 10.02f);
		u += 0.1f;
		TEST_CHECK(TestEqual(u.x, 21.07f));
		TEST_CHECK(TestEqual(u.y, -3.f));
		TEST_CHECK(TestEqual(u.z, 10.12f));
	}
	{
		Vec3 u(55.555f, 0.336f, 5.5555f);
		u -= Vec3(2.002f, 1.01f, 2.2222f);
		TEST_CHECK(TestEqual(u.x, 53.553f));
		TEST_CHECK(TestEqual(u.y, -0.674f));
		TEST_CHECK(TestEqual(u.z, 3.3333f));
	}
	{
		Vec3 u(-1.025f, 2.3235f, 7.81194f);
		u -= 0.004f;
		TEST_CHECK(TestEqual(u.x, -1.029));
		TEST_CHECK(TestEqual(u.y, 2.3195f));
		TEST_CHECK(TestEqual(u.z, 7.80794f));
	}

// inline Vec3 &operator*=(const Vec3 &b) {
// inline Vec3 &operator*=(const float k) {
// inline Vec3 &operator/=(const Vec3 &b) {
// inline Vec3 &operator/=(const float k) {
// inline float operator[](size_t n) const { return (&x)[n]; }
// 	inline float &operator[](size_t n) { return (&x)[n]; }
// inline bool operator==(const Vec3 &a, const Vec3 &b) { return a.x == b.x && a.y == b.y && a.z == b.z; }
// inline bool operator!=(const Vec3 &a, const Vec3 &b) { return a.x != b.x || a.y != b.y || a.z != b.z; }
// inline Vec3 operator+(const Vec3 &a, const Vec3 &b) { return Vec3(a.x + b.x, a.y + b.y, a.z + b.z); }
// inline Vec3 operator+(const Vec3 &a, const float v) { return Vec3(a.x + v, a.y + v, a.z + v); }
// inline Vec3 operator-(const Vec3 &a, const Vec3 &b) { return Vec3(a.x - b.x, a.y - b.y, a.z - b.z); }
// inline Vec3 operator-(const Vec3 &a, const float v) { return Vec3(a.x - v, a.y - v, a.z - v); }
// inline Vec3 operator*(const Vec3 &a, const Vec3 &b) { return Vec3(a.x * b.x, a.y * b.y, a.z * b.z); }
// inline Vec3 operator*(const Vec3 &a, const float v) { return Vec3(a.x * v, a.y * v, a.z * v); }
// inline Vec3 operator*(const float v, const Vec3 &a) { return a * v; }
// inline Vec3 operator/(const Vec3 &a, const Vec3 &b) { return Vec3(a.x / b.x, a.y / b.y, a.z / b.z); }
// inline Vec3 operator/(const Vec3 &a, const float v) { return Vec3(a.x / v, a.y / v, a.z / v); }
// Vec3 RandomVec3(float min = -1.f, float max = 1.f);
// Vec3 RandomVec3(const Vec3 &min, const Vec3 &max);
// bool AlmostEqual(const Vec3 &a, const Vec3 &b, float epsilon);
// Vec3 BaseToEuler(const Vec3 &u);
// Vec3 BaseToEuler(const Vec3 &u, const Vec3 &v);
// int Hash(const Vec3 &v);
// float Dist2(const Vec3 &a, const Vec3 &b);
// float Dist(const Vec3 &a, const Vec3 &b);
// float Len2(const Vec3 &v);
// float Len(const Vec3 &v);
// Vec3 Min(const Vec3 &a, const Vec3 &b);
// Vec3 Max(const Vec3 &a, const Vec3 &b);
// inline float Dot(const Vec3 &a, const Vec3 &b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
// inline Vec3 Cross(const Vec3 &a, const Vec3 &b) { return Vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }
// inline Vec3 Reverse(const Vec3 &v) { return Vec3(-v.x, -v.y, -v.z); }
// inline Vec3 Inverse(const Vec3 &v) { return Vec3(1.f / v.x, 1.f / v.y, 1.f / v.z); }
// Vec3 Normalize(const Vec3 &v);
// Vec3 Clamp(const Vec3 &v, float min, float max);
// Vec3 Clamp(const Vec3 &v, const Vec3 &min, const Vec3 &max);
// Vec3 ClampLen(const Vec3 &v, float min, float max);
// Vec3 Abs(const Vec3 &v);
// Vec3 Sign(const Vec3 &v);
// Vec3 Reflect(const Vec3 &v, const Vec3 &n);
// Vec3 Refract(const Vec3 &v, const Vec3 &n, float k_in = 1.f, float k_out = 1.f);
// Vec3 Floor(const Vec3 &v);
// Vec3 Ceil(const Vec3 &v);
// Vec3 FaceForward(const Vec3 &v, const Vec3 &d);
// Vec3 MakeVec3(const Vec4 &v);
// Vec3 Quantize(const Vec3 &v, float qx, float qy, float qz);
// Vec3 Quantize(const Vec3 &v, float q);
// Vec3 Deg3(float x, float y, float z);
// Vec3 Rad3(float x, float y, float z);
// inline Vec3 Vec3I(int x, int y, int z) { return Vec3(float(x) / 255.f, float(y) / 255.f, float(z) / 255.f); }

#if 0
	{
		Vec2 u(0.5f,-0.5f);
		Vec2 v(1.f, -1.f);
		Vec2 w = u + v;
		TEST_CHECK(TestEqual(w.x, 1.5f));
		TEST_CHECK(TestEqual(w.y,-1.5f));
	}
	{
		iVec2 u(1,-1);
		iVec2 v(2, 6);
		iVec2 w = u + v;
		TEST_CHECK(w.x == 3);
		TEST_CHECK(w.y == 5);
	}
	{
		Vec2 u(0.5f,-0.5f);
		Vec2 v = u + 0.25f;
		TEST_CHECK(TestEqual(v.x, 0.75f));
		TEST_CHECK(TestEqual(v.y,-0.25f));
	}
	{
		iVec2 u(-2,-1);
		iVec2 v = u + 2;
		TEST_CHECK(v.x == 0);
		TEST_CHECK(v.y == 1);
	}
	{
		Vec2 u(0.5f,-0.5f);
		Vec2 v(1.f, -1.f);
		Vec2 w = u - v;
		TEST_CHECK(TestEqual(w.x,-0.5f));
		TEST_CHECK(TestEqual(w.y, 0.5f));
	}
	{
		iVec2 u(3, 6);
		iVec2 v(4, 8);
		iVec2 w = u - v;
		TEST_CHECK(w.x == -1);
		TEST_CHECK(w.y == -2);
	}
	{
		Vec2 u(0.5f,-0.5f);
		Vec2 v = u - 0.25f;
		TEST_CHECK(TestEqual(v.x, 0.25f));
		TEST_CHECK(TestEqual(v.y,-0.75f));
	}
	{
		iVec2 u(-2,-1);
		iVec2 v = u - 2;
		TEST_CHECK(v.x == -4);
		TEST_CHECK(v.y == -3);
	}
	{
		Vec2 u(0.15f, -2.5f);
		Vec2 v(1.1f, 0.3f);
		Vec2 w = u * v;
		TEST_CHECK(TestEqual(w.x, 0.165f));
		TEST_CHECK(TestEqual(w.y,-0.75f));
	}
	{
		iVec2 u(5, 9);
		iVec2 v(3,-7);
		iVec2 w = u * v;
		TEST_CHECK(w.x == 15);
		TEST_CHECK(w.y ==-63);
	}
	{
		Vec2 u(-5.06f, 0.75f);
		Vec2 v = u * 4.25f;
		Vec2 w = 4.25f * u;
		TEST_CHECK(TestEqual(v.x, -21.505f));
		TEST_CHECK(TestEqual(v.y, 3.1875f));
		TEST_CHECK(TestEqual(w.x, v.x));
		TEST_CHECK(TestEqual(w.y, v.y));
	}
	{
		iVec2 u(-77, 109);
		iVec2 v = u * 3;
		iVec2 w = 3 * u;
		TEST_CHECK(v.x == -231);
		TEST_CHECK(v.y == 327);
		TEST_CHECK(w.x == v.x);
		TEST_CHECK(w.y == v.y);
	}
	{
		Vec2 u(0.48f, -2.79f);
		Vec2 v(1.5f, 0.03f);
		Vec2 w = u / v;
		TEST_CHECK(TestEqual(w.x, 0.32));
		TEST_CHECK(TestEqual(w.y, -93.f));
	}
	{
		iVec2 u(104, 54);
		iVec2 v(3, -3);
		iVec2 w = u / v;
		TEST_CHECK(w.x == 34);
		TEST_CHECK(w.y == -18);
	}
	{
		Vec2 u(9.008f, 0.75f);
		Vec2 v = u / 0.1f;
		TEST_CHECK(TestEqual(v.x, 90.08f));
		TEST_CHECK(TestEqual(v.y, 7.5f));
	}
	{
		iVec2 u(200, 18);
		iVec2 v = u / 4;
		TEST_CHECK(v.x == 50);
		TEST_CHECK(v.y == 4);
	}

	{
		Vec2 u(4.0f, -11.f);
		u *= Vec2(0.5f, 0.4f);
		TEST_CHECK(TestEqual(u.x, 2.f));
		TEST_CHECK(TestEqual(u.y, -4.4f));
	}
	{
		iVec2 u(4, 7);
		u *= iVec2(4, -10);
		TEST_CHECK(u.x == 16);
		TEST_CHECK(u.y == -70);
	}
	{
		Vec2 u(11.11f, 0.075f);
		u *= 0.3f;
		TEST_CHECK(TestEqual(u.x, 3.333f));
		TEST_CHECK(TestEqual(u.y, 0.0225f));
	}
	{
		iVec2 u(5, 101);
		u *= 3;
		TEST_CHECK(u.x == 15);
		TEST_CHECK(u.y == 303);
	}
	{
		Vec2 u(4.8f, -26.f);
		u /= Vec2(0.3f, -0.104f);
		TEST_CHECK(TestEqual(u.x, 16.f));
		TEST_CHECK(TestEqual(u.y, 250.f));
	}
	{
		iVec2 u(8, 365);
		u /= iVec2(2, 97);
		TEST_CHECK(u.x == 4);
		TEST_CHECK(u.y == 3);
	}
	{
		Vec2 u(0.03f, 27.6f);
		u /= 0.75f;
		TEST_CHECK(TestEqual(u.x, 0.04f));
		TEST_CHECK(TestEqual(u.y, 36.8f));
	}
	{
		iVec2 u(121, -909);
		u /= 11;
		TEST_CHECK(u.x == 11);
		TEST_CHECK(u.y == -82);
	} 
	{
		Vec2 u(1.207f,-44.01f);
		TEST_CHECK(TestEqual(u[0], u.x));
		TEST_CHECK(TestEqual(u[1], u.y));
	}
	{
		iVec2 u(121, -909);
		TEST_CHECK(u[0] == u.x);
		TEST_CHECK(u[1] == u.y);
	}
	{
		Vec2 u(1.207f, -44.01f);
		Vec2 v(1.207f, -44.01f);
		Vec2 w(4.4444f, 1.0001f);
		TEST_CHECK(u == v);
		TEST_CHECK((u == w) == false);
	}
	{
		iVec2 u(121, -909);
		iVec2 v(121, -909);
		iVec2 w(4, -908);
		TEST_CHECK(u == v);
		TEST_CHECK((u == w) == false);
	}
	{
		Vec2 u(1.207f, -44.01f);
		Vec2 v(1.207f, -44.01f);
		Vec2 w(4.4444f, 1.0001f);
		TEST_CHECK(u != w);
		TEST_CHECK((u != v) == false);
	}
	{
		iVec2 u(121, -909);
		iVec2 v(121, -909);
		iVec2 w(4, -908);
		TEST_CHECK(u != w);
		TEST_CHECK((u != v) == false);
	}
	{
		Vec2 u(-3.0444f, 102.001f);
		Vec2 v(-3.0443f, 102.00105f);
		Vec2 w(-3.04f, 102.0015f);
		TEST_CHECK(AlmostEqual(u, v, 0.0001f));
		TEST_CHECK(AlmostEqual(u, w, 0.0001f) == false);
		TEST_CHECK(AlmostEqual(u, w, 0.005f));
	}
	{
		iVec2 u(10001, 10003);
		iVec2 v(10000, 10004);
		iVec2 w(10011, 9999);
		TEST_CHECK(AlmostEqual(u, v, 1));
		TEST_CHECK(AlmostEqual(u, w, 1) == false);
		TEST_CHECK(AlmostEqual(u, w, 10));
	}
	{
		TEST_CHECK(Min(Vec2(0.336f, 5.5555f), Vec2(1.01f, 2.2222f)) == Vec2(0.336f, 2.2222f));
		TEST_CHECK(Min(iVec2(7, -4), iVec2(5, -2)) == iVec2(5, -4));
	}
	{
		TEST_CHECK(Max(Vec2(0.336f, 5.5555f), Vec2(1.01f, 2.2222f)) == Vec2(1.01f, 5.5555f));
		TEST_CHECK(Max(iVec2(7, -4), iVec2(5, -2)) == iVec2(7, -2));
	}
	{ 
		TEST_CHECK(TestEqual(Len2(Vec2(1.f, -1.f)), 2.f));
		TEST_CHECK(Len2(iVec2(-1, -1)) == 2);
	}
	{
		TEST_CHECK(TestEqual(Len(Vec2(3.f, -4.f)), 5.f));
		TEST_CHECK(Len(iVec2(-4, -3)) == 5);
	}
	{
		TEST_CHECK(TestEqual(Dot(Vec2(1.f, -1.f), Vec2(-1.f,-1.f)), 0.f));
		TEST_CHECK(TestEqual(Dot(Vec2(2.f, 3.f), Vec2(4.f, 5.f)), 23.f));
		TEST_CHECK(Dot(iVec2(1, 1), iVec2(-1, -1)) == -2);
		TEST_CHECK(Dot(iVec2(2, -3), iVec2(4, 5)) == -7);
	}
	{
		TEST_CHECK(Normalize(Vec2(0.3, -0.4f)) == Vec2(0.6, -0.8));
		TEST_CHECK(Len(Normalize(Vec2(0.3, -0.4f))) == 1.f);
		TEST_CHECK(Normalize(iVec2(-1, 1)) == iVec2(-1, 1));
		TEST_CHECK(Len(Normalize(iVec2(-1, 1))) == 1);
	}
	{
		TEST_CHECK(Reverse(Vec2(-1.1f, 0.5f)) == Vec2(1.1,-0.5f));
		TEST_CHECK(Reverse(iVec2(78, -99)) == iVec2(-78, 99));
	}
	{
		TEST_CHECK(TestEqual(Dist2(Vec2(0.3f, -1.2f), Vec2(1.2f, 3.1f)), 19.3f, 0.00001f));
		TEST_CHECK(TestEqual(Dist2(iVec2(4, 12), iVec2(7, 8)), 25));
	}
	{
		TEST_CHECK(TestEqual(Dist(Vec2(1.4f, 2.3f), Vec2(-0.2f, -0.1f)), sqrt(8.32f)));
		TEST_CHECK(TestEqual(Dist(iVec2(-2, -2), iVec2(1, 2)), 5));
	}
#endif
}
