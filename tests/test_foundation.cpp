// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.
#include <math.h>
#include <utf8.h>

#include "acutest.h"

#include "foundation/string.h"

#include "foundation/time.h"
#include "foundation/clock.h"
#include "foundation/log.h"

#include <fmt/format.h>
#include <map>

using namespace hg;

//
extern void test_os();
extern void test_math();
extern void test_units();
extern void test_string();
extern void test_path_tools();
extern void test_vec2();
extern void test_vec3();
extern void test_vec4();
extern void test_mat3();
extern void test_mat4();
extern void test_mat44();
extern void test_color();
extern void test_minmax();
extern void test_plane();
extern void test_projection();
extern void test_obb();
extern void test_easing();
extern void test_quaternion();
extern void test_vector_list();

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
	{"OS", test_os},
	{"Math", test_math},
	{"Units", test_units},
	{"String", test_string},
	{"Path_tools", test_path_tools},
	{"Vec2", test_vec2},
	{"Vec3", test_vec3},
	{"Vec4", test_vec4},
	{"Mat3", test_mat3},
	{"Mat4", test_mat4},
	{"Mat44", test_mat44},
	{"Color", test_color},
	{"MinMax", test_minmax},
	{"Plane", test_plane},
	{"Projection", test_projection},
	{"OBB", test_obb},
	{"Easing", test_easing},
    {"Quaternion", test_quaternion},
	{"Vector_list", test_vector_list},

	{"Clock.Update", test_clock_update},

//	{"Log", test_log},

	{NULL, NULL},
};
