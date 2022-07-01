// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "foundation/cext.h"
#include "foundation/data_rw_interface.h"
#include "foundation/data.h"

namespace hg {

static size_t data_reader_read_impl(Handle hnd, void *data, size_t size) { return (*reinterpret_cast<const Data **>(&hnd))->Read(data, size); }
static size_t data_reader_size_impl(Handle hnd) { return (*reinterpret_cast<const Data **>(&hnd))->GetSize(); }
static bool data_reader_seek_impl(Handle hnd, ptrdiff_t offset, SeekMode mode) {
	Data* data = (*reinterpret_cast<Data **>(&hnd));

	if (mode == SM_Start)
		data->SetCursor(offset);
	else if (mode == SM_Current)
		data->SetCursor(data->GetCursor() + offset);
	else if (mode == SM_End)
		data->SetCursor(data->GetSize() + offset);

	return true;
}
static size_t data_reader_tell_impl(Handle hnd) { return (*reinterpret_cast<const Data **>(&hnd))->GetCursor(); }
static bool data_reader_is_valid_impl(Handle hnd) { return *reinterpret_cast<const Data **>(&hnd) != nullptr; }
static bool data_reader_is_eof_impl(Handle hnd) {
	const Data *data = *reinterpret_cast<const Data **>(&hnd);
	return data->GetCursor() >= data->GetSize();
}

const Reader g_data_reader = {
	data_reader_read_impl,
	data_reader_size_impl,
	data_reader_seek_impl,
	data_reader_tell_impl,
	data_reader_is_valid_impl,
	data_reader_is_eof_impl
};

size_t data_writer_write_impl(Handle hnd, const void *data, size_t size) { return (*reinterpret_cast<Data **>(&hnd))->Write(data, size); }
bool data_writer_seek_impl (Handle hnd, ptrdiff_t offset, SeekMode mode) {
	Data* data = (*reinterpret_cast<Data **>(&hnd));

	if (mode == SM_Start)
		data->SetCursor(offset);
	else if (mode == SM_Current)
		data->SetCursor(data->GetCursor() + offset);
	else if (mode == SM_End)
		data->SetCursor(data->GetSize() + offset);

	return true;
}
size_t data_writer_tell_impl(Handle hnd) { return (*reinterpret_cast<const Data **>(&hnd))->GetCursor(); }
bool data_writer_is_valid_impl(Handle hnd) { return *reinterpret_cast<Data **>(&hnd) != nullptr; }

const Writer g_data_writer = {
	data_writer_write_impl,
	data_writer_seek_impl,
	data_writer_tell_impl,
	data_writer_is_valid_impl,
};

#ifdef ENABLE_BINARY_DEBUG_HANDLE
DataReadHandle::DataReadHandle(const Data &data, bool debug) {
	*reinterpret_cast<const Data **>(&h_) = &data;
	h_.debug = debug;
}

DataWriteHandle::DataWriteHandle(Data &data, bool debug) {
	*reinterpret_cast<Data **>(&h_) = &data;
	h_.debug = debug;
}
#else
DataReadHandle::DataReadHandle(const Data &data) { *reinterpret_cast<const Data **>(&h_) = &data; }
DataWriteHandle::DataWriteHandle(Data &data) { *reinterpret_cast<Data **>(&h_) = &data; }
#endif

} // namespace hg
