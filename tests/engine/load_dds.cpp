// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#define TEST_NO_MAIN
#include "acutest.h"

#include "foundation/path_tools.h"
#include "foundation/file_rw_interface.h"
#include "foundation/dir.h"

#include "engine/load_dds.h"

#if !defined(SOKOL_DUMMY_BACKEND)
#include "app_glfw/app_glfw.h"
#endif

using namespace hg;

void test_load_dds() {
#if defined(SOKOL_DUMMY_BACKEND)
	sg_desc desc;
	memset(&desc, 0, sizeof(sg_desc));
	sg_setup(desc);
#else
	GLFWwindow *win = RenderInit(640, 480, "test_load_dds");
#endif

	std::vector<DirEntry> entries = ListDir("data", DE_File);
	for (std::vector<DirEntry>::iterator it = entries.begin(); it != entries.end(); ++it) {
		std::string filename = PathJoin("data", it->name);
		Handle h = g_file_read_provider.open(filename, false);
		h.debug = false;
		Texture tex = LoadDDS(g_file_reader, h, filename);
		TEST_CHECK(tex.image.id != SG_INVALID_ID);
		Destroy(tex);
		g_file_read_provider.close(h);
	}

#if defined(SOKOL_DUMMY_BACKEND)
	sg_shutdown();
#else
	RenderShutdown();
#endif
}