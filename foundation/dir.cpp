// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "foundation/dir.h"
#include "foundation/cext.h"
#include "foundation/file.h"
#include "foundation/log.h"
#include "foundation/os.h"
#include "foundation/path_tools.h"
#include "foundation/rand.h"
#include "foundation/string.h"

#include <fmt/format.h>

#include <sys/stat.h>
#if _WIN32
#include <direct.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shlwapi.h>
#else /* POSIX */
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#define _unlink unlink
#endif

#undef CopyFile

namespace hg {

#if _WIN32

std::vector<DirEntry> ListDir(const std::string &path, int mask) {
	std::vector<DirEntry> entries;

	WIN32_FIND_DATAW data;
	HANDLE hFind = FindFirstFileW(utf8_to_wchar(PathJoin(path, "*.*")).c_str(), &data);

	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			std::string name = wchar_to_utf8(data.cFileName);

			if (name == "." || name == "..") {
				continue;
			}

			const DirEntryType type = (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? DE_Dir : DE_File;

			if (mask & type) {
				const int64_t last_modified =
					time_to_ns(((static_cast<uint64_t>(data.ftLastWriteTime.dwHighDateTime) << 32) + data.ftLastWriteTime.dwLowDateTime) * 100);

				LARGE_INTEGER size;
				size.HighPart = data.nFileSizeHigh;
				size.LowPart = data.nFileSizeLow;

				DirEntry entry;
				entry.type = type;
				entry.name = name;
				entry.last_modified = last_modified;
				entry.size = numeric_cast<size_t>(size.QuadPart);
				entries.push_back(entry);
			}
		} while (FindNextFileW(hFind, &data));

		FindClose(hFind);
	}

	return entries;
}

std::vector<DirEntry> ListDirRecursive(const std::string &path, int mask) {
	std::vector<DirEntry> entries = ListDir(path, mask);

	WIN32_FIND_DATAW data;

	HANDLE hFind = FindFirstFileW(utf8_to_wchar(PathJoin(path, "*")).c_str(), &data);

	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			const std::string name = wchar_to_utf8(data.cFileName);

			if (name == "." || name == "..") {
				continue;
			}

			if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				const std::vector<DirEntry> sub_entries = ListDirRecursive(PathJoin(path, name), mask);

				for (std::vector<DirEntry>::const_iterator i = sub_entries.begin(); i != sub_entries.end(); ++i) {
					std::vector<std::string> elms(2);
					elms[0] = name;
					elms[1] = i->name;

					DirEntry e = {i->type, PathJoin(elms), 0, 0};

					entries.push_back(e);
				}
			}
		} while (FindNextFileW(hFind, &data));

		FindClose(hFind);
	}

	return entries;
}

#else /* POSIX */

std::vector<DirEntry> ListDir(const std::string &path, int mask) {
	std::vector<DirEntry> entries;

	DIR *dir = opendir(path.c_str());

	if (dir != nullptr) {
		while (struct dirent *ent = readdir(dir)) {
			if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
				continue;

			DirEntry entry;

			int type;

			if (ent->d_type == DT_DIR) {
				type = DE_Dir;
			} else if (ent->d_type == DT_REG) {
				type = DE_File;
			} else if (ent->d_type == DT_LNK) {
				type = DE_Link;
			} else {
				type = 0;
			}

			// TODO: stat() missing infos
			if (mask & type) {
				DirEntry e = {type, ent->d_name, 0, 0};
				entries.push_back(e);
			}
		}

		closedir(dir);
	}

	return entries;
}

std::vector<DirEntry> ListDirRecursive(const std::string &path, int mask) {
	std::vector<DirEntry> entries = ListDir(path, mask);

	DIR *dir = opendir(path.c_str());
	if (dir != nullptr) {
		while (struct dirent *ent = readdir(dir)) {
			if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..")) {
				continue;
			}

			if (ent->d_type == DT_DIR) {
				elms[0] = path;
				elms[1] = ent->d_name;

				const std::vector<DirEntry> sub_entries = ListDirRecursive(PathJoin(elms), mask);

				for (std::vector<DirEntry>::const_iterator i = sub_entries.begin(); i != sub_entries.end(); ++i) {
					std::vector<std::string> elms(2);
					elms[0] = ent->d_name;
					elms[1] = i->name;
					DirEntry e = {i->type, PathJoin(elms), 0, 0};

					entries.push_back(e);
				}
			}
		}

		closedir(dir);
	}

	return entries;
}

#endif

//
size_t GetDirSize(const std::string &path) {
	const std::vector<DirEntry> entries = ListDirRecursive(path);

	size_t size = 0;
	std::vector<std::string> tmp(2);
	tmp[0] = path;
	for (std::vector<DirEntry>::const_iterator e = entries.begin(); e != entries.end(); ++e) {
		if (e->type == DE_File) {
			tmp[1] = e->name;
			const std::string fpath = PathJoin(tmp);
			const FileInfo finfo = GetFileInfo(fpath);
			size += finfo.size;
		}
	}
	return size;
}

//
#if _WIN32

bool MkDir(const std::string &path, int permissions, bool verbose) {
	permissions = 0; // unused
	const bool res = CreateDirectoryW(utf8_to_wchar(path).c_str(), nullptr) != 0;

	if (!verbose && !res) {
		warn(fmt::format("MkDir({}) failed with error: {}", path, OSGetLastError()));
	}

	return res;
}

bool RmDir(const std::string &path, bool verbose) {
	const bool res = RemoveDirectoryW(utf8_to_wchar(path).c_str()) != 0;

	if (verbose && !res) {
		warn(fmt::format("RmDir({}) failed with error: {}", path, OSGetLastError()));
	}

	return res;
}

#else

bool MkDir(const std::string &path, int permissions, bool verbose) {
	return mkdir(path.c_str(), permissions) == 0;
}

bool RmDir(const std::string &path, bool verbose) {
	int ret = remove(path.c_str());

	if (verbose && ret) {
		warn(fmt::format("RmDir({}) failed with error: {}", path, errno));
	}

	return ret == 0;
}

#endif

//
bool MkTree(const std::string &path, int permissions, bool verbose) {
	bool res = true;

	const std::vector<std::string> dirs = split(CleanPath(path), "/");

	std::string p;
	for (std::vector<std::string>::const_iterator dir = dirs.begin(); dir != dirs.end(); ++dir) {
		p += *dir + "/";

		if (ends_with(*dir, ":")) {
			continue; // skip c:
		}

		if (Exists(p)) {
			continue;
		}

		if (!MkDir(p, permissions, verbose)) {
			res = false;
			break;
		}
	}

	return res;
}

//
#if _WIN32

bool RmTree(const std::string &path, bool verbose) {
	bool res = true;

	std::string _path(path);
	if (!ends_with(_path, "/")) {
		_path += "/";
	}

	const std::wstring wpath = utf8_to_wchar(_path);
	const std::wstring wfilter = wpath + L"*.*";

	WIN32_FIND_DATAW FindFileData;
	ZeroMemory(&FindFileData, sizeof(FindFileData));
	HANDLE hFind = FindFirstFileW(wfilter.c_str(), &FindFileData);

	if ((hFind != nullptr) && (hFind != INVALID_HANDLE_VALUE)) {
		while (FindNextFileW(hFind, &FindFileData) != 0) {
			const std::wstring filename = FindFileData.cFileName;

			if ((filename == L".") || (filename == L"..")) {
				continue;
			}

			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				if (!RmTree(wchar_to_utf8(wpath + filename), verbose)) {
					res = false;
				}
			} else {
				if (!DeleteFileW((wpath + filename).c_str())) {
					res = false;
				}
			}

			if (!res) {
				break;
			}
		}

		FindClose(hFind);
	}

	if (res) {
		if (!RmDir(path, verbose)) {
			res = false;
		}
	}

	return res;
}

#else

bool RmTree(const std::string &path, bool verbose) {
	bool res = true;

	DIR *dir = opendir(path.c_str());

	if (dir != nullptr) {
		while (const struct dirent *ent = readdir(dir)) {
			if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..")) {
				continue;
			}

			const std::string ent_path = PathJoin(path, ent->d_name);

			if (ent->d_type == DT_DIR) {
				if (!RmTree(ent_path, verbose)) {
					res = false;
					break;
				}
			} else {
				const int ret = remove(ent_path.c_str());

				if (verbose && ret) {
					warn(fmt::format("RmTree({}) failed to delete {}: {}", path, ent_path, strerror(errno)));
					res = false;
					break;
				}
			}
		}

		closedir(dir);
	}

	if (res) {
		if (!RmDir(path, verbose)) {
			res = false;
		}
	}

	return res;
}

#endif

//
#if _WIN32

bool IsDir(const std::string &path) {
	bool res;

	struct _stat info;

	if (_wstat(utf8_to_wchar(path).c_str(), &info) == 0) {
		if (info.st_mode & S_IFDIR) {
			res = true;
		} else {
			res = false;
		}
	} else {
		res = false;
	}

	return res;
}

#else

bool IsDir(const std::string &path) {
	bool res;

	struct stat info;

	if (stat(path.c_str(), &info) == 0) {
		if (info.st_mode & S_IFDIR) {
			res = true;
		} else {
			res = false;
		}
	} else {
		res = false;
	}

	return res;
}

#endif

//
bool CopyDir(const std::string &src, const std::string &dst) {
	bool res;

	if (IsDir(src)) {
		res = true;

		const std::vector<DirEntry> entries = ListDir(src);
		for (std::vector<DirEntry>::const_iterator e = entries.begin(); e != entries.end(); ++e) {
			if (e->type & DE_File) {
				std::vector<std::string> elms(2);
				elms[1] = e->name;

				elms[0] = src;
				const std::string file_src = PathJoin(elms);
				elms[0] = dst;
				const std::string file_dst = PathJoin(elms);

				if (!CopyFile(file_src, file_dst)) {
					res = false;
					break;
				}
			}
		}
	} else {
		res = false;
	}

	return res;
}

bool CopyDirRecursive(const std::string &src, const std::string &dst) {
	bool res;

	if (IsDir(src) && IsDir(dst)) {
		res = true;

		const std::vector<DirEntry> entries = ListDir(src);
		for (std::vector<DirEntry>::const_iterator e = entries.begin(); e != entries.end(); ++e) {
			if (e->type & DE_Dir) {
				std::vector<std::string> elms(2);
				elms[1] = e->name;

				elms[0] = src;
				const std::string src_path = PathJoin(elms);
				elms[0] = dst;
				const std::string dst_path = PathJoin(elms);

				if (MkDir(dst_path)) {
					if (!CopyDirRecursive(src_path, dst_path)) {
						res = false;
					}
				} else {
					res = false;
				}
			} else if (e->type & DE_File) {
				std::vector<std::string> elms(2);
				elms[1] = e->name;

				elms[0] = src;
				const std::string file_src = PathJoin(elms);
				elms[0] = dst;
				const std::string file_dst = PathJoin(elms);

				if (!CopyFile(file_src, file_dst)) {
					res = false;
				}
			} else {
				//
			}

			if (!res) {
				break;
			}
		}
	} else {
		res = false;
	}

	return res;
}

//
#if _WIN32

bool Exists(const std::string &path) {
	struct _stat info;
	return _wstat(utf8_to_wchar(path).data(), &info) == 0;
}

#else

bool Exists(const std::string &path) {
	struct stat info;
	return stat(path.c_str(), &info) == 0;
}

#endif

} // namespace hg
