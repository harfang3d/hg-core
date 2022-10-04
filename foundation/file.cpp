// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#if _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <sys/stat.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include "foundation/cext.h"
#include "foundation/dir.h"
#include "foundation/file.h"
#include "foundation/log.h"
#include "foundation/rand.h"
#include "foundation/string.h"

//#include <cstdio>
#include <fmt/format.h>

#undef CopyFile

namespace hg {

static generational_vector_list<FILE *> files;

static FILE *Open_(const std::string &path, const std::string &mode, bool silent = false) {
	FILE *file = nullptr;
#if _WIN32
	const std::wstring wpath = utf8_to_wchar(path);
	const std::wstring wmode = utf8_to_wchar(mode);

	const errno_t err = _wfopen_s(&file, wpath.data(), wmode.data());

	if (!silent && err != 0) {
		warn(fmt::format("Failed to open file '{}' mode '{}', error code {} ({})", path, mode, err, strerror(err)));
	}
#else
	file = fopen(path.c_str(), mode.c_str());
#endif
	return file;
}

static inline File from_posix_FILE(FILE *f) {
	File out = {f ? files.add_ref(f) : invalid_gen_ref};
	return out;
}

//
File Open(const std::string &path, bool silent) {
	return from_posix_FILE(Open_(path, "rb", silent));
}

File OpenText(const std::string &path, bool silent) {
	return from_posix_FILE(Open_(path, "r", silent));
}

File OpenWrite(const std::string &path) {
	return from_posix_FILE(Open_(path, "wb"));
}

File OpenWriteText(const std::string &path) {
	return from_posix_FILE(Open_(path, "w"));
}

File OpenAppendText(const std::string &path) {
	return from_posix_FILE(Open_(path, "a"));
}

//
bool Close(File file) {
	bool res;

	if (files.is_valid(file.ref)) {
		fclose(files[file.ref.idx]);
		files.remove_ref(file.ref);
		res = true;
	} else {
		res = false;
	}

	return res;
}

bool IsValid(File file) {
	return files.is_valid(file.ref);
}

bool IsEOF(File file) {
	return files.is_valid(file.ref) ? feof(files[file.ref.idx]) != 0 : true;
}

size_t GetSize(File file) {
	size_t res;

	if (files.is_valid(file.ref)) {
		FILE *s = files[file.ref.idx];
		long t = ftell(s);
		fseek(s, 0, SEEK_END);
		res = ftell(s);
		fseek(s, t, SEEK_SET);
	} else {
		res = 0;
	}

	return res;
}

size_t Read(File file, void *data, size_t size) {
	return files.is_valid(file.ref) ? fread(data, 1, size, files[file.ref.idx]) : 0;
}

size_t Write(File file, const void *data, size_t size) {
	return files.is_valid(file.ref) ? fwrite(data, 1, size, files[file.ref.idx]) : 0;
}

bool Seek(File file, ptrdiff_t offset, SeekMode mode) {
	int _mode;

	if (mode == SM_Start) {
		_mode = SEEK_SET;
	} else if (mode == SM_Current) {
		_mode = SEEK_CUR;
	} else {
		_mode = SEEK_END;
	}

	bool res;

	if (files.is_valid(file.ref)) {
		res = fseek(files[file.ref.idx], static_cast<long>(offset), _mode) == 0;
	} else {
		res = false;
	}

	return res;
}

size_t Tell(File file) {
	return files.is_valid(file.ref) ? ftell(files[file.ref.idx]) : 0;
}

void Rewind(File file) {
	if (files.is_valid(file.ref)) {
		fseek(files[file.ref.idx], 0, SEEK_SET);
	}
}

//
FileInfo GetFileInfo(const std::string &path) {
	static const FileInfo not_found = {false, 0, 0, 0};

	FileInfo res;

#if _WIN32
	struct _stat info;
	if (_wstat(utf8_to_wchar(path).c_str(), &info) == 0) {
		res.is_file = info.st_mode & S_IFREG ? true : false;
		res.size = static_cast<size_t>(info.st_size);
		res.created = static_cast<time_ns>(info.st_ctime);
		res.modified = static_cast<time_ns>(info.st_mtime);
	} else {
		res = not_found;
	}
#else
	struct stat info;
	if (stat(path.c_str(), &info) == 0) {
		res.is_file = info.st_mode & S_IFREG ? true : false;
		res.size = static_cast<size_t>(info.st_size);
		res.created = static_cast<time_ns>(info.st_ctime);
		res.modified = static_cast<time_ns>(info.st_mtime);
	} else {
		res = not_found;
	}
#endif

	return res;
}

//
bool IsFile(const std::string &path) {
	bool res;

#if _WIN32
	struct _stat info;
	if (_wstat(utf8_to_wchar(path).c_str(), &info) == 0) {
		res = (info.st_mode & S_IFREG) != 0;
	} else {
		res = false;
	}
#else
	struct stat info;
	if (stat(path.c_str(), &info) == 0) {
		res = (info.st_mode & S_IFREG) != 0;
	} else {
		res = false;
	}
#endif

	return res;
}
//
bool IsDirectory(const std::string &path) {
	bool res;

#if _WIN32
	struct _stat info;
	if (_wstat(utf8_to_wchar(path).c_str(), &info) == 0) {
		res = (info.st_mode & S_IFDIR) != 0;
	} else {
		res = false;
	}
#else
	struct stat info;
	if (stat(path.c_str(), &info) == 0) {
		res = (info.st_mode & S_IFDIR) != 0;
	} else {
		res = false;
	}
#endif

	return res;
}

bool Unlink(const std::string &path) {
#if _WIN32
	const std::wstring wpath = utf8_to_wchar(path);
	return DeleteFileW(wpath.c_str()) == TRUE;
#else
	return unlink(path.c_str()) == 0;
#endif
}

//
bool CopyFile(const std::string &src, const std::string &dst) {
#if _WIN32
	const std::wstring wsrc = utf8_to_wchar(src);
	const std::wstring wdst = utf8_to_wchar(dst);
	return ::CopyFileW(wsrc.c_str(), wdst.c_str(), FALSE) ? true : false;
#else
	bool res;

	ScopedFile in(Open(src));
	if (in) {
		ScopedFile out(OpenWrite(dst));

		std::vector<char> data(65536); // 64KB copy

		res = true;
		while (!IsEOF(in)) {
			const size_t size = Read(in, data.data(), data.size());

			if (size == 0) {
				break;
			}

			if (Write(out, data.data(), size) != size) {
				res = false;
				break;
			}
		}
	} else {
		res = false;
	}

	return res;
#endif
}

//
bool FileToData(const std::string &path, Data &data, bool silent) {
	bool res;
	File file = Open(path, silent);

	if (IsValid(file)) {
		data.Resize(GetSize(file));
		Read(file, data.GetData(), data.GetSize());
		Close(file);
		res = true;
	} else {
		res = false;
	}

	return res;
}

//
std::string FileToString(const std::string &path, bool silent) {
	ScopedFile in(Open(path, silent));
	const size_t size = GetSize(in);

	std::string str(size, 0);
	Read(in, &str[0], size);
	return str;
}

bool StringToFile(const std::string &path, const std::string &str) {
	ScopedFile out(OpenWrite(path));
	const size_t size = str.length();
	return size ? Write(out, str.data(), size) == size : true;
}

//
std::string ReadString(File file) {
	const size_t len = Read<uint32_t>(file);
	std::string s(len, 0);
	Read(file, &s[0], len);
	return s;
}

bool WriteString(File file, const std::string &v) {
	const size_t len = v.length();
	return Write(file, numeric_cast<uint32_t>(len)) && Write(file, v.data(), len) == len;
}

bool WriteStringAsText(File file, const std::string &v) {
	return Write(file, v.data(), v.length()) == v.length();
}

} // namespace hg
