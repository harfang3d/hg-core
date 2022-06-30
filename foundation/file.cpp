// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "foundation/file.h"
#include "foundation/cext.h"
#include "foundation/dir.h"
#include "foundation/log.h"
#include "foundation/rand.h"
#include "foundation/string.h"

#if _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

//#include <cstdio>
#include <fmt/format.h>

#undef CopyFile

namespace hg {

static generational_vector_list<FILE *> files;

static FILE *_Open(const std::string &path, const std::string &mode, bool silent = false) {
	FILE *file = nullptr;
#if _WIN32
	auto wpath = utf8_to_wchar(path);
	auto wmode = utf8_to_wchar(mode);

	const auto err = _wfopen_s(&file, wpath.data(), wmode.data());

	if (!silent && err != 0) {
		char errmsg[256];
		strerror_s(errmsg, 255, err);
		warn(fmt::format("Failed to open file '{}' mode '{}', error code {} ({})", path, mode, err, errmsg));
	}
#else
	file = fopen(path, mode);
#endif
	return file;
}

File Open(const std::string &path, bool silent) {
	auto *f = _Open(path, "rb", silent);

	if (f)
		return {files.add_ref(f)};

	return {invalid_gen_ref};
}

File OpenText(const std::string &path, bool silent) {
	auto *f = _Open(path, "r", silent);

	if (f)
		return {files.add_ref(f)};

	return {invalid_gen_ref};
}

File OpenWrite(const std::string &path) {
	auto *f = _Open(path, "wb");

	if (f)
		return {files.add_ref(f)};

	return {invalid_gen_ref};
}

File OpenWriteText(const std::string &path) {
	auto *f = _Open(path, "w");

	if (f)
		return {files.add_ref(f)};

	return {invalid_gen_ref};
}

File OpenAppendText(const std::string &path) {
	auto *f = _Open(path, "a");

	if (f)
		return {files.add_ref(f)};

	return {invalid_gen_ref};
}

bool Close(File file) {
	if (!files.is_valid(file.ref))
		return false;
	fclose(files[file.ref.idx]);
	files.remove_ref(file.ref);
	return true;
}

bool IsValid(File file) { return files.is_valid(file.ref); }

bool IsEOF(File file) { return files.is_valid(file.ref) ? feof(files[file.ref.idx]) != 0 : true; }

size_t GetSize(File file) {
	if (!files.is_valid(file.ref))
		return 0;
	const auto s = files[file.ref.idx];
	const auto t = ftell(s);
	fseek(s, 0, SEEK_END);
	const auto size = ftell(s);
	fseek(s, t, SEEK_SET);
	return size;
}

size_t Read(File file, void *data, size_t size) { return files.is_valid(file.ref) ? fread(data, 1, size, files[file.ref.idx]) : 0; }

size_t Write(File file, const void *data, size_t size) { return files.is_valid(file.ref) ? fwrite(data, 1, size, files[file.ref.idx]) : 0; }

bool Seek(File file, ptrdiff_t offset, SeekMode mode) {
	int _mode;
	if (mode == SM_Start)
		_mode = SEEK_SET;
	else if (mode == SM_Current)
		_mode = SEEK_CUR;
	else
		_mode = SEEK_END;
	return files.is_valid(file.ref) ? (fseek(files[file.ref.idx], long(offset), _mode) == 0) : false;
}

size_t Tell(File file) { return files.is_valid(file.ref) ? ftell(files[file.ref.idx]) : 0; }

void Rewind(File file) {
	if (files.is_valid(file.ref))
		fseek(files[file.ref.idx], 0, SEEK_SET);
}

//
FileInfo GetFileInfo(const std::string &path) {
#if WIN32
	struct _stat info;
	const auto wpath = utf8_to_wchar(path);
	if (_wstat(wpath.c_str(), &info) != 0)
		return {false, 0, 0, 0};
#else
	struct stat info;
	if (stat(path, &info) != 0)
		return {false, 0, 0, 0};
#endif
	return {info.st_mode & S_IFREG ? true : false, size_t(info.st_size), time_ns(info.st_ctime), time_ns(info.st_mtime)};
}

//
bool IsFile(const std::string &path) {
#if WIN32
	struct _stat info;
	const auto wpath = utf8_to_wchar(path);
	if (_wstat(wpath.c_str(), &info) != 0)
		return false;
#else
	struct stat info;
	if (stat(path, &info) != 0)
		return false;
#endif

	if (info.st_mode & S_IFREG)
		return true;
	return false;
}

bool Unlink(const std::string &path) {
#if _WIN32
	const auto wpath = utf8_to_wchar(path);
	return DeleteFileW(wpath.c_str()) == TRUE;
#else
	return unlink(path) == 0;
#endif
}

//
bool CopyFile(const std::string &src, const std::string &dst) {
#if _WIN32
	const auto wsrc = utf8_to_wchar(src);
	const auto wdst = utf8_to_wchar(dst);
	return ::CopyFileW(wsrc.c_str(), wdst.c_str(), FALSE) ? true : false;
#else
	ScopedFile in(Open(src));
	if (!in)
		return false;

	ScopedFile out(OpenWrite(dst));

	std::vector<char> data(65536); // 64KB copy

	while (!IsEOF(in)) {
		const auto size = Read(in, data.data(), data.size());
		if (size == 0)
			break;
		if (Write(out, data.data(), size) != size)
			return false;
	}
	return true;
#endif
}

//
Data FileToData(const std::string &path, bool silent) {
	Data data;
	File file = Open(path, silent);

	if (IsValid(file)) {
		data.Resize(GetSize(file));
		Read(file, data.GetData(), data.GetSize());
		Close(file);
	}

	return data;
}

//
std::string FileToString(const std::string &path, bool silent) {
	ScopedFile in(Open(path, silent));
	const auto size = GetSize(in);

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
	const auto len = Read<uint32_t>(file);
	std::string s(len, 0);
	Read(file, &s[0], len);
	return s;
}

bool WriteString(File file, const std::string &v) {
	const auto len = v.length();
	return Write(file, numeric_cast<uint32_t>(len)) && Write(file, v.data(), len) == len;
}

bool WriteStringAsText(File file, const std::string &v) { return Write(file, v.data(), v.length()) == v.length(); }

} // namespace hg
