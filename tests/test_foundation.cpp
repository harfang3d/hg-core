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

	TEST_CHECK(TestEqual(Sin(0.f), 0.f));
	TEST_CHECK(TestEqual(Sin(Pi / 6.f), 1.f / 2.f));
	TEST_CHECK(TestEqual(Sin(Pi / 4.f), float(sqrt(2.0) / 2.0)));
	TEST_CHECK(TestEqual(Sin(Pi / 3.f), float(sqrt(3.0) / 2.0)));
	TEST_CHECK(TestEqual(Sin(HalfPi), 1.f));
	TEST_CHECK(TestEqual(Sin(Pi), 0.f));
	TEST_CHECK(TestEqual(Sin(TwoPi), 0.f));
	TEST_CHECK(TestEqual(Sin(3.f * Pi / 2.f), -1.f));
	TEST_CHECK(TestEqual(Sin(-HalfPi), -1.f));
	TEST_CHECK(TestEqual(Sin(-1.5f), -Sin(1.5f)));
	TEST_CHECK(TestEqual(Sin(1.5f - Pi), -Sin(1.5f)));
	TEST_CHECK(TestEqual(Sin(0.4f + Pi), -Sin(0.4f)));

	TEST_CHECK(TestEqual(ASin(0.f), 0.f));
	TEST_CHECK(TestEqual(ASin(0.5f), Pi / 6.f));
	TEST_CHECK(TestEqual(ASin(float(sqrt(2.0) / 2.0)), Pi / 4.f));
	TEST_CHECK(TestEqual(ASin(float(sqrt(3.0) / 2.0)), Pi / 3.f));
	TEST_CHECK(TestEqual(ASin(1.f), HalfPi));
	TEST_CHECK(TestEqual(ASin(-1.f), -HalfPi));
	TEST_CHECK(TestEqual(ASin(-0.7f), -ASin(0.7f)));

	TEST_CHECK(TestEqual(Cos(0.f), 1.f));
	TEST_CHECK(TestEqual(Cos(Pi / 6.f), float(sqrt(3.0) / 2.f)));
	TEST_CHECK(TestEqual(Cos(Pi / 4.f), float(sqrt(2.0) / 2.0)));
	TEST_CHECK(TestEqual(Cos(Pi / 3.f), 1.0 / 2.0));
	TEST_CHECK(TestEqual(Cos(HalfPi), 0.f));
	TEST_CHECK(TestEqual(Cos(Pi),-1.f));
	TEST_CHECK(TestEqual(Cos(TwoPi), 1.f));
	TEST_CHECK(TestEqual(Cos(3.f * Pi / 2.f), 0.f));
	TEST_CHECK(TestEqual(Cos(-1.5f), Cos(1.5f)));
	TEST_CHECK(TestEqual(Cos(1.5f - Pi), -Cos(1.5f)));
	TEST_CHECK(TestEqual(Cos(2.6f - Pi), -Cos(2.6f)));

	TEST_CHECK(TestEqual(Cos(0.7f + HalfPi),-Sin(0.7f)));
	TEST_CHECK(TestEqual(Cos(2.5f - HalfPi), Sin(2.5f)));
	TEST_CHECK(TestEqual(Sin(0.3f + HalfPi), Cos(0.3f)));
	TEST_CHECK(TestEqual(Sin(1.8f - HalfPi),-Cos(1.8f)));

	TEST_CHECK(TestEqual(ACos(1.f), 0.f));
	TEST_CHECK(TestEqual(ACos(float(sqrt(3.0) / 2.f)), Pi / 6.f));
	TEST_CHECK(TestEqual(ACos(float(sqrt(2.0) / 2.0)), Pi / 4.f));
	TEST_CHECK(TestEqual(ACos(1.0 / 2.0), Pi / 3.f));
	TEST_CHECK(TestEqual(ACos(0.f), HalfPi));
	TEST_CHECK(TestEqual(ACos(-1.f), Pi));

	TEST_CHECK(TestEqual(ACos(-0.7f), HalfPi + ASin(0.7f)));

	TEST_CHECK(TestEqual(Tan(0.f), 0.f));
	TEST_CHECK(TestEqual(Tan(Pi / 6.f), float(sqrt(3.0) / 3.0)));
	TEST_CHECK(TestEqual(Tan(Pi / 4.f), 1.f));
	TEST_CHECK(TestEqual(Tan(Pi / 3.f), float(sqrt(3.0))));
	TEST_CHECK(TestEqual(Tan(-1.4f), -Tan(1.4f)));
	TEST_CHECK(TestEqual(Tan(0.6f + Pi), Tan(0.6f)));

	TEST_CHECK(TestEqual(ATan(0.f), 0.f));
	TEST_CHECK(TestEqual(ATan(float(sqrt(3.0) / 3.0)), Pi / 6.f));
	TEST_CHECK(TestEqual(ATan(1.f), Pi / 4.f));
	TEST_CHECK(TestEqual(ATan(float(sqrt(3.0))), Pi / 3.f));
	TEST_CHECK(TestEqual(ATan(-1.4f), -ATan(1.4f)));
	TEST_CHECK(TestEqual(ATan(1.f/0.4f) + ATan(0.4f), HalfPi));

	TEST_CHECK(TestEqual(LinearInterpolate(-1.f, 1.f, 0.5f), 0.f));
	TEST_CHECK(TestEqual(LinearInterpolate(102, 204, 0), 102));
	TEST_CHECK(TestEqual(LinearInterpolate(102, 204, 1), 204));
	TEST_CHECK(TestEqual(LinearInterpolate(16, 32, 0.5), 24));
	TEST_CHECK(TestEqual(LinearInterpolate(-5.f, 5.f, 0.1f), -4.f));
	TEST_CHECK(TestEqual(LinearInterpolate(-5.f, 5.f, 0.9f), 4.f));
	TEST_CHECK(TestEqual(LinearInterpolate( 8.f, 9.f, 1.1f), 9.1f));
	TEST_CHECK(TestEqual(LinearInterpolate( 6.f, 8.f,-0.1f), 5.8f));

	TEST_CHECK(TestEqual(CosineInterpolate(-1.f, 1.f, 0.0f),-1.f));
	TEST_CHECK(TestEqual(CosineInterpolate(-1.f, 1.f, 1.0f), 1.f));
	TEST_CHECK(TestEqual(CosineInterpolate( 1.f, 3.f, 0.5f), 2.f));
	TEST_CHECK(TestEqual(CosineInterpolate( 2.f, 4.f, 2.0f), 2.f));
	TEST_CHECK(TestEqual(CosineInterpolate( 2.f, 4.f, 3.0f), 4.f));
	TEST_CHECK(TestEqual(CosineInterpolate(-2.f, 2.f, 2.5f), 0.f));
	TEST_CHECK(TestEqual(CosineInterpolate(-2.f, 2.f, 1.f / 3.f), -1.f));
	TEST_CHECK(TestEqual(CosineInterpolate(-2.f, 2.f, 7.f / 3.f), -1.f));
	TEST_CHECK(TestEqual(CosineInterpolate(-2.f, 2.f, 4.f / 3.f), 1.f));
	
	const float tab[5] = {-2.f,-1.f,0.f,1.f,2.f};
	TEST_CHECK(TestEqual(LinearInterpolateArray(5, tab, 0.0f),-2.0f));
	TEST_CHECK(TestEqual(LinearInterpolateArray(5, tab, 1.0f), 2.0f));
	TEST_CHECK(TestEqual(LinearInterpolateArray(5, tab, 0.5f), 0.0f));
	TEST_CHECK(TestEqual(LinearInterpolateArray(5, tab, 0.25f),-1.0f));
	TEST_CHECK(TestEqual(LinearInterpolateArray(5, tab, 0.75f), 1.0f));
	TEST_CHECK(TestEqual(LinearInterpolateArray(5, tab, 0.125f),-1.5f));
	TEST_CHECK(TestEqual(LinearInterpolateArray(5, tab, 0.875f), 1.5f));
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
	
	TEST_CHECK(TestEqual(Csec(1.0f), 0.01f));
	TEST_CHECK(TestEqual(Csec(100.f), 1.f));

	TEST_CHECK(TestEqual(Ms(1.0f), 0.001f));
	TEST_CHECK(TestEqual(Ms(1000.f), 1.f));

	TEST_CHECK(TestEqual(Ton(2.f), 2000.f));
	TEST_CHECK(TestEqual(Ton(0.001f), 1.f));

	TEST_CHECK(TestEqual(Kg(2.f), 2.f));

	TEST_CHECK(TestEqual(G(2.f), 0.002f));
	TEST_CHECK(TestEqual(G(1000.f), 1.f));

	TEST_CHECK(TestEqual(Km(1.f), 1000.f));
	TEST_CHECK(TestEqual(Km(0.001f), 1.f));

	TEST_CHECK(TestEqual(Mtr(1.f), 1.f));

	TEST_CHECK(TestEqual(Cm(1.f), 0.01f));
	TEST_CHECK(TestEqual(Cm(100.f), 1.f));

	TEST_CHECK(TestEqual(Mm(1.f), 0.001f));
	TEST_CHECK(TestEqual(Mm(1000.f), 1.f));
	
	TEST_CHECK(TestEqual(Inch(1.f), 0.0254f));
	TEST_CHECK(TestEqual(Inch(39.3701f), 1.f));
	
	TEST_CHECK(TestEqual(KB(1), 1024));
	TEST_CHECK(TestEqual(KB(1024), MB(1)));

	TEST_CHECK(FormatMemorySize(512) == "512B");
	TEST_CHECK(FormatMemorySize(KB(1)) == "1.0KB");
	TEST_CHECK(FormatMemorySize(MB(1)) == "1.0MB");
	TEST_CHECK(FormatMemorySize(MB(1024)) == "1.0GB");
	TEST_CHECK(FormatMemorySize(MB(1024*1024)) == "1.0TB");
	TEST_CHECK(FormatMemorySize(KB(1) + 512) == "1.5KB");
	TEST_CHECK(FormatMemorySize(MB(2048+768)) == "2.7GB");
	TEST_CHECK(FormatMemorySize(MB(1) + KB(512)) == "1.5MB");

	TEST_CHECK(FormatCount(1) == "1");
	TEST_CHECK(FormatCount(10) == "10");
	TEST_CHECK(FormatCount(1000) == "1.0K");
	TEST_CHECK(FormatCount(12345) == "12.3K");
	TEST_CHECK(FormatCount(2000000) == "2.0M");
	TEST_CHECK(FormatCount(5555555) == "5.5M");

	TEST_CHECK(FormatDistance(Mm(1.f)) == "1.0mm");
	TEST_CHECK(FormatDistance(Mm(8.2f)) == "8.2mm");
	TEST_CHECK(FormatDistance(Cm(1.f)) == "10.0mm");
	TEST_CHECK(FormatDistance(Cm(53.5f)) == "53.5cm");
	TEST_CHECK(FormatDistance(45.3f) == "45.3m");
	TEST_CHECK(FormatDistance(Km(1.f)) == "1.0km");
	TEST_CHECK(FormatDistance(Km(12.345)) == "12.3km");

	TEST_CHECK(FormatTime(time_from_sec(1) + time_from_ms(100)) == "1 sec 100 ms");
	TEST_CHECK(FormatTime(time_from_min(30) + time_from_sec(26) + time_from_ms(3)) == "30 min 26 sec 3 ms");
	TEST_CHECK(FormatTime(time_from_hour(2) + time_from_min(15) + time_from_sec(30) + time_from_ms(222)) == "2 hour 15 min 30 sec 222 ms");
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
