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
#include <unistd.h>
#define _unlink unlink
#endif

#undef CopyFile

namespace hg {

std::vector<DirEntry> ListDir(const std::string &path, int mask) {
	std::vector<DirEntry> entries;

#if _WIN32
	WIN32_FIND_DATAW data;

	std::wstring wfind_path = utf8_to_wchar(PathJoin({path, "*.*"}));
	HANDLE hFind = FindFirstFileW(wfind_path.c_str(), &data);
	if (hFind == INVALID_HANDLE_VALUE)
		return entries;

	do {
		std::string name = wchar_to_utf8(data.cFileName);
		if (name == "." || name == "..")
			continue;

		const DirEntryType type = (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? DE_Dir : DE_File;
		const int64_t last_modified = time_to_ns(((uint64_t(data.ftLastWriteTime.dwHighDateTime) << 32) + data.ftLastWriteTime.dwLowDateTime) * 100);

		LARGE_INTEGER size;
		size.HighPart = data.nFileSizeHigh;
		size.LowPart = data.nFileSizeLow;

		if (mask & type)
			entries.push_back({type, std::move(name), last_modified, numeric_cast<size_t>(size.QuadPart)});
	} while (FindNextFileW(hFind, &data));

	FindClose(hFind);
#else /* POSIX */
	DIR *dir = opendir(path.c_str());
	if (!dir)
		return entries;

	while (struct dirent *ent = readdir(dir)) {
		if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
			continue;

		DirEntry entry;

		int type = 0;

		if (ent->d_type == DT_DIR)
			type = DE_Dir;
		else if (ent->d_type == DT_REG)
			type = DE_File;
		else if (ent->d_type == DT_LNK)
			type = DE_Link;

		// TODO: stat() missing infos
		if (mask & type) {
			DirEntry e = {type, ent->d_name, 0, 0};
			entries.push_back(e);
		}
	}

	closedir(dir);
#endif
	return entries;
}

std::vector<DirEntry> ListDirRecursive(const std::string &path, int mask) {
	std::vector<DirEntry> entries = ListDir(path, mask);
#if _WIN32
	WIN32_FIND_DATAW data;
	std::wstring wfind_path = utf8_to_wchar(PathJoin({path, "*"}));
	HANDLE hFind = FindFirstFileW(wfind_path.c_str(), &data);
	if (hFind == INVALID_HANDLE_VALUE)
		return entries;
	do {
		std::string name = wchar_to_utf8(data.cFileName);
		if (name == "." || name == "..")
			continue;
		if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			const std::vector<DirEntry> sub_entries = ListDirRecursive(PathJoin({path, name}).c_str(), mask);
			std::vector<std::string> tmp(2);
			for (std::vector<DirEntry>::const_iterator i = sub_entries.begin(); i != sub_entries.end(); ++i) {
				tmp[0] = name;
				tmp[1] = i->name;
				DirEntry e = {i->type, PathJoin(tmp), 0, 0};

				entries.push_back(e);
			}
		}
	} while (FindNextFileW(hFind, &data));

	FindClose(hFind);
#else /* POSIX */
	DIR *dir = opendir(path.c_str());
	if (!dir)
		return entries;

	std::vector<std::string> tmp(2);
	while (struct dirent *ent = readdir(dir)) {
		if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
			continue;
		if (ent->d_type == DT_DIR) {
			tmp[0] = path;
			tmp[1] = ent->d_name;
			const std::vector<DirEntry> sub_entries = ListDirRecursive(PathJoin(tmp).c_str(), mask);
			for (std::vector<DirEntry>::const_iterator i = sub_entries.begin(); i != sub_entries.end(); ++i) {
				tmp[0] = ent->d_name;
				tmp[1] = i->name;
				DirEntry e = {i->type, PathJoin(tmp), 0, 0};

				entries.push_back(e);
			}
		}
	}

	closedir(dir);
#endif
	return entries;
}

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
			const FileInfo finfo = GetFileInfo(fpath.c_str());
			size += finfo.size;
		}
	}
	return size;
}

//
bool MkDir(const std::string &path, int permissions, bool verbose) {
#if _WIN32
	const auto res = CreateDirectoryW(utf8_to_wchar(path).c_str(), nullptr) != 0;
	if (!verbose && !res)
		warn(fmt::format("MkDir({}) failed with error: {}", path, OSGetLastError()));
	return res;
#else
	return mkdir(path.c_str(), permissions) == 0;
#endif
}

bool RmDir(const std::string &path, bool verbose) {
#if _WIN32
	const auto res = RemoveDirectoryW(utf8_to_wchar(path).c_str()) != 0;
	if (verbose && !res)
		warn(fmt::format("RmDir({}) failed with error: {}", path, OSGetLastError()));
	return res;
#else
	return false;
#endif
}

bool MkTree(const std::string &path, int permissions, bool verbose) {
	const std::vector<std::string> dirs = split(CleanPath(path), "/");

	std::string p;
	for (std::vector<std::string>::const_iterator dir = dirs.begin(); dir != dirs.end(); ++dir) {
		p += *dir + "/";

		if (ends_with(dir->c_str(), ":"))
			continue; // skip c:

		if (Exists(p.c_str()))
			continue;

		if (!MkDir(p.c_str(), permissions, verbose))
			return false;
	}

	return true;
}

bool RmTree(const std::string &path, bool verbose) {
#if _WIN32
	std::string _path(path);
	if (!ends_with(_path, "/"))
		_path += "/";

	const std::wstring wpath = utf8_to_wchar(_path);
	const std::wstring wfilter = wpath + L"*.*";

	WIN32_FIND_DATAW FindFileData;
	ZeroMemory(&FindFileData, sizeof(FindFileData));
	HANDLE hFind = FindFirstFileW(wfilter.c_str(), &FindFileData);

	if (hFind != nullptr && hFind != INVALID_HANDLE_VALUE) {
		while (FindNextFileW(hFind, &FindFileData) != 0) {
			if (wcscmp(FindFileData.cFileName, L".") != 0 && wcscmp(FindFileData.cFileName, L"..") != 0) {
				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					if (!RmTree(wchar_to_utf8(wpath + std::wstring(FindFileData.cFileName)).c_str(), verbose))
						return false;
				} else {
					if (!DeleteFileW((wpath + std::wstring(FindFileData.cFileName)).c_str()))
						return false;
				}
			}
		}
		FindClose(hFind);
	}

	return RemoveDirectoryW(wpath.c_str());
#else
	return false;
#endif
}

bool IsDir(const std::string &path) {
#if _WIN32
	struct _stat info;
	if (_wstat(utf8_to_wchar(path).c_str(), &info) != 0)
		return false;
#else
	struct stat info;
	if (stat(path.c_str(), &info) != 0)
		return false;
#endif

	if (info.st_mode & S_IFDIR)
		return true;
	return false;
}

bool CopyDir(const std::string &src, const std::string &dst) {
	if (!IsDir(src))
		return false;

	std::vector<std::string> tmp(2);
	const std::vector<DirEntry> entries = ListDir(src);
	for (std::vector<DirEntry>::const_iterator e = entries.begin(); e != entries.end(); ++e) {
		if (e->type & DE_File) {
			tmp[1] = e->name;
			tmp[0] = src;
			const std::string file_src = PathJoin(tmp);
			tmp[0] = dst;
			const std::string file_dst = PathJoin(tmp);
			if (!CopyFile(file_src, file_dst))
				return false;
		}
	}
	return true;
}

bool CopyDirRecursive(const std::string &src, const std::string &dst) {
	if (!IsDir(src) || !IsDir(dst))
		return false;

	std::vector<std::string> tmp(2);
	const std::vector<DirEntry> entries = ListDir(src);
	for (std::vector<DirEntry>::const_iterator e = entries.begin(); e != entries.end(); ++e) {
		if (e->type & DE_Dir) {
			tmp[1] = e->name;
			tmp[0] = src;
			const std::string src_path = PathJoin(tmp);
			tmp[0] = dst;
			const std::string dst_path = PathJoin(tmp);
			if (!MkDir(dst_path.c_str()))
				return false;
			if (!CopyDirRecursive(src_path.c_str(), dst_path.c_str()))
				return false;
		} else if (e->type & DE_File) {
			tmp[1] = e->name;
			tmp[0] = src;
			const std::string file_src = PathJoin(tmp);
			tmp[0] = dst;
			const std::string file_dst = PathJoin(tmp);
			if (!CopyFile(file_src, file_dst))
				return false;
		}
	}
	return true;
}

bool Exists(const std::string &path) {
#if _WIN32
	struct _stat info;
	return _wstat(utf8_to_wchar(path).data(), &info) == 0;
#else
	struct stat info;
	return stat(path.c_str(), &info) == 0;
#endif
}

} // namespace hg
