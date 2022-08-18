// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#if _WIN32
#define WIN32_LEAN_AND_MEAN
#include <ShlObj.h>
#include <Windows.h>
#else // POSIX
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include "foundation/assert.h"
#include "foundation/cext.h"
#include "foundation/path_tools.h"
#include "foundation/string.h"

#include <fmt/format.h>
#include <string.h>

namespace hg {

std::string GetFilePath(const std::string &p) {
	std::string res;

	ptrdiff_t i = p.length() - 1;
	for (; i >= 0; --i) {
		if (p[i] == '/' || p[i] == '\\') { // found last separator in file name
			break;
		}
	}

	if (i < 0) {
		res = "./"; // explicit current path
	} else {
		if (i == 0) {
#if _WIN32
			HG_ASSERT(i > 0);
			res = "./"; // a path starting with a slash on windows is a non-sense...
#else
			res = "/"; // root file
#endif
		} else {
			if (p[i] == '/') {
				res = slice(p, 0, i) + "/";
			} else {
				res = slice(p, 0, i) + "\\";
			}
		}
	}

	return res;
}

std::string GetFileName(const std::string &path) {
	return CutFilePath(CutFileExtension(path));
}

//
bool IsPathAbsolute(const std::string &path) {
#if WIN32
	return path.length() > 2 && path[1] == ':';
#else
	return path.length() > 1 && path[0] == '/';
#endif
}

bool PathStartsWith(const std::string &path, const std::string &with) {
	return starts_with(CleanPath(path), CleanPath(with));
}

std::string PathStripPrefix(const std::string &path, const std::string &prefix) {
	return strip_prefix(strip_prefix(CleanPath(path), CleanPath(prefix)), "/");
}

std::string PathStripSuffix(const std::string &path, const std::string &suffix) {
	return strip_suffix(strip_suffix(CleanPath(path), CleanPath(suffix)), "/");
}

//
std::string PathToDisplay(const std::string &path) {
	std::string out(path);
	replace_all(out, "_", " ");
	return out;
}

std::string NormalizePath(const std::string &path) {
	std::string out(path);
	replace_all(out, " ", "_");
	return out;
}

//
std::string PathJoin(const std::vector<std::string> &elements) {
	std::vector<std::string> stripped_elements;
	stripped_elements.reserve(elements.size());
#if !defined(_WIN32)
	if (!elements.empty()) {
		if (starts_with(elements[0], "/")) {
			stripped_elements.push_back("");
		}
	}
#endif
	for (std::vector<std::string>::const_iterator element = elements.begin(); element != elements.end(); ++element) {
		if (!element->empty()) {
			stripped_elements.push_back(rstrip(lstrip(*element, "/"), "/"));
		}
	}
	return CleanPath(join(stripped_elements.begin(), stripped_elements.end(), "/"));
}

std::string PathJoin(const std::string &a, const std::string &b) {
	std::vector<std::string> elms;
	elms.push_back(a);
	elms.push_back(b);
	return PathJoin(elms);
}

std::string PathJoin(const std::string &a, const std::string &b, const std::string &c) {
	std::vector<std::string> elms;
	elms.push_back(a);
	elms.push_back(b);
	elms.push_back(c);
	return PathJoin(elms);
}

//
std::string CutFilePath(const std::string &path) {
	std::string res = path;

	if (!path.empty()) {
		for (size_t n = path.length() - 1; n > 0; --n) {
			if (path[n] == '\\' || path[n] == '/') {
				res = slice(path, n + 1);
				break;
			}
		}
	}

	return res;
}

std::string CutFileExtension(const std::string &path) {
	std::string res = path;

	if (!path.empty()) {
		for (size_t n = path.length() - 1; n > 0; --n) {
			bool done;

			if (path[n] == '.') {
				res = slice(path, 0, n);
				done = true;
			} else if (path[n] == '\\' || path[n] == '/' || path[n] == ':') {
				done = true;
			} else {
				done = false;
			}

			if (done) {
				break; // disclaimer: these inefficient and redundant code structures are mandated by MISRA compliance (2-liners become garbled mess of 10+
					   // lines of convoluted logic)
			}
		}
	}

	return res;
}

std::string CutFileName(const std::string &path) {
	std::string res;

	if (!path.empty()) {
		for (size_t n = path.length() - 1; n > 0; --n) {
			if (path[n] == '\\' || path[n] == '/' || path[n] == ':') {
				res = slice(path, 0, n + 1);
				break;
			}
		}
	}

	return res;
}

//
std::string GetFileExtension(const std::string &path) {
	std::string res;

	if (!path.empty()) {
		for (size_t n = path.length() - 1; n > 0; --n) {
			if (path[n] == '.') {
				res = slice(path, n + 1);
				break;
			}
		}
	}

	return res;
}

bool HasFileExtension(const std::string &path) {
	return !GetFileExtension(path).empty();
}

//
std::string SwapFileExtension(const std::string &path, const std::string &ext) {
	std::string res;

	if (ext.empty()) {
		res = path;
	} else {
		res = CutFileExtension(path) + "." + ext;
	}

	return res;
}

//
std::string FactorizePath(const std::string &path) {
	std::string res;

	std::vector<std::string> dirs = split(path, "/");

	if (dirs.size() < 2) {
		res = dirs.empty() ? path : dirs[0];
	} else {
		bool factorized = false;

		while (dirs.size() > 1 && !factorized) {
			factorized = true;

			std::vector<std::string>::iterator i = dirs.begin();
			for (; i != dirs.end() - 1; ++i) {
				if ((*i != "..") && (*(i + 1) == "..")) {
					factorized = false;
					break;
				}
			}

			if (!factorized) {
				dirs.erase(i + 1);
				dirs.erase(i);
			}
		}

		res = join(dirs.begin(), dirs.end(), "/");
	}

	return res;
}

//
std::string CleanPath(const std::string &path) {
	std::string res = path;

	// drive letter to lower case on Windows platform
#if _WIN32
	if (path.length() > 2 && path[1] == ':') {
		res = tolower(path, 0, 1);
	}

	const bool is_network_path = starts_with(res, "\\\\");
#endif

	// convert directory separator from backslash to forward slash
	replace_all(res, "\\", "/");

	// remove redundant forward slashes
	while (replace_all(res, "//", "/")) {}
	while (replace_all(res, "/./", "/")) {}

#if _WIN32
	if (is_network_path) {
		const std::string tmp = slice(res, 1);
		res = "\\\\";
		res += tmp;
	}
#endif

	// remove pointless ./ when it is starting the file path
	while (starts_with(res, "./")) {
		res = slice(res, 2);
	}

	// remove pointless /. when it is ending the file path
	while (ends_with(res, "/.")) {
		res = slice(res, 0, -2);
	}

	res = FactorizePath(res);

	return res;
}

std::string CleanFileName(const std::string &filename) {
	std::string res = filename;

	const char filename_invalid_chars[] = "<>:\"/\\|?*";

	for (size_t i = 0; i < sizeof(filename_invalid_chars); i++) {
		const std::string str_to_replace(1, filename_invalid_chars[i]);
		replace_all(res, str_to_replace, "_");
	}

	return res;
}

//
#if _WIN32

std::string GetCurrentWorkingDirectory() {
	std::array<WCHAR, 1024> cwd;
	GetCurrentDirectoryW(1024 - 1, cwd.data()); // poorly worded documentation makes it unclear if nBufferLength should account for the terminator or not...
	return wchar_to_utf8(cwd.data());
}

bool SetCurrentWorkingDirectory(const std::string &path) {
	return SetCurrentDirectoryW(utf8_to_wchar(path).c_str()) == TRUE;
}

std::string GetUserFolder() {
	std::string res;

	PWSTR path;
	HRESULT hres = SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_DEFAULT, NULL, &path);

	if (SUCCEEDED(hres)) {
		res = wchar_to_utf8(path);
		CoTaskMemFree(path);
	}

	return res;
}

#else // POSIX

std::string GetCurrentWorkingDirectory() {
	std::string res;

	std::array<char, 1024> cwd;
	if (getcwd(cwd.data(), 1024)) {
		res = cwd.data();
	}

	return res;
}

bool SetCurrentWorkingDirectory(const std::string &path) {
	return chdir(path.c_str()) == 0;
}

std::string GetUserFolder() {
	return getpwuid(getuid())->pw_dir;
}

#endif // POSIX

} // namespace hg
