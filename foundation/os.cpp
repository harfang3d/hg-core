// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "foundation/os.h"
#include "foundation/string.h"

#if _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <errno.h>
#include <string.h>
#endif

namespace hg {

#if _WIN32
std::string OSGetLastError() {
	LPWSTR err_win32 = NULL;

	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&err_win32, 0, NULL);

	std::string err;

	if (err_win32) {
		err = wchar_to_utf8(err_win32);
		LocalFree(err_win32);
	}

	return err;
}
#else
std::string OSGetLastError() { return std::string(strerror(errno)); }
#endif

} // namespace hg
