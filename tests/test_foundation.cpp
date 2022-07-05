// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include <math.h>

#include "acutest.h"

#include "foundation/math.h"
#include "foundation/unit.h"
#include "foundation/time.h"
#include "foundation/clock.h"
#include "foundation/vector2.h"
#include "foundation/vector3.h"
#include "foundation/vector4.h"
#include "foundation/log.h"

#include <fmt/format.h>

using namespace hg;

//
void test_math() {
	TEST_CHECK(Abs(0) == 0);
	TEST_CHECK(Abs( 1000) == 1000);
	TEST_CHECK(Abs(-1000) == 1000);
	TEST_CHECK(Abs( 2.f) == 2.f);
	TEST_CHECK(Abs(-2.f) == 2.f);

	TEST_CHECK(Min(0, 0) == 0);
	TEST_CHECK(Min(10, 40) == 10);
	TEST_CHECK(Min(60, 3) == 3);
	TEST_CHECK(Min(-1, 4) == -1);
	TEST_CHECK(Min(-999, -3) == -999);
	TEST_CHECK(Min(-12, -34) == -34);
	TEST_CHECK(Min(1.0/3.0, 1.0/4.0) == 1.0/4.0);
	TEST_CHECK(Min(55.5555f, 48.33333f) == 48.33333f);
	TEST_CHECK(Min(-55.5555f,-48.33333f) == -55.5555f);
	TEST_CHECK(Min(10.0051, 10.005) == 10.005);

	TEST_CHECK(Min(22.22224, 22.22226, 22.222265) == 22.22224);
	TEST_CHECK(Min(0.00003, -0.00001, 0.0004) == -0.00001);
	TEST_CHECK(Min(5.55755, 5.550015, 5.54975) == 5.54975);

	TEST_CHECK(Max(0, 0) == 0);
	TEST_CHECK(Max(10, 40) == 40);
	TEST_CHECK(Max(60, 3) == 60);
	TEST_CHECK(Max(-1, 4) == 4);
	TEST_CHECK(Max(-999, -3) == -3);
	TEST_CHECK(Max(-12, -34) == -12);
	TEST_CHECK(Max(1.0 / 3.0, 1.0 / 4.0) == 1.0 / 3.0);
	TEST_CHECK(Max(55.5555f, 48.33333f) == 55.5555f);
	TEST_CHECK(Max(-55.5555f, -48.33333f) == -48.33333f);
	TEST_CHECK(Max(10.0051, 10.005) == 10.0051);

	TEST_CHECK(Max(22.22224, 22.22226, 22.222265) == 22.222265);
	TEST_CHECK(Max(0.00003, 0.0004, -0.00001) == 0.0004);
	TEST_CHECK(Max(5.55755, 5.550015, 5.54975) == 5.55755);

	TEST_CHECK(Clamp(0, -1, 1) == 0);
	TEST_CHECK(Clamp(1, -2, 2) == 1);
	TEST_CHECK(Clamp(2, -1, 1) == 1);
	TEST_CHECK(Clamp(-2, -1, 1) == -1);
	TEST_CHECK(Clamp(0.0, -1.02, 1.415) == 0.0);
	TEST_CHECK(Clamp(-1.2, -1.02, 1.415) == -1.02);
	TEST_CHECK(Clamp(1.4152, -1.02, 1.415) == 1.415);
	TEST_CHECK(Clamp(-33.3333, -9.4, 13.5) == -9.4);
	TEST_CHECK(Clamp(999.9999, -77.002, 2.715) == 2.715);
	TEST_CHECK(Clamp(1.10333, -2.11111, 2.323232) == 1.10333);

	TEST_CHECK(Lerp(-4.f, 5.f, 0.f) == -4.f);
	TEST_CHECK(Lerp(-4.f, 5.f, 1.f) == 5.f);
	TEST_CHECK(Lerp(9.f, 3.f, 0.f) == 9.f);
	TEST_CHECK(Lerp(9.f, 3.f, 1.f) == 3.f);
	TEST_CHECK(Lerp(-11.f, 11.f, 0.5f) == 0.f);

	TEST_CHECK(Sqrt(0.f) == 0.f);
	TEST_CHECK(Sqrt(1.f) == 1.f);
	TEST_CHECK(Sqrt(25.f) == 5.f);
	TEST_CHECK(Sqrt(9.f) == 3.f);
	TEST_CHECK(Sqrt(2.f) == 1.41421356237309504880f);

	TEST_CHECK(TestEqual(0.f, 0.f));
	TEST_CHECK(TestEqual(0.f, 1.f) == false);
	TEST_CHECK(TestEqual(99.999998f, 99.999998f));
	TEST_CHECK(TestEqual(17.353535f, 17.353535f));
	TEST_CHECK(TestEqual(17.3535352f, 17.3535351f));
	TEST_CHECK(TestEqual(0.12547f, 0.125721f, 0.001));
	TEST_CHECK(TestEqual(0.1259999f, 0.1281111f, 0.001) == false);

	TEST_CHECK(EqualZero(0.f));
	TEST_CHECK(EqualZero(0.000001f));
	TEST_CHECK(EqualZero(-0.000001f));
	TEST_CHECK(EqualZero(0.000002f) == false);
	TEST_CHECK(EqualZero(0.0001678f, 0.0002f));

	TEST_CHECK(Ceil(1.f) == 1.f);
	TEST_CHECK(Ceil(-1.f) == -1.f);
	TEST_CHECK(Ceil(1.5125f) == 2.f);
	TEST_CHECK(Ceil(1.99999f) == 2.f);
	TEST_CHECK(Ceil(1.49999f) == 2.f);
	TEST_CHECK(Ceil(1.11111f) == 2.f);
	TEST_CHECK(Ceil(-2.11111f) == -2.f);
	TEST_CHECK(Ceil(-0.5f) == 0.f);

	TEST_CHECK(Floor(1.f) == 1.f);
	TEST_CHECK(Floor(-1.f) == -1.f);
	TEST_CHECK(Floor(1.5125f) == 1.f);
	TEST_CHECK(Floor(1.99999f) == 1.f);
	TEST_CHECK(Floor(1.49999f) == 1.f);
	TEST_CHECK(Floor(1.11111f) == 1.f);
	TEST_CHECK(Floor(-2.11111f) == -3.f);
	TEST_CHECK(Floor(-0.5f) == -1.f);

	TEST_CHECK(Mod(301.f) == 0.f);
	TEST_CHECK(TestEqual(Mod(0.5555f), 0.5555f));
	TEST_CHECK(TestEqual(Mod(-99.125f), -0.125f));
	
	TEST_CHECK(Frac(4.f) == 0.f);
	TEST_CHECK(TestEqual(Frac(11.5555f), 0.5555f));
	TEST_CHECK(TestEqual(Frac(-8.125f), -0.125f));

	TEST_CHECK(RangeAdjust(0.f, -1.f, 1.f, -2.f, 1.f) == -0.5f);
	TEST_CHECK(RangeAdjust(0.5f, 0.f, 1.f, -1.f, 1.f) == 0.f);

	TEST_CHECK(TestEqual(Quantize(5.f, 4.f), 4.f));
	TEST_CHECK(TestEqual(Quantize(6.f, 9.f), 0.f));
	TEST_CHECK(TestEqual(Quantize(32.561f, 3.f), 30.f));

	TEST_CHECK(TestEqual(Wrap(2.5f, 0.f, 1.f), 0.5f));
	TEST_CHECK(TestEqual(Wrap(4.5, 2.5, 5.25), 4.5f));
	TEST_CHECK(TestEqual(Wrap(1.7, 0.5, 0.725), 0.575f));
	TEST_CHECK(TestEqual(Wrap(HalfPi, -Pi, Pi), HalfPi));
	TEST_CHECK(TestEqual(Wrap(Pi, -Pi, Pi), Pi));
	TEST_CHECK(TestEqual(Wrap(1.0, 0.0, 1.0), 1.0));
	TEST_CHECK(TestEqual(Wrap(2.f, 0.f, 1.f), 1.f));
	TEST_CHECK(TestEqual(Wrap(-7, -6, -3), -4));
	TEST_CHECK(TestEqual(Wrap(28,  6,  3), 4));
	
	TEST_CHECK(IsFinite(32755.f) == true);
	TEST_CHECK(IsFinite(-9999.999f) == true);
	TEST_CHECK(IsFinite(log(0.0)) == false);
	
	TEST_CHECK(getPOT(256) == 256);
	TEST_CHECK(getPOT(220) == 256);
	TEST_CHECK(getPOT(1960) == 2048);
	TEST_CHECK(getPOT(-140) == 1);

	TEST_CHECK(isPOT(512));
	TEST_CHECK(isPOT(1080) == false);
	TEST_CHECK(isPOT(-256) == false);
	
// [todo] float Sin(float);
// [todo] float ASin(float);
// [todo] float Cos(float);
// [todo] float ACos(float);
// [todo] float Tan(float);
// [todo] float ATan(float);

// [todo] template <typename T> constexpr T LinearInterpolate(T y0, T y1, float t)
// [todo] template <typename T> T CosineInterpolate(T y0, T y1, float t)

// [todo] template <typename T> T CubicInterpolate(T y0, T y1, T y2, T y3, float t)

// [todo] template <typename T> T HermiteInterpolate(T y0, T y1, T y2, T y3, float t, float tension, float bias)

// [todo] template <class T> T LinearInterpolateArray(uint32_t count, const T *values, float t)
}

// 
void test_units() {
	TEST_CHECK(TestEqual(Deg(60.f), Pi/3.f));
	TEST_CHECK(TestEqual(Deg(90.f), HalfPi));
	TEST_CHECK(TestEqual(Deg(120.f), TwoPi / 3.f));
	TEST_CHECK(TestEqual(Deg(180.f), Pi));
	TEST_CHECK(TestEqual(Deg(315.f), 7.f*Pi/4.f));
	TEST_CHECK(TestEqual(Deg(360.f), TwoPi));

	TEST_CHECK(TestEqual(DegreeToRadian(60.f), Pi / 3.f));
	TEST_CHECK(TestEqual(DegreeToRadian(90.f), HalfPi));
	TEST_CHECK(TestEqual(DegreeToRadian(120.f), TwoPi / 3.f));
	TEST_CHECK(TestEqual(DegreeToRadian(180.f), Pi));
	TEST_CHECK(TestEqual(DegreeToRadian(315.f), 7.f * Pi / 4.f));
	TEST_CHECK(TestEqual(DegreeToRadian(360.f), TwoPi));

	TEST_CHECK(TestEqual(RadianToDegree(Pi / 3.f), 60.f));
	TEST_CHECK(TestEqual(RadianToDegree(HalfPi), 90.f));
	TEST_CHECK(TestEqual(RadianToDegree(TwoPi / 3.f), 120.f));
	TEST_CHECK(TestEqual(RadianToDegree(Pi), 180.f));
	TEST_CHECK(TestEqual(RadianToDegree(7.f * Pi / 4.f), 315.f));
	TEST_CHECK(TestEqual(RadianToDegree(TwoPi), 360.f));
		
// [todo] template <typename T> constexpr T Sec(T v)

// [todo] template <typename T> constexpr T Csec(T v)
// [todo] template <typename T> constexpr T Ms(T v)

// [todo] template <typename T> constexpr T Ton(T v) { return v * T(1000); }
// [todo] template <typename T> constexpr T Kg(T v) { return v; }

// [todo] template <typename T> constexpr T G(T v)

// [todo] template <typename T> constexpr T Km(T v)
// [todo] template <typename T> constexpr T Mtr(T v)

// [todo] template <typename T> constexpr T Cm(T v)
// [todo] template <typename T> constexpr T Mm(T v)
// [todo] template <typename T> constexpr T Inch(T v)

// [todo] inline constexpr size_t KB(const size_t size)
// [todo] inline constexpr size_t MB(const size_t size)

// [todo] template <typename T> std::string FormatMemorySize(T v_)

// [todo] template <typename T> std::string FormatCount(T v_)

// [todo] template <typename T> std::string FormatDistance(T v_)

// [todo] std::string FormatTime(time_ns t)

}

//
void test_clock_update() {
	reset_clock();

	TEST_CHECK(get_clock() == 0);
	TEST_CHECK(get_clock_dt() == 1);

	tick_clock();
	sleep_for(time_from_ms(16));
	tick_clock();

	TEST_CHECK(time_to_us(get_clock_dt()) >= 15000);

	for (int n = 0; n < 16; ++n) {
		sleep_for(time_from_ms(16));
		tick_clock();
	}
}

//
void test_vec2_mul() {
	Vec2 a(2.f, 4.f);
	Vec2 b(1.f, 2.f);
	TEST_CHECK(b * 2.f == a);
	TEST_CHECK(2.f * b == a);
}

//
void test_vec3_zero() {
	Vec3 zero(0, 0, 0);
	TEST_CHECK(Len(zero) == 0);
}

void test_vec3_mul() {
	Vec3 a(2.f, 4.f, 6.f);
	Vec3 b(1.f, 2.f, 3.f);
	TEST_CHECK(b * 2.f == a);
	TEST_CHECK(2.f * b == a);
}

//
void test_vec4_mul() {
	Vec4 a(3.f, 6.f, 9.f, 12.f);
	Vec4 b(1.f, 2.f, 3.f, 4.f);
	TEST_CHECK(b * 3.f == a);
	TEST_CHECK(3.f * b == a);
}

//
void test_log() {
	log("Calling Log()");
	warn("Calling Warn()");
	error("Calling Error()");
}

//
TEST_LIST = {
	{"Math", test_math},
	{"Units", test_units},

	{"Clock.Update", test_clock_update},

	{"Vec2.Mul", test_vec2_mul},

	{"Vec3.Zero", test_vec3_zero},
	{"Vec3.Mul", test_vec3_mul},

	{"Vec4.Mul", test_vec4_mul},

//	{"Log", test_log},

	{NULL, NULL},
};
