// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#pragma once

#include "foundation/time.h"

#include <string>
#include <vector>

namespace hg {

enum DirEntryType { DE_File = 0x1, DE_Dir = 0x2, DE_Link = 0x4, DE_All = 0xffff };

struct DirEntry {
	int type;
	std::string name;
	time_ns last_modified;
	size_t size;
};

std::vector<DirEntry> ListDir(const std::string &path, int mask = DE_All);
std::vector<DirEntry> ListDirRecursive(const std::string &path, int mask = DE_All);

size_t GetDirSize(const std::string &path);

bool MkDir(const std::string &path, int permissions = 1023, bool verbose = false); // 01777
bool RmDir(const std::string &path, bool verbose = false);

bool MkTree(const std::string &path, int permissions = 1023, bool verbose = false); // 01777
bool RmTree(const std::string &path, bool verbose = false);

bool IsDir(const std::string &path);

/// Copy a directory on the local filesystem, this function does not recurse through subdirectories.
/// @see CopyDirRecursive.
bool CopyDir(const std::string &src, const std::string &dst);
/// Copy a directory on the local filesystem, recurse through subdirectories.
bool CopyDirRecursive(const std::string &src, const std::string &dst);

/// Return `true` if a file exists on the local filesystem, `false` otherwise.
bool Exists(const std::string &path);

} // namespace hg
