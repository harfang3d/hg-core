// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#define TEST_NO_MAIN
#include "acutest.h"

#include <string>

#include "engine/anim.h"

#include "foundation/math.h"
#include "foundation/unit.h"
#include "foundation/time.h"

using namespace hg;

static void test_anim_bool_track() {
	AnimTrackT<bool> bool_track;

	hg::time_ns t[4] = {time_from_ms(100), time_from_ms(400), time_from_ms(800), time_from_ms(1200)};

	TEST_CHECK(GetKey(bool_track, t[0]) == InvalidKeyIdx);

	SetKey(bool_track, t[3], false);
	SetKey(bool_track, t[0], false);
	SetKey(bool_track, t[2], true);
	SetKey(bool_track, t[1], true);

	SetKey(bool_track, t[2], false);
	SetKey(bool_track, t[3], true);

	int idx;
	idx = GetKey(bool_track, t[0]);
	TEST_CHECK(idx != InvalidKeyIdx);
	TEST_CHECK(bool_track.keys[idx].t == t[0]);
	TEST_CHECK(bool_track.keys[idx].v == false);

	idx = GetKey(bool_track, t[3]);
	TEST_CHECK(idx != InvalidKeyIdx);
	TEST_CHECK(bool_track.keys[idx].t == t[3]);
	TEST_CHECK(bool_track.keys[idx].v == true);

	hg::time_ns not_here = time_from_ms(1000);
	TEST_CHECK(GetKey(bool_track, not_here) == InvalidKeyIdx);
	DeleteKey(bool_track, not_here);
	TEST_CHECK(GetKey(bool_track, not_here) == InvalidKeyIdx);

	DeleteKey(bool_track, t[3]);
	TEST_CHECK(GetKey(bool_track, t[3]) == InvalidKeyIdx);

	int k[2];
	bool ret;
	ret = GetIntervalKeys<AnimTrackT<bool>, bool>(bool_track, time_ns(t[1] + time_from_ms(200)), k[0], k[1]);
	TEST_CHECK(ret == true);
	TEST_CHECK(k[0] == 1);
	TEST_CHECK(k[1] == 2);

	ret = GetIntervalKeys<AnimTrackT<bool>, bool>(bool_track, time_from_ms(2000), k[0], k[1]);
	TEST_CHECK(ret == false);
	TEST_CHECK(k[0] == 2);

	ret = GetIntervalKeys<AnimTrackT<bool>, bool>(bool_track, time_from_ms(20), k[0], k[1]);
	TEST_CHECK(ret == false);
	TEST_CHECK(k[0] == 0);

	bool value;

	TEST_CHECK(Evaluate(AnimTrackT<bool>(), 0, value) == false);
	
	TEST_CHECK(Evaluate(bool_track, time_from_ms(0), value) == true);
	TEST_CHECK(value == false);

	TEST_CHECK(Evaluate(bool_track, time_from_ms(50), value) == true);
	TEST_CHECK(value == false);

	TEST_CHECK(Evaluate(bool_track, time_from_ms(100), value) == true);
	TEST_CHECK(value == false);

	TEST_CHECK(Evaluate(bool_track, time_from_ms(300), value) == true);
	TEST_CHECK(value == false);

	TEST_CHECK(Evaluate(bool_track, time_from_ms(400), value) == true);
	TEST_CHECK(value == true);

	TEST_CHECK(Evaluate(bool_track, time_from_ms(600), value) == true);
	TEST_CHECK(value == true);

	TEST_CHECK(Evaluate(bool_track, time_from_ms(900), value) == true);
	TEST_CHECK(value == false);

	TEST_CHECK(Evaluate(bool_track, time_from_ms(1500), value) == true);
	TEST_CHECK(value == false);
}

static void test_anim_string_track() {
	AnimTrackT<std::string> str_track;

	hg::time_ns t[4] = {time_from_ms(100), time_from_ms(400), time_from_ms(800), time_from_ms(1200)};

	str_track.keys.resize(4);
	str_track.keys[0].t = t[3];
	str_track.keys[0].v = "Key #3";

	str_track.keys[1].t = t[0];
	str_track.keys[1].v = "Key #0";

	str_track.keys[2].t = t[2];
	str_track.keys[2].v = "Key #2";

	str_track.keys[3].t = t[1];
	str_track.keys[3].v = "Key #1";

	SortAnimTrackKeys(str_track);

	for (size_t i = 1; i < str_track.keys.size(); i++) {
		TEST_CHECK(str_track.keys[i - 1].t < str_track.keys[i].t);
	}

	std::string value;
	TEST_CHECK(Evaluate(AnimTrackT<std::string>(), time_from_ms(900), value) == false);

	TEST_CHECK(Evaluate(str_track, time_from_ms(0), value) == true);
	TEST_CHECK(value == "Key #0");

	TEST_CHECK(Evaluate(str_track, time_from_ms(50), value) == true);
	TEST_CHECK(value == "Key #0");

	TEST_CHECK(Evaluate(str_track, time_from_ms(180), value) == true);
	TEST_CHECK(value == "Key #0");

	TEST_CHECK(Evaluate(str_track, time_from_ms(400), value) == true);
	TEST_CHECK(value == "Key #1");

	TEST_CHECK(Evaluate(str_track, time_from_ms(720), value) == true);
	TEST_CHECK(value == "Key #1");

	TEST_CHECK(Evaluate(str_track, time_from_ms(1040), value) == true);
	TEST_CHECK(value == "Key #2");

	TEST_CHECK(Evaluate(str_track, time_from_ms(1600), value) == true);
	TEST_CHECK(value == "Key #3");
}

static void test_anim_int_track() { 
	AnimTrackT<int> int_track; 

	int value;
	TEST_CHECK(Evaluate(int_track, time_from_ms(500), value) == false);

	hg::time_ns t[4] = {time_from_ms(100), time_from_ms(200), time_from_ms(300), time_from_ms(400)};

	int i0 = -10;
	int i1 = -5;
	int i2 = 5;
	int i3 = 10;
		
	int_track.keys.resize(4);

	int_track.keys[0].t = t[3];
	int_track.keys[0].v = i3;

	int_track.keys[1].t = t[2];
	int_track.keys[1].v = i2;

	int_track.keys[2].t = t[1];
	int_track.keys[2].v = i1;

	int_track.keys[3].t = t[0];
	int_track.keys[3].v = i0;

	SortAnimTrackKeys(int_track);

	TEST_CHECK(Evaluate(int_track, time_from_ms(20), value) == true);
	TEST_CHECK(value == i0);

	TEST_CHECK(Evaluate(int_track, time_from_ms(100), value) == true);
	TEST_CHECK(value == i0);

	TEST_CHECK(Evaluate(int_track, time_from_ms(150), value) == true);
	TEST_CHECK(value == (i0+i1)/2);

	TEST_CHECK(Evaluate(int_track, time_from_ms(200), value) == true);
	TEST_CHECK(value == i1);

	TEST_CHECK(Evaluate(int_track, time_from_ms(400), value) == true);
	TEST_CHECK(value == i3);

	TEST_CHECK(Evaluate(int_track, time_from_ms(1000), value) == true);
	TEST_CHECK(value == i3);
}

static void test_anim_vec3_track() {
	AnimTrackHermiteT<Vec3> vec3_track;

	Vec3 value;
	TEST_CHECK(Evaluate(vec3_track, time_from_ms(600), value) == false);

	hg::time_ns t[4] = {time_from_ms(200), time_from_ms(400), time_from_ms(600), time_from_ms(800)};

	Vec3 v0(-1.f, -1.f, -1.f);
	Vec3 v1(-1.f, 0.f, -1.f);
	Vec3 v2(0.f, 1.f, 0.f);
	Vec3 v3(0.f, 1.f, 0.f);

	vec3_track.keys.resize(4);

	vec3_track.keys[0].t = t[3];
	vec3_track.keys[0].v = v3;
	vec3_track.keys[0].bias = 0.f;
	vec3_track.keys[0].tension = 0.f;

	vec3_track.keys[1].t = t[2];
	vec3_track.keys[1].v = v2;
	vec3_track.keys[1].bias = 0.75f;
	vec3_track.keys[1].tension = 0.25f;

	vec3_track.keys[2].t = t[1];
	vec3_track.keys[2].v = v1;
	vec3_track.keys[2].bias = 0.25f;
	vec3_track.keys[2].tension = 0.75f;

	vec3_track.keys[3].t = t[0];
	vec3_track.keys[3].v = v0;
	vec3_track.keys[3].bias = 0.5f;
	vec3_track.keys[3].tension = 0.5f;

	SortAnimTrackKeys(vec3_track);

	TEST_CHECK(Evaluate(vec3_track, time_from_ms(100), value) == true);
	TEST_CHECK(value == v0);

	TEST_CHECK(Evaluate(vec3_track, time_from_ms(200), value) == true);
	TEST_CHECK(value == v0);

	TEST_CHECK(Evaluate(vec3_track, time_from_ms(400), value) == true);
	TEST_CHECK(value == v1);

	TEST_CHECK(Evaluate(vec3_track, time_from_ms(600), value) == true);
	TEST_CHECK(value == v2);

	TEST_CHECK(Evaluate(vec3_track, time_from_ms(800), value) == true);
	TEST_CHECK(value == v3);

	TEST_CHECK(Evaluate(vec3_track, time_from_ms(1000), value) == true);
	TEST_CHECK(value == v3);

	ResampleAnimTrack(vec3_track, time_from_ms(100), time_from_ms(50), 1000000, time_from_ms(16));

	TEST_CHECK(vec3_track.keys[0].t == time_from_ms(144));
	TEST_CHECK(vec3_track.keys[1].t == time_from_ms(352));
	TEST_CHECK(vec3_track.keys[2].t == time_from_ms(544));
	TEST_CHECK(vec3_track.keys[3].t == time_from_ms(752));

	ResampleAnimTrack(vec3_track, time_from_ms(120), time_from_ms(100), 1000000, time_from_ms(220));

	TEST_CHECK(vec3_track.keys.size() == 3);

	TEST_CHECK(vec3_track.keys[0].t == time_from_ms(220));
	TEST_CHECK(vec3_track.keys[1].t == time_from_ms(440));
	TEST_CHECK(vec3_track.keys[2].t == time_from_ms(660));

	vec3_track.keys.clear();

	SetKey(vec3_track, time_from_ms(0), Vec3(0.f, 1.f, 0.f));
	SetKey(vec3_track, time_from_ms(50), Vec3(0.2f, 0.8f,-0.2f));
	SetKey(vec3_track, time_from_ms(100), Vec3(0.4f, 0.6f,-0.4f));
	SetKey(vec3_track, time_from_ms(150), Vec3(0.6f, 0.4f,-0.6f));
	SetKey(vec3_track, time_from_ms(200), Vec3(0.8f, 0.2f,-0.8f));
	SetKey(vec3_track, time_from_ms(250), Vec3(1.0f, 0.0f,-1.0f));

	size_t removed = SimplifyAnimTrackT<AnimTrackHermiteT<Vec3> >(vec3_track, 0.1f);
	TEST_CHECK(removed == 4);

	TEST_CHECK(vec3_track.keys[0].t == time_from_ms(0));
	TEST_CHECK(vec3_track.keys[1].t == time_from_ms(250));
}

static void test_anim_float_track() {
	AnimTrackT<float> track;

	SetKey(track, time_from_ms(100), 0.f);
	SetKey(track, time_from_ms(200), 1.f);
	SetKey(track, time_from_ms(300), 2.f);
	SetKey(track, time_from_ms(400), 3.f);
	SetKey(track, time_from_ms(500), 4.f);
	SetKey(track, time_from_ms(600), 3.f);
	SetKey(track, time_from_ms(700), 2.f);
	SetKey(track, time_from_ms(800), 1.f);
	SetKey(track, time_from_ms(900), 0.f);
	
	size_t removed = SimplifyAnimTrackT<AnimTrackT<float> >(track, 0.00001f);
	TEST_CHECK(removed == 6);

	TEST_CHECK((track.keys[0].t == time_from_ms(100)) && (track.keys[0].v == 0.f));
	TEST_CHECK((track.keys[1].t == time_from_ms(500)) && (track.keys[1].v == 4.f));
	TEST_CHECK((track.keys[2].t == time_from_ms(900)) && (track.keys[2].v == 0.f));

	SetKey(track, time_from_ms(500), 0.f);

	removed = SimplifyAnimTrackT<AnimTrackT<float> >(track, 0.00001f);
	TEST_CHECK(removed == 2);
	TEST_CHECK((track.keys[0].t == time_from_ms(100)) && (track.keys[0].v == 0.f));
}

void test_misc() {
	Vec3 axis = Normalize(Vec3::One);
	Quaternion q0 = QuaternionFromAxisAngle(Deg(300.f), axis);
	Quaternion q1 = QuaternionFromAxisAngle(Deg(-60.f), axis);
	Quaternion q2 = QuaternionFromAxisAngle(Deg(30.f), axis);
	Quaternion q3 = QuaternionFromAxisAngle(Deg(170.f), Vec3::Back);

	TEST_CHECK(CompareKeyValue(q0, q1, 0.00001f) == true);
	TEST_CHECK(CompareKeyValue(q1, q2, 0.00001f) == false);
	TEST_CHECK(CompareKeyValue(q2, q3, 0.00001f) == false);
	
	TEST_CHECK(CompareKeyValue(Color::Orange, Color::Yellow, 0.00001f) == false);
	TEST_CHECK(CompareKeyValue(Color(0.7f, 0.2f, 0.1f), Color(0.69f, 0.18f, 0.11f), 0.02f) == true);	
}

void test_anim() {
	test_anim_bool_track();
	test_anim_string_track();
	test_anim_int_track();
	test_anim_vec3_track();
	test_anim_float_track();
	test_misc();

	Anim anim;
	anim.t_start = time_from_ms(50);
	anim.t_end   = time_from_ms(1200);
	// [todo] anim.flags seems unused atm.

	TEST_CHECK(AnimHasKeys(anim) == false);

	anim.bool_tracks.resize(2);
	anim.float_tracks.resize(1);

	SetKey(anim.bool_tracks[0], time_from_ms(50), false);
	SetKey(anim.bool_tracks[0], time_from_ms(100), true);
	SetKey(anim.bool_tracks[0], time_from_ms(1000), false);
	
	SetKey(anim.float_tracks[0], time_from_ms(50), 0.f);
	SetKey(anim.float_tracks[0], time_from_ms(100), 0.2f);
	SetKey(anim.float_tracks[0], time_from_ms(400), 1.f);
	SetKey(anim.float_tracks[0], time_from_ms(1000), 0.2f);
	SetKey(anim.float_tracks[0], time_from_ms(1200), 0.f);

	TEST_CHECK(AnimHasKeys(anim) == true);

	DeleteEmptyAnimTracks(anim);
	TEST_CHECK(anim.bool_tracks.size() == 1);

	// [todo]
	// InstanceAnimKey 
	// void ResampleAnim(Anim & anim, time_ns old_start, time_ns old_end, time_ns new_start, time_ns new_end, time_ns frame_duration);
	// void ReverseAnim(Anim & anim, time_ns t_start, time_ns t_end);
	// void QuantizeAnim(Anim & anim, time_ns t_step);
}
