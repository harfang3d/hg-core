// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "utils.h"

#if _WIN32
#include <windows.h>
#endif

#include <stdio.h>

#if !defined(SOKOL_DUMMY_BACKEND)
#include "app_glfw/app_glfw.h"
#endif

namespace hg {
namespace test {

const std::string LoremIpsum("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vestibulum non\n"
							 "gravida sapien, quis luctus magna. Nullam ut nunc in lacus pretium luctus at\n"
							 "nec nibh. Praesent fermentum vehicula pretium. Cras in magna non diam sagittis\n"
							 "malesuada. Duis a nulla tempor, bibendum libero non, pellentesque magna.\n"
							 "Pellentesque dignissim odio eget sem semper, non luctus sapien pellentesque.\n"
							 "Cras condimentum pellentesque orci, ut vulputate sem convallis et. Nulla in\n"
							 "enim non nibh dignissim lobortis. Phasellus efficitur enim sit amet vulputate\n"
							 "pulvinar.\n");

std::string GetTempDirectoryName() {
	std::string out;
#if _WIN32
	DWORD temp_path_len;
	TCHAR temp_path[MAX_PATH];

	DWORD temp_filename_len;
	TCHAR temp_filename[MAX_PATH];

	temp_path_len = GetTempPath(MAX_PATH, temp_path);
	if ((temp_path_len == 0) || (temp_path_len >= MAX_PATH)) {
		temp_path[0] = '\0';
		temp_path_len = 0;
	}

#ifdef UNICODE
	size_t len;
	wcstombs_s(&len, NULL, 0, &temp_path[0], temp_path_len);
	out.resize(len);
	wcstombs_s(&len, out.data(), len, &temp_path[0], temp_path_len);
#else
	out = temp_path;
#endif

#else
	out = "/tmp";
#endif
	return out;
}

std::string CreateTempFilepath() {
	std::string out;
#if _WIN32
	DWORD temp_path_len;
	TCHAR temp_path[MAX_PATH];

	DWORD temp_filename_len;
	TCHAR temp_filename[MAX_PATH];

	temp_path_len = GetTempPath(MAX_PATH, temp_path);
	if ((temp_path_len == 0) || (temp_path_len >= MAX_PATH)) {
		temp_path[0] = '\0';
		temp_path_len = 0;
	}

	temp_filename_len = GetTempFileName(temp_path, TEXT("hg_core"), 0, temp_filename);
	if (temp_filename_len == 0) {
		return "dummy.txt";
	}

#ifdef UNICODE
	size_t len;
	wcstombs_s(&len, NULL, 0, &temp_filename[0], temp_filename_len);
	out.resize(len);
	wcstombs_s(&len, out.data(), len, &temp_filename[0], temp_filename_len);
#else
	out = temp_filename;
#endif

#else
	out = tmpnam(NULL);
#endif
	return out;
}

uintptr_t RenderInit(int width, int height, const std::string &name) {
#if defined(SOKOL_DUMMY_BACKEND)
	sg_desc desc;
	memset(&desc, 0, sizeof(sg_desc));
	sg_setup(desc);
	return 0xcaffe;
#else
	return reinterpret_cast<uintptr_t>(hg::RenderInit(width, height, name.c_str()));
#endif
}

void RenderShutdown() {
#if defined(SOKOL_DUMMY_BACKEND)
	sg_shutdown();
#else
	hg::RenderShutdown();
#endif
}

} // namespace test
} // namespace hg
