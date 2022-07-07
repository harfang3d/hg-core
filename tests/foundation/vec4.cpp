// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.
#include <math.h>

#define TEST_NO_MAIN 
#include "acutest.h"

#include "foundation/vector4.h"

#include "foundation/vector2.h"
#include "foundation/vector3.h"
#include "foundation/color.h"
#include "foundation/matrix4.h"

#include "foundation/math.h"

using namespace hg;

void test_vec4() {
	{
		Vec2 v2f(0.111f, 2.222f);
		Vec4 vf(v2f);
		TEST_CHECK(TestEqual(vf.x, 0.111f));
		TEST_CHECK(TestEqual(vf.y, 2.222f));
		TEST_CHECK(TestEqual(vf.z, 0.f));
		TEST_CHECK(TestEqual(vf.w, 1.f));
	}
	{
		iVec2 v2i(3, -17);
		Vec4 vf(v2i);
		TEST_CHECK(TestEqual(vf.x, 3.f));
		TEST_CHECK(TestEqual(vf.y, -17.f));
		TEST_CHECK(TestEqual(vf.z, 0.f));
		TEST_CHECK(TestEqual(vf.w, 1.f));
	}
	{
		Vec4 v(3.333f);
		TEST_CHECK(TestEqual(v.x, 3.333f));
		TEST_CHECK(TestEqual(v.y, 3.333f));
		TEST_CHECK(TestEqual(v.z, 3.333f));
		TEST_CHECK(TestEqual(v.w, 3.333f));
	}
	{
		Color c(1.f, 2.f, 3.f, 4.f);
		Vec4 v(c);
		TEST_CHECK(TestEqual(v.x, 1.f));
		TEST_CHECK(TestEqual(v.y, 2.f));
		TEST_CHECK(TestEqual(v.z, 3.f));
		TEST_CHECK(TestEqual(v.w, 4.f));
	}
	{
		Vec4 v = -Vec4(1.f, 2.f, 3.f,-5.f);
		TEST_CHECK(TestEqual(v.x, -1.f));
		TEST_CHECK(TestEqual(v.y, -2.f));
		TEST_CHECK(TestEqual(v.z, -3.f));
		TEST_CHECK(TestEqual(v.w, 5.f));
	}
	{
		Vec4 u(0.22f, 1.4f, -2.75f, -0.3503f);
		u += Vec4(0.042f, -0.2f, 5.25f, 0.9017f);
		TEST_CHECK(TestEqual(u.x, 0.262f));
		TEST_CHECK(TestEqual(u.y, 1.2f));
		TEST_CHECK(TestEqual(u.z, 2.5f));
		TEST_CHECK(TestEqual(u.w, 0.5514f));
	}
	{
		Vec4 u(20.97f, -3.1f, 10.02f, -1.402f);
		u += 0.1f;
		TEST_CHECK(TestEqual(u.x, 21.07f));
		TEST_CHECK(TestEqual(u.y, -3.f));
		TEST_CHECK(TestEqual(u.z, 10.12f));
		TEST_CHECK(TestEqual(u.w, -1.302f));
	}
	{
		Vec4 u(55.555f, 0.336f, 5.5555f, 7.9191f);
		u -= Vec4(2.002f, 1.01f, 2.2222f, 0.054f);
		TEST_CHECK(TestEqual(u.x, 53.553f));
		TEST_CHECK(TestEqual(u.y, -0.674f));
		TEST_CHECK(TestEqual(u.z, 3.3333f));
		TEST_CHECK(TestEqual(u.w, 7.8651f));
	}
	{
		Vec4 u(-1.025f, 2.3235f, 7.81194f, -0.40441f);
		u -= 0.004f;
		TEST_CHECK(TestEqual(u.x, -1.029));
		TEST_CHECK(TestEqual(u.y, 2.3195f));
		TEST_CHECK(TestEqual(u.z, 7.80794f));
		TEST_CHECK(TestEqual(u.w, -0.40841f));
	}
	{
		Vec4 u(7.011f, 4.0f, -11.f, 1.202f);
		u *= Vec4(0.33f, 0.5f, 0.4f, -5.01f);
		TEST_CHECK(TestEqual(u.x, 2.31363f));
		TEST_CHECK(TestEqual(u.y, 2.f));
		TEST_CHECK(TestEqual(u.z, -4.4f));
		TEST_CHECK(TestEqual(u.w, -6.02202f));
	}
	{
		Vec4 u(-2.022f, 11.11f, 0.075f, -101.3f);
		u *= 0.3f;
		TEST_CHECK(TestEqual(u.x, -0.6066f));
		TEST_CHECK(TestEqual(u.y, 3.333f));
		TEST_CHECK(TestEqual(u.z, 0.0225f));
		TEST_CHECK(TestEqual(u.w, -30.39f, 0.00001f));
	}
	{
		Vec4 u(0.4f, 4.8f, -26.f, 1.4f);
		u /= Vec4(-0.25f, 0.3f, -0.104f, 0.07f);
		TEST_CHECK(TestEqual(u.x, -1.6f));
		TEST_CHECK(TestEqual(u.y, 16.f));
		TEST_CHECK(TestEqual(u.z, 250.f));
		TEST_CHECK(TestEqual(u.w, 20.f));
	}
	{
		Vec4 u(-2.015f, 0.03f, 27.6f, -0.975f);
		u /= 0.75f;
		TEST_CHECK(TestEqual(u.x, -2.686666666f));
		TEST_CHECK(TestEqual(u.y, 0.04f));
		TEST_CHECK(TestEqual(u.z, 36.8f, 0.00001f));
		TEST_CHECK(TestEqual(u.w, -1.3f));
	}
	{
		Vec4 u(1.207f, -44.01f, 0.34034f, -54.0127f);
		TEST_CHECK(TestEqual(u[0], u.x));
		TEST_CHECK(TestEqual(u[1], u.y));
		TEST_CHECK(TestEqual(u[2], u.z));
		TEST_CHECK(TestEqual(u[3], u.w));
	}
	{
		Vec4 u(75.757575f, 1.207f, -44.01f, 0.192f);
		Vec4 v(75.757575f, 1.207f, -44.01f, 0.192f);
		Vec4 w(70.0101f, 4.4444f, 1.0001f, 0.4f);
		TEST_CHECK(u == v);
		TEST_CHECK((u == w) == false);
	}
	{
		Vec4 u(75.757575f, 1.207f, -44.01f, 0.192f);
		Vec4 v(75.757575f, 1.207f, -44.01f, 0.192f);
		Vec4 w(70.0101f, 4.4444f, 1.0001f, 0.4f);
		TEST_CHECK(u != w);
		TEST_CHECK((u != v) == false);
	}
	{
		Vec4 u(2.020f, 0.5f, -0.5f, 0.3104f);
		Vec4 v(1.010f, 1.f, -1.f, 0.0306f);
		Vec4 w = u + v;
		TEST_CHECK(TestEqual(w.x, 3.030f));
		TEST_CHECK(TestEqual(w.y, 1.5f));
		TEST_CHECK(TestEqual(w.z, -1.5f));
		TEST_CHECK(TestEqual(w.w, 0.341f));
	}
	{
		Vec4 u(47.3473f, 0.5f, -0.5f, 0.754f);
		Vec4 v = u + 0.25f;
		TEST_CHECK(TestEqual(v.x, 47.5973f));
		TEST_CHECK(TestEqual(v.y, 0.75f));
		TEST_CHECK(TestEqual(v.z, -0.25f));
		TEST_CHECK(TestEqual(v.w, 1.004f));
	}
	{
		Vec4 u(0.5f, -0.5f, 1.25909f, -0.30303f);
		Vec4 v(1.f, -1.f, 1.70707f, 0.0101f);
		Vec4 w = u - v;
		TEST_CHECK(TestEqual(w.x, -0.5f));
		TEST_CHECK(TestEqual(w.y, 0.5f));
		TEST_CHECK(TestEqual(w.z, -0.44798f));
		TEST_CHECK(TestEqual(w.w, -0.31313f));
	}
	{
		Vec4 u(0.5f, -0.5f, 3.333f, 7.05f);
		Vec4 v = u - 0.25f;
		TEST_CHECK(TestEqual(v.x, 0.25f));
		TEST_CHECK(TestEqual(v.y, -0.75f));
		TEST_CHECK(TestEqual(v.z, 3.083f));
		TEST_CHECK(TestEqual(v.w, 6.8f));
	}
	{
		Vec4 u(0.15f, -2.5f, 1.505f, 0.08);
		Vec4 v(1.1f, 0.3f, 0.76f, 2.04f);
		Vec4 w = u * v;
		TEST_CHECK(TestEqual(w.x, 0.165f));
		TEST_CHECK(TestEqual(w.y, -0.75f));
		TEST_CHECK(TestEqual(w.z, 1.1438f));
		TEST_CHECK(TestEqual(w.w, 0.1632f));
	}
	{
		Vec4 u(-5.06f, 0.75f, 2.72645f, 0.1717f);
		Vec4 v = u * 4.25f;
		Vec4 w = 4.25f * u;
		TEST_CHECK(TestEqual(v.x, -21.505f));
		TEST_CHECK(TestEqual(v.y, 3.1875f));
		TEST_CHECK(TestEqual(v.z, 11.5874125f));
		TEST_CHECK(TestEqual(v.w, 0.729725f));
		TEST_CHECK(TestEqual(w.x, v.x));
		TEST_CHECK(TestEqual(w.y, v.y));
		TEST_CHECK(TestEqual(w.z, v.z));
		TEST_CHECK(TestEqual(w.w, v.w));
	}
	{
		Vec4 u(0.48f, -2.79f, -1.3334f, 0.0794f);
		Vec4 v(1.5f, 0.03f, -0.401401f, 2.2f);
		Vec4 w = u / v;
		TEST_CHECK(TestEqual(w.x, 0.32));
		TEST_CHECK(TestEqual(w.y, -93.f));
		TEST_CHECK(TestEqual(w.z, 3.321865167f));
		TEST_CHECK(TestEqual(w.w, 0.0360909f));
	}
	{
		Vec4 u(9.008f, 0.75f, -57.1002f, 3.7f);
		Vec4 v = u / 0.1f;
		TEST_CHECK(TestEqual(v.x, 90.08f));
		TEST_CHECK(TestEqual(v.y, 7.5f));
		TEST_CHECK(TestEqual(v.z, -571.002));
		TEST_CHECK(TestEqual(v.w, 37.f));
	}
	{
		Vec4 u = RandomVec4();
		TEST_CHECK(Abs(u.x) <= 1.f);
		TEST_CHECK(Abs(u.y) <= 1.f);
		TEST_CHECK(Abs(u.z) <= 1.f);
		TEST_CHECK(Abs(u.w) <= 1.f);
		Vec4 v = RandomVec4();
		TEST_CHECK(TestEqual(u.x, v.x) == false);
		TEST_CHECK(TestEqual(u.y, v.y) == false);
		TEST_CHECK(TestEqual(u.z, v.z) == false);
		TEST_CHECK(TestEqual(u.w, v.w) == false);
		Vec4 w = RandomVec4(0.f, 10.f);
		TEST_CHECK((w.x >= 0.f) && (w.x <= 10.f));
		TEST_CHECK((w.y >= 0.f) && (w.y <= 10.f));
		TEST_CHECK((w.z >= 0.f) && (w.z <= 10.f));
		TEST_CHECK((w.w >= 0.f) && (w.w <= 10.f));
	}
	{
		Vec4 u = RandomVec4(Vec4(-1.f, 0.f, -0.5f, 2.f), Vec4(0.f, 1.f, 0.5f, 4.f));
		TEST_CHECK((u.x >= -1.f) && (u.x <= 0.f));
		TEST_CHECK((u.y >= 0.f) && (u.y <= 1.f));
		TEST_CHECK((u.z >= -0.5f) && (u.z <= 0.5f));
		TEST_CHECK((u.w >= 2.0f) && (u.w <= 4.f));

		Vec4 v = RandomVec4(-Vec4::One, Vec4::One);
		Vec4 w = RandomVec4(-Vec4::One, Vec4::One);
		TEST_CHECK(TestEqual(v.x, w.x) == false);
		TEST_CHECK(TestEqual(v.y, w.y) == false);
		TEST_CHECK(TestEqual(v.z, w.z) == false);
		TEST_CHECK(TestEqual(v.w, w.w) == false);
	}
	{
		Vec4 v = Abs(Vec4(-1.f, 1.f, -0.5f,-0.5f));
		TEST_CHECK(TestEqual(v.x, 1.f));
		TEST_CHECK(TestEqual(v.y, 1.f));
		TEST_CHECK(TestEqual(v.z, 0.5f));
		TEST_CHECK(TestEqual(v.z, 0.5f));
	}
	{
		Vec4 v = Reverse(Vec4(1.f, 2.f, 3.f, 4.f));
		TEST_CHECK(TestEqual(v.x, -1.f));
		TEST_CHECK(TestEqual(v.y, -2.f));
		TEST_CHECK(TestEqual(v.z, -3.f));
		TEST_CHECK(TestEqual(v.w, -4.f));
	}
	{ 
		Vec4 v = Normalize(Vec4(4.701f, -4.701f, -4.701f, 4.701f));
		float l = Sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
		TEST_CHECK(TestEqual(l, 1.f)); 
	}
	{
		Vec4 v = Vec4::One;
		v *= Mat4(
			0.1f, 1.f, 0.1f,
			0.2f, 2.f, 10.0f,
			0.3f, 3.f, 0.01f,
			0.4f, 4.f, 1.0f
		);
		TEST_CHECK(TestEqual(v.x, 1.f));
		TEST_CHECK(TestEqual(v.y, 10.f));
		TEST_CHECK(TestEqual(v.z, 11.11f));
		TEST_CHECK(TestEqual(v.w, 1.f));
	}
	// inline Vec4 Vec4I(int x, int y, int z, int w = 255) { return Vec4(float(x) / 255.f, float(y) / 255.f, float(z) / 255.f, float(w) / 255.f); }
}