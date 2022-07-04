// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "acutest.h"

#include "foundation/time.h"
#include "foundation/clock.h"
#include "foundation/vector2.h"
#include "foundation/vector3.h"
#include "foundation/vector4.h"
#include "foundation/log.h"

using namespace hg;

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
	{"Clock.Update", test_clock_update},

	{"Vec2.Mul", test_vec2_mul},

	{"Vec3.Zero", test_vec3_zero},
	{"Vec3.Mul", test_vec3_mul},

	{"Vec4.Mul", test_vec4_mul},

//	{"Log", test_log},

	{NULL, NULL},
};
