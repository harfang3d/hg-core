// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "foundation/file_rw_interface.h"
#include "foundation/file.h"

namespace hg {

static size_t file_reader_read_impl(Handle hnd, void *data, size_t size) { return Read(reinterpret_cast<File &>(hnd), data, size); }
static size_t file_reader_size_impl(Handle hnd) { return GetSize(reinterpret_cast<File &>(hnd)); }
static bool file_reader_seek_impl(Handle hnd, ptrdiff_t offset, SeekMode mode) { return Seek(reinterpret_cast<File &>(hnd), offset, mode); }
static size_t file_reader_tell_impl(Handle hnd) { return Tell(reinterpret_cast<File &>(hnd)); }
static bool file_reader_is_valid_impl(Handle hnd) { return reinterpret_cast<File &>(hnd).ref != invalid_gen_ref; }
static bool file_reader_is_eof_impl(Handle hnd) { return IsEOF(reinterpret_cast<File &>(hnd)); }

const Reader g_file_reader = {
	file_reader_read_impl,
	file_reader_size_impl,
	file_reader_seek_impl,
	file_reader_tell_impl,
	file_reader_is_valid_impl,
	file_reader_is_eof_impl
};

static size_t file_writer_write_impl(Handle hnd, const void *data, size_t size) { return Write(reinterpret_cast<File &>(hnd), data, size); }
static bool file_writer_seek_impl(Handle hnd, ptrdiff_t offset, SeekMode mode) { return Seek(reinterpret_cast<File &>(hnd), offset, mode); }
static size_t file_writer_tell_impl(Handle hnd) { return Tell(reinterpret_cast<File &>(hnd)); }
static bool file_writer_is_valid_impl(Handle hnd) { return reinterpret_cast<File &>(hnd).ref != invalid_gen_ref; }

const Writer g_file_writer = {
	file_writer_write_impl,
	file_writer_seek_impl,
	file_writer_tell_impl,
	file_writer_is_valid_impl
};

static Handle read_provider_open_impl(const std::string &path, bool silent) {
	Handle hnd;
	reinterpret_cast<File &>(hnd) = Open(path, silent);
	return hnd;
}
static void read_provide_close_impl(Handle hnd) { Close(reinterpret_cast<File &>(hnd)); }
static bool read_provider_is_file_impl(const std::string &path) { return IsFile(path); }

const ReadProvider g_file_read_provider = {
	read_provider_open_impl,
	read_provide_close_impl,
	read_provider_is_file_impl
};

static Handle write_provider_open_impl(const std::string &path) {
	Handle hnd;
	reinterpret_cast<File &>(hnd) = OpenWrite(path);
	return hnd;
}
static void write_provider_close_impl(Handle hnd) { Close(reinterpret_cast<File &>(hnd)); }

const WriteProvider g_file_write_provider = {
	write_provider_open_impl,
	write_provider_close_impl
};

} // namespace hg
