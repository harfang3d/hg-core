// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "engine/assets.h"

#include "foundation/cext.h"
#include "foundation/dir.h"
#include "foundation/file.h"
#include "foundation/log.h"
#include "foundation/path_tools.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <deque>
#include <fmt/format.h>
#include <functional>
#include <memory>
#include <miniz.h>
#include <set>
#include <string>
#include <vector>

namespace hg {

enum AssetsSourceType { AssetsFolder, AssetsPackage };

struct AssetsSource {
	AssetsSourceType type;
	std::string name;
};

//
static std::deque<std::string> assets_folders;

bool AddAssetsFolder(const std::string &path) {
	if (std::find(assets_folders.begin(), assets_folders.end(), path) == assets_folders.end()) {
		assets_folders.push_front(path);
		return true;
	}
	return false;
}

void RemoveAssetsFolder(const std::string &path) {
	for (std::deque<std::string>::iterator i = assets_folders.begin(); i != assets_folders.end();)
		if (*i == path)
			i = assets_folders.erase(i);
		else
			++i;
}

//
struct ZipPackage {
	mz_zip_archive archive;
	std::string filename;

	ZipPackage() { mz_zip_zero_struct(&archive); }
	~ZipPackage() { mz_zip_reader_end(&archive); }
};

static std::deque<ZipPackage> assets_packages;

bool AddAssetsPackage(const std::string &path) {
	for (std::deque<ZipPackage>::iterator i = assets_packages.begin(); i != assets_packages.end(); ++i)
		if (i->filename == path)
			return false;

	ZipPackage pkg;

	pkg.filename = path;
	if (mz_zip_reader_init_file(&pkg.archive, path.c_str(), MZ_ZIP_FLAG_VALIDATE_HEADERS_ONLY) == MZ_FALSE)
		return false;

	assets_packages.push_front(pkg);
	return true;
}

void RemoveAssetsPackage(const std::string &path) {
	for (std::deque<ZipPackage>::iterator i = assets_packages.begin(); i != assets_packages.end();)
		if (i->filename == path)
			i = assets_packages.erase(i);
		else
			++i;
}

struct PackageFile {
	std::vector<uint8_t> data;
	ptrdiff_t cursor;
};

//
struct Asset_ {
	File file;
	PackageFile pkg_file;

	size_t (*get_size)(Asset_ &asset);
	size_t (*read)(Asset_ &asset, void *data, size_t size);
	bool (*seek)(Asset_ &asset, ptrdiff_t offset, SeekMode mode);
	size_t (*tell)(Asset_ &asset);
	void (*close)(Asset_ &asset);
	bool (*is_eof)(Asset_ &asset);
};

//
static size_t Asset_file_GetSize(Asset_ &asset) { return GetSize(asset.file); }
static size_t Asset_file_Read(Asset_ &asset, void *data, size_t size) { return Read(asset.file, data, size); }
static bool Asset_file_Seek(Asset_ &asset, ptrdiff_t offset, SeekMode mode) { return Seek(asset.file, offset, mode); }
static size_t Asset_file_Tell(Asset_ &asset) { return Tell(asset.file); }
static void Asset_file_Close(Asset_ &asset) { Close(asset.file); }
static bool Asset_file_is_EOF(Asset_ &asset) { return IsEOF(asset.file); }

//
static size_t Package_file_GetSize(Asset_ &asset) { return asset.pkg_file.data.size(); }
static size_t Package_file_Read(Asset_ &asset, void *data, size_t size) {
	if (asset.pkg_file.cursor + size <= asset.pkg_file.data.size()) {
		memcpy(data, asset.pkg_file.data.data() + sizeof(char) * asset.pkg_file.cursor, size);
		asset.pkg_file.cursor += size;
		return size;
	}
	return 0;
}
static bool Package_file_Seek(Asset_ &asset, ptrdiff_t offset, SeekMode mode) {
	if (offset <= asset.pkg_file.data.size()) {
		asset.pkg_file.cursor = offset;
		return true;
	}
	return false;
}
static size_t Package_file_Tell(Asset_ &asset) { return asset.pkg_file.cursor; }
static void Package_file_Close(Asset_ &asset) {}
static bool Package_file_is_EOF(Asset_ &asset) { return asset.pkg_file.cursor >= asset.pkg_file.data.size(); }

//
static generational_vector_list<Asset_> assets;

std::string FindAssetPath(const std::string &name) {
	for (std::deque<std::string>::iterator i = assets_folders.begin(); i != assets_folders.end(); ++i) {
		const std::string asset_path = PathJoin(*i, name);
		if (IsFile(asset_path))
			return asset_path;
	}
	return "";
}

Asset OpenAsset(const std::string &name, bool silent) {
	// look on filesystem
	for (std::deque<std::string>::iterator i = assets_folders.begin(); i != assets_folders.end(); ++i) {
		const std::string asset_path = PathJoin(*i, name);

		const File file = Open(asset_path, true);

		if (IsValid(file)) {
			Asset_ asset_;
			asset_.file = file;
			asset_.get_size = Asset_file_GetSize;
			asset_.read = Asset_file_Read;
			asset_.seek = Asset_file_Seek;
			asset_.tell = Asset_file_Tell;
			asset_.close = Asset_file_Close;
			asset_.is_eof = Asset_file_is_EOF;

			Asset asset;
			asset.ref = assets.add_ref(asset_);

			return asset;
		}
	}

	// look in archive
	for (std::deque<ZipPackage>::iterator i = assets_packages.begin(); i != assets_packages.end(); ++i) {
		const int index = mz_zip_reader_locate_file(&i->archive, name.c_str(), nullptr, MZ_ZIP_FLAG_CASE_SENSITIVE);
		if (index == -1)
			continue; // missing file

		size_t size;
		void *data = mz_zip_reader_extract_to_heap(&i->archive, index, &size, 0);

		if (data == nullptr) {
			const mz_zip_error err = mz_zip_get_last_error(&i->archive);

			if (!silent)
				warn(fmt::format(
					"Failed to open asset '{}' from file '{}' (asset was found but failed to open): {}", name, i->filename, mz_zip_get_error_string(err)));
			break;
		}

		Asset_ asset_;
		asset_.pkg_file.data.resize(size);
		memcpy(asset_.pkg_file.data.data(), data, size);
		mz_free(data);
		asset_.pkg_file.cursor = 0;
		asset_.get_size = Package_file_GetSize;
		asset_.read = Package_file_Read;
		asset_.seek = Package_file_Seek;
		asset_.tell = Package_file_Tell;
		asset_.close = Package_file_Close;
		asset_.is_eof = Package_file_is_EOF;

		Asset asset;
		asset.ref = assets.add_ref(asset_);

		return asset;
	}

	if (!silent)
		warn(fmt::format("Failed to open asset '{}' (file not found)", name));

	return Asset();
}

void Close(Asset asset) {
	if (assets.is_valid(asset.ref)) {
		Asset_ &asset_ = assets[asset.ref.idx];
		asset_.close(asset_);
		assets.remove_ref(asset.ref);
	}
}

bool IsAssetFile(const std::string &name) {
	for (std::deque<std::string>::iterator i = assets_folders.begin(); i != assets_folders.end(); ++i)
		if (IsFile(PathJoin(*i, name)))
			return true;

	// look in archive
	for (std::deque<ZipPackage>::iterator i = assets_packages.begin(); i != assets_packages.end(); ++i)
		if (mz_zip_reader_locate_file(&i->archive, name.c_str(), nullptr, MZ_ZIP_FLAG_CASE_SENSITIVE) != -1)
			return true;

	return false;
}

size_t GetSize(Asset asset) {
	if (assets.is_valid(asset.ref)) {
		Asset_ &asset_ = assets[asset.ref.idx];
		return asset_.get_size(asset_);
	}
	return 0;
}

size_t Read(Asset asset, void *data, size_t size) {
	if (assets.is_valid(asset.ref)) {
		Asset_ &asset_ = assets[asset.ref.idx];
		return asset_.read(asset_, data, size);
	}
	return 0;
}

bool Seek(Asset asset, ptrdiff_t offset, SeekMode mode) {
	if (assets.is_valid(asset.ref)) {
		Asset_ &asset_ = assets[asset.ref.idx];
		return asset_.seek(asset_, offset, mode);
	}
	return false;
}

size_t Tell(Asset asset) {
	if (assets.is_valid(asset.ref)) {
		Asset_ &asset_ = assets[asset.ref.idx];
		return asset_.tell(asset_);
	}
	return 0;
}

bool IsEOF(Asset asset) {
	if (assets.is_valid(asset.ref)) {
		Asset_ &asset_ = assets[asset.ref.idx];
		return asset_.is_eof(asset_);
	}
	return false;
}

//
std::string AssetToString(const std::string &name) {
	const Asset h = OpenAsset(name);
	if (h.ref == invalid_gen_ref)
		return "";
	const size_t size = GetSize(h);
	std::string str;
	str.resize(size + 1);
	Read(h, &str[0], size);
	Close(h);
	return str;
}

Data AssetToData(const std::string &name) {
	Data data;
	Asset asset = OpenAsset(name);

	if (IsValid(asset)) {
		data.Resize(GetSize(asset));
		Read(asset, data.GetData(), data.GetSize());
		Close(asset);
	}

	return data;
}

} // namespace hg
