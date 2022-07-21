// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#pragma once

#include "foundation/data.h"
#include "foundation/generational_vector_list.h"
#include "foundation/seek_mode.h"
#include "foundation/time.h"

#include <cstddef>
#include <string>

#if _WIN32
#undef CopyFile
#endif

namespace hg {

struct File {
	gen_ref ref;
};

File Open(const std::string &path, bool silent = false);
File OpenText(const std::string &path, bool silent = false);
File OpenWrite(const std::string &path);
File OpenWriteText(const std::string &path);
File OpenAppendText(const std::string &path);
/// Close a file handle.
bool Close(File file);

bool IsValid(File file);
bool IsEOF(File file);

size_t GetSize(File file);

size_t Read(File file, void *data, size_t size);
size_t Write(File file, const void *data, size_t size);

bool Seek(File file, ptrdiff_t offset, SeekMode mode);
size_t Tell(File file);

void Rewind(File file);

bool IsFile(const std::string &path);
bool Unlink(const std::string &path);

struct FileInfo {
	bool is_file;
	size_t size;
	time_ns created;
	time_ns modified;
};

FileInfo GetFileInfo(const std::string &path);

//
std::string ReadString(File file);
bool WriteString(File file, const std::string &v);
bool WriteStringAsText(File file, const std::string &v);

//
template <typename T> T Read(File file) {
	T v;
	Read(file, &v, sizeof(T));
	return v;
}

template <typename T> bool Write(File file, const T &v) { return Write(file, &v, sizeof(T)) == sizeof(T); }

/// Copy a file on the local filesystem.
bool CopyFile(const std::string &src, const std::string &dst);

/// Return the content of a file on the local filesystem as a string.
std::string FileToString(const std::string &path, bool silent = false);
/// Write a string as a file on the local filesystem.
bool StringToFile(const std::string &path, const std::string &str);

bool FileToData(const std::string &path, Data &data, bool silent = false);

//
struct ScopedFile {
	ScopedFile(File file) : f(file) {}
	~ScopedFile() { Close(f); }

	operator const File &() const { return f; }
	operator bool() const { return IsValid(f); }

	File f;
};

} // namespace hg
