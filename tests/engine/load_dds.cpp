// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#define TEST_NO_MAIN
#include "acutest.h"

#include "foundation/path_tools.h"
#include "foundation/file_rw_interface.h"
#include "foundation/dir.h"

#include "engine/load_dds.h"

#include "../utils.h"

using namespace hg;

void test_load_dds() {
	(void)hg::test::RenderInit(640, 480, "test_load_dds");

	std::vector<DirEntry> entries = ListDir("data", DE_File);
	for (std::vector<DirEntry>::iterator it = entries.begin(); it != entries.end(); ++it) {
		std::string filename = PathJoin("data", it->name);
		Handle h = g_file_read_provider.open(filename, false);
#ifdef ENABLE_BINARY_DEBUG_HANDLE
		h.debug = false;
#endif
		Texture tex = LoadDDS(g_file_reader, g_file_read_provider, h, filename, false);
		TEST_CHECK(tex.image.id != SG_INVALID_ID);
		Destroy(tex);
		g_file_read_provider.close(h);
	}

	hg::test::RenderShutdown();
}
