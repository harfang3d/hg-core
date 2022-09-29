// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#define TEST_NO_MAIN
#include "acutest.h"

#include "foundation/data_rw_interface.h"

#include "engine/render_pipeline.h"

#include "../utils.h"

using namespace hg;

static Handle dummy_read_provider_open_impl(const std::string &path, bool silent) {
	static const uint8_t prog[] = {0x48, 0x47, 0x53, 0x4C, 0x00, 0x00, 0x00, 0x4A, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x75, 0x5F,
		0x6D, 0x74, 0x78, 0x40, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0D, 0x00, 0x6D, 0x6F, 0x64, 0x65, 0x6C, 0x56, 0x69, 0x65,
		0x77, 0x50, 0x72, 0x6F, 0x6A, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x04, 0x00, 0x6D, 0x61, 0x69, 0x6E, 0x02, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x0A, 0x00, 0x61, 0x5F, 0x70, 0x6F, 0x73, 0x69, 0x74, 0x69, 0x6F, 0x6E, 0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x61, 0x5F, 0x6E, 0x6F,
		0x72, 0x6D, 0x61, 0x6C, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x69, 0x5F, 0x6E, 0x6F, 0x72, 0x6D, 0x61, 0x6C, 0x00, 0x17, 0x01, 0x00, 0x00,
		0x23, 0x76, 0x65, 0x72, 0x73, 0x69, 0x6F, 0x6E, 0x20, 0x33, 0x33, 0x30, 0x0A, 0x0A, 0x73, 0x74, 0x72, 0x75, 0x63, 0x74, 0x20, 0x4D, 0x61, 0x74, 0x72,
		0x69, 0x63, 0x65, 0x73, 0x0A, 0x7B, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x6D, 0x61, 0x74, 0x34, 0x20, 0x6D, 0x6F, 0x64, 0x65, 0x6C, 0x56, 0x69, 0x65, 0x77,
		0x50, 0x72, 0x6F, 0x6A, 0x3B, 0x0A, 0x7D, 0x3B, 0x0A, 0x0A, 0x75, 0x6E, 0x69, 0x66, 0x6F, 0x72, 0x6D, 0x20, 0x4D, 0x61, 0x74, 0x72, 0x69, 0x63, 0x65,
		0x73, 0x20, 0x75, 0x5F, 0x6D, 0x74, 0x78, 0x3B, 0x0A, 0x0A, 0x6F, 0x75, 0x74, 0x20, 0x76, 0x65, 0x63, 0x33, 0x20, 0x69, 0x5F, 0x6E, 0x6F, 0x72, 0x6D,
		0x61, 0x6C, 0x3B, 0x0A, 0x6C, 0x61, 0x79, 0x6F, 0x75, 0x74, 0x28, 0x6C, 0x6F, 0x63, 0x61, 0x74, 0x69, 0x6F, 0x6E, 0x20, 0x3D, 0x20, 0x31, 0x29, 0x20,
		0x69, 0x6E, 0x20, 0x76, 0x65, 0x63, 0x33, 0x20, 0x61, 0x5F, 0x6E, 0x6F, 0x72, 0x6D, 0x61, 0x6C, 0x3B, 0x0A, 0x6C, 0x61, 0x79, 0x6F, 0x75, 0x74, 0x28,
		0x6C, 0x6F, 0x63, 0x61, 0x74, 0x69, 0x6F, 0x6E, 0x20, 0x3D, 0x20, 0x30, 0x29, 0x20, 0x69, 0x6E, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x61, 0x5F, 0x70,
		0x6F, 0x73, 0x69, 0x74, 0x69, 0x6F, 0x6E, 0x3B, 0x0A, 0x0A, 0x76, 0x6F, 0x69, 0x64, 0x20, 0x6D, 0x61, 0x69, 0x6E, 0x28, 0x29, 0x0A, 0x7B, 0x0A, 0x20,
		0x20, 0x20, 0x20, 0x69, 0x5F, 0x6E, 0x6F, 0x72, 0x6D, 0x61, 0x6C, 0x20, 0x3D, 0x20, 0x61, 0x5F, 0x6E, 0x6F, 0x72, 0x6D, 0x61, 0x6C, 0x3B, 0x0A, 0x20,
		0x20, 0x20, 0x20, 0x67, 0x6C, 0x5F, 0x50, 0x6F, 0x73, 0x69, 0x74, 0x69, 0x6F, 0x6E, 0x20, 0x3D, 0x20, 0x75, 0x5F, 0x6D, 0x74, 0x78, 0x2E, 0x6D, 0x6F,
		0x64, 0x65, 0x6C, 0x56, 0x69, 0x65, 0x77, 0x50, 0x72, 0x6F, 0x6A, 0x20, 0x2A, 0x20, 0x61, 0x5F, 0x70, 0x6F, 0x73, 0x69, 0x74, 0x69, 0x6F, 0x6E, 0x3B,
		0x0A, 0x7D, 0x0A, 0x0A, 0x01, 0x04, 0x00, 0x6D, 0x61, 0x69, 0x6E, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x69, 0x5F, 0x6E, 0x6F, 0x72, 0x6D,
		0x61, 0x6C, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x6F, 0x5F, 0x63, 0x6F, 0x6C, 0x6F, 0x72, 0x00, 0x8B, 0x00, 0x00, 0x00, 0x23, 0x76, 0x65,
		0x72, 0x73, 0x69, 0x6F, 0x6E, 0x20, 0x33, 0x33, 0x30, 0x0A, 0x0A, 0x69, 0x6E, 0x20, 0x76, 0x65, 0x63, 0x33, 0x20, 0x69, 0x5F, 0x6E, 0x6F, 0x72, 0x6D,
		0x61, 0x6C, 0x3B, 0x0A, 0x6C, 0x61, 0x79, 0x6F, 0x75, 0x74, 0x28, 0x6C, 0x6F, 0x63, 0x61, 0x74, 0x69, 0x6F, 0x6E, 0x20, 0x3D, 0x20, 0x30, 0x29, 0x20,
		0x6F, 0x75, 0x74, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x6F, 0x5F, 0x63, 0x6F, 0x6C, 0x6F, 0x72, 0x3B, 0x0A, 0x0A, 0x76, 0x6F, 0x69, 0x64, 0x20, 0x6D,
		0x61, 0x69, 0x6E, 0x28, 0x29, 0x0A, 0x7B, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x6F, 0x5F, 0x63, 0x6F, 0x6C, 0x6F, 0x72, 0x20, 0x3D, 0x20, 0x76, 0x65, 0x63,
		0x34, 0x28, 0x6E, 0x6F, 0x72, 0x6D, 0x61, 0x6C, 0x69, 0x7A, 0x65, 0x28, 0x69, 0x5F, 0x6E, 0x6F, 0x72, 0x6D, 0x61, 0x6C, 0x29, 0x2E, 0x79, 0x79, 0x79,
		0x2C, 0x20, 0x31, 0x2E, 0x30, 0x29, 0x3B, 0x0A, 0x7D, 0x0A, 0x0A};
	static const size_t prog_size = sizeof(prog) / sizeof(prog[0]);

	Data *raw = new Data();
	raw->Write(prog, prog_size);
	raw->Rewind();

	DataReadHandle h(*raw);
	return h;
}

static void dummy_read_provide_close_impl(Handle hnd) {
	Data *data = *reinterpret_cast<Data **>(&hnd);
	data->Free();
	delete data;
}

static bool dummy_read_provider_is_file_impl(const std::string &path) {
	return true;
}

static void test_shader_load() {
	const ReadProvider dummy_read_provider = {dummy_read_provider_open_impl, dummy_read_provide_close_impl, dummy_read_provider_is_file_impl};
	Shader sh = LoadShader(g_data_reader, dummy_read_provider, "dummy program", false);
	TEST_CHECK(sh.shader.id != SG_INVALID_ID);
	TEST_CHECK(sh.layout.attrib[0] == VA_Position);
	TEST_CHECK(sh.layout.attrib[1] == VA_Normal);
	for (int i = 2; i < SG_MAX_VERTEX_ATTRIBUTES; i++) {
		TEST_CHECK(sh.layout.attrib[i] == VA_Count);
	}
	TEST_CHECK(sh.uniforms[0].layout == SG_UNIFORMLAYOUT_STD140);
	TEST_CHECK(sh.uniforms[0].uniform[0].name == "u_mtx.modelViewProj");
	TEST_CHECK(sh.uniforms[0].uniform[0].type == SG_UNIFORMTYPE_MAT4);
	TEST_CHECK(sh.uniforms[0].uniform[0].count == 0);
	for (int j = 1; j < SG_MAX_UB_MEMBERS; j++) {
		TEST_CHECK(sh.uniforms[0].uniform[j].name.empty());
		TEST_CHECK(sh.uniforms[0].uniform[j].type == SG_UNIFORMTYPE_INVALID);
		TEST_CHECK(sh.uniforms[0].uniform[j].count == 0);
	}
		 
	for (int i = 1; i < SG_MAX_SHADERSTAGE_UBS; i++) {
		TEST_CHECK(sh.uniforms[i].layout == _SG_UNIFORMLAYOUT_DEFAULT);
		for (int j = 0; j < SG_MAX_UB_MEMBERS; j++) {
			TEST_CHECK(sh.uniforms[i].uniform[j].name.empty());
			TEST_CHECK(sh.uniforms[i].uniform[j].type == SG_UNIFORMTYPE_INVALID);
			TEST_CHECK(sh.uniforms[i].uniform[j].count == 0);
		}
	}
	sg_destroy_shader(sh.shader);
}

void test_render_pipeline() {
	(void)hg::test::RenderInit(640, 480, "test_load_dds");

	test_shader_load();

	hg::test::RenderShutdown();
}