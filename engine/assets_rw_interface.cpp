// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "engine/assets_rw_interface.h"
#include "engine/assets.h"

namespace hg {

static size_t assets_reader_read_impl(Handle hnd, void *data, size_t size) { return Read(reinterpret_cast<Asset &>(hnd), data, size); }
static size_t assets_reader_size_impl(Handle hnd) { return GetSize(reinterpret_cast<Asset &>(hnd)); }
static bool assets_reader_seek_impl(Handle hnd, ptrdiff_t offset, SeekMode mode) { return Seek(reinterpret_cast<Asset &>(hnd), offset, mode); }
static size_t assets_reader_tell_impl(Handle hnd) { return Tell(reinterpret_cast<Asset &>(hnd)); }
static bool assets_reader_valid_impl(Handle hnd) { return reinterpret_cast<Asset &>(hnd).ref != invalid_gen_ref; }
static bool assets_reader_eof_impl(Handle hnd) { return IsEOF(reinterpret_cast<Asset &>(hnd)); }

const Reader g_assets_reader = {
	assets_reader_read_impl, assets_reader_size_impl, assets_reader_seek_impl, assets_reader_tell_impl, assets_reader_valid_impl, assets_reader_eof_impl};

static Handle assets_read_provider_open_impl(const std::string &path, bool silent) {
	Handle hnd;
	reinterpret_cast<Asset &>(hnd) = OpenAsset(path, silent);
	return hnd;
}
static void assets_read_provider_close_impl(Handle hnd) { Close(reinterpret_cast<Asset &>(hnd)); }
static bool assets_read_provider_is_file_impl(const std::string &path) { return IsAssetFile(path); }

const ReadProvider g_assets_read_provider = {assets_read_provider_open_impl, assets_read_provider_close_impl, assets_read_provider_is_file_impl};

} // namespace hg
