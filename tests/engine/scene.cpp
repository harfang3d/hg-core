// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#define TEST_NO_MAIN
#include "acutest.h"

#include "engine/scene.h"

#include "foundation/log.h"

#include "engine/file_format.h"

using namespace hg;

static void on_log(const std::string &, int mask, const std::string &, void *user) {
	int *ptr = reinterpret_cast<int *>(user);
	*ptr = mask;
}

static void check_probe(Data &in) {
	Probe dflt;
	
	std::string str;

	TEST_CHECK(Read(in, str) == true); // irradiance map name
	TEST_CHECK(str.empty());

	TEST_CHECK(Read(in, str) == true); // radiance map name
	TEST_CHECK(str.empty());

	ProbeType type;
	TEST_CHECK(Read<ProbeType>(in, type) == true); // probe type
	TEST_CHECK(type == dflt.type);

	uint8_t parallax;
	TEST_CHECK(Read<uint8_t>(in, parallax) == true); // parallax
	TEST_CHECK(parallax == dflt.parallax);

	TransformTRS trs;
	TEST_CHECK(Read<TransformTRS>(in, trs) == true); // transform
	TEST_CHECK(trs.pos == dflt.trs.pos);
	TEST_CHECK(trs.rot == dflt.trs.rot);
	TEST_CHECK(trs.scl == dflt.trs.scl);
}

static void check_canvas(Data &in) {
	Scene::Canvas dflt;

	bool b;

	TEST_CHECK(Read<bool>(in, b) == true); // clear z
	TEST_CHECK(b == dflt.clear_z);

	TEST_CHECK(Read<bool>(in, b) == true); // clear color
	TEST_CHECK(b == dflt.clear_color);

	Color col;

	TEST_CHECK(Read<Color>(in, col) == true); // clear color
	TEST_CHECK(col == dflt.color);
}

static void check_environment(Data &in) {
	Scene::Environment dflt;

	Color col;
	float f;
	TEST_CHECK(Read<Color>(in, col) == true); // ambient
	TEST_CHECK(col == dflt.ambient);

	TEST_CHECK(Read<float>(in, f) == true); // fog near
	TEST_CHECK(f == dflt.fog_near);

	TEST_CHECK(Read<float>(in, f) == true); // fog far
	TEST_CHECK(f == dflt.fog_far);

	TEST_CHECK(Read<Color>(in, col) == true); // fog color
	TEST_CHECK(col == dflt.fog_color);

	check_probe(in);
	
	std::string str;
	TEST_CHECK(Read(in, str) == true); // brdf map name
	TEST_CHECK(str.empty());

	check_canvas(in);
}

static void test_scene_binary_serialization() {
	int mask = 0; // here mask will serve as an errno substitute.

	Data d0;

	{
		Scene scene;
		PipelineResources resources;
		TEST_CHECK(SaveSceneBinaryToData(d0, scene, resources) == true);
	}

	d0.Rewind();

	uint32_t u32;
	uint16_t u16;
	uint8_t u8;
	
	uint32_t flags;

	TEST_CHECK(Read<uint32_t>(d0, u32) == true);
	TEST_CHECK(u32 == HarfangMagic); // harfang magic

	TEST_CHECK(Read<uint8_t>(d0, u8) == true);
	TEST_CHECK(u8 == SceneMarker); // scene marker

	TEST_CHECK(Read<uint32_t>(d0, u32) == true);
	TEST_CHECK(u32 == GetSceneBinaryFormatVersion()); // binary version

	TEST_CHECK(Read<uint32_t>(d0, flags) == true);
	TEST_CHECK(flags == LSSF_All); // save flags

	TEST_CHECK(Read<uint32_t>(d0, u32) == true); // Transform
	TEST_CHECK(u32 == 0);

	TEST_CHECK(Read<uint32_t>(d0, u32) == true); // Camera
	TEST_CHECK(u32 == 0);

	TEST_CHECK(Read<uint32_t>(d0, u32) == true); // Object
	TEST_CHECK(u32 == 0);

	TEST_CHECK(Read<uint32_t>(d0, u32) == true); // Light
	TEST_CHECK(u32 == 0);

	if (flags & LSSF_Physics) {
		TEST_CHECK(Read<uint32_t>(d0, u32) == true); // Rigid body
		TEST_CHECK(u32 == 0);

		TEST_CHECK(Read<uint32_t>(d0, u32) == true); // Collision
		TEST_CHECK(u32 == 0);
	}

	if (flags & LSSF_Scripts) {
		TEST_CHECK(Read<uint32_t>(d0, u32) == true); // Scripts
		TEST_CHECK(u32 == 0);
	}

	TEST_CHECK(Read<uint32_t>(d0, u32) == true); // Instance
	TEST_CHECK(u32 == 0);

	if (flags & LSSF_Nodes) {
		TEST_CHECK(Read<uint32_t>(d0, u32) == true); // Node
		TEST_CHECK(u32 == 0);
	}

	if (flags & LSSF_Scene) {
		if (flags & LSSF_Nodes) {
			gen_ref dflt;
			TEST_CHECK(Read<uint32_t>(d0, u32) == true); // Current camera idx
			TEST_CHECK(u32 == dflt.idx);
		}
		check_environment(d0);		
	}

	if (flags & LSSF_Anims) {
		TEST_CHECK(Read<uint32_t>(d0, u32) == true); // Chunk size
		TEST_CHECK(u32 == 8);

		TEST_CHECK(Read<uint32_t>(d0, u32) == true); // AnimRef count
		TEST_CHECK(u32 == 0);

		TEST_CHECK(Read<uint32_t>(d0, u32) == true); // SceneAnimRef count
		TEST_CHECK(u32 == 0);
	}

	if (flags & LSSF_KeyValues) {
		TEST_CHECK(Read<uint32_t>(d0, u32) == true); // Keys
		TEST_CHECK(u32 == 0);
	}
}

void test_scene() {
	test_scene_binary_serialization();
	// [todo]
}