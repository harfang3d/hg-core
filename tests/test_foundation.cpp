// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.
#include <math.h>
#include <utf8.h>

#include "acutest.h"

#include "foundation/string.h"

#include "foundation/time.h"
#include "foundation/clock.h"
#include "foundation/log.h"

#include <fmt/format.h>

using namespace hg;

//
extern void test_math();
extern void test_units();
extern void test_string();
extern void test_path_tools();
extern void test_vec2();
extern void test_vec3();

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
void test_log() {
	log("Calling Log()");
	warn("Calling Warn()");
	error("Calling Error()");
}

//
TEST_LIST = {
	{"Math", test_math},
	{"Units", test_units},
	{"String", test_string},
	{"Path_tools", test_path_tools},
	{"Vec2", test_vec2},
	{"Vec3", test_vec3},

	{"Clock.Update", test_clock_update},

//	{"Log", test_log},

	{NULL, NULL},
};
