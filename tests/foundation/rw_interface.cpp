// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.
#include <vector>
#include <algorithm>

#define TEST_NO_MAIN
#include "acutest.h"

#include "foundation/cext.h"

#include "foundation/rw_interface.h"

#include "foundation/data.h"

#include "../utils.h"

using namespace hg;

struct DummyReader {
	Data buffer;

	static size_t Read(Handle h, void *data, size_t size);
	static size_t Size(Handle h);
	static bool Seek(Handle h, ptrdiff_t offset, SeekMode mode);
	static size_t Tell(Handle h);
	static bool IsValid(Handle h);
	static bool IsEOF(Handle h);
};

size_t DummyReader::Read(Handle h, void* data, size_t size) { 
	DummyReader *impl = *(reinterpret_cast<DummyReader **>(&h.v[0])); 
	return impl->buffer.Read(data, size);
}

size_t DummyReader::Size(Handle h) {
	DummyReader *impl = *(reinterpret_cast<DummyReader **>(&h.v[0]));
	return impl->buffer.GetSize();
}

bool DummyReader::Seek(Handle h, ptrdiff_t offset, SeekMode mode) {
	DummyReader *impl = *(reinterpret_cast<DummyReader **>(&h.v[0]));
	if (mode == SM_Start) {
		if ((offset < 0) || (offset > impl->buffer.GetSize())) {
			return false;
		}
		impl->buffer.SetCursor(offset);
	} else if (mode == SM_Current) {
		if (offset >= 0) {
			if ((impl->buffer.GetCursor() + offset) > impl->buffer.GetSize()) {
				return false;
			}
		} else if (offset < 0) {
			if ((impl->buffer.GetCursor() + offset) < 0) {
				return false;
			}
		}
		impl->buffer.SetCursor(impl->buffer.GetCursor() + offset);
	} else {
		if ((offset < 0) || (offset > impl->buffer.GetSize())) {
			return false;
		}
		impl->buffer.SetCursor(impl->buffer.GetSize() - offset);
	}
	return true;
}

size_t DummyReader::Tell(Handle h) {
	DummyReader *impl = *(reinterpret_cast<DummyReader **>(&h.v[0]));
	return impl->buffer.GetCursor();
}

bool DummyReader::IsValid(Handle h) {
	DummyReader *impl = *(reinterpret_cast<DummyReader **>(&h.v[0]));
	return !impl->buffer.Empty();
}

bool DummyReader::IsEOF(Handle h) {
	DummyReader *impl = *(reinterpret_cast<DummyReader **>(&h.v[0]));
	return (impl->buffer.GetCursor() >= impl->buffer.GetSize());
}

static Handle DummyReaderOpen(const std::string& path, bool silent) {
	Handle h;
	DummyReader *impl = *(reinterpret_cast<DummyReader **>(&h.v[0]));
	impl = new DummyReader;
	return h;
}

static void DummyReaderClose(Handle h) {
	DummyReader *impl = *(reinterpret_cast<DummyReader **>(&h.v[0]));
	delete impl;
}

static bool DummyReaderIsFile(const std::string &path) { return !path.empty(); }

static void test_reader_interface() { 
	DummyReader dummy;

	Reader reader;
	reader.read = DummyReader::Read;
	reader.size = DummyReader::Size;
	reader.seek = DummyReader::Seek;
	reader.tell = DummyReader::Tell;
	reader.is_valid = DummyReader::IsValid;
	reader.is_eof = DummyReader::IsEOF;

	Handle h;
	*(reinterpret_cast<DummyReader **>(&h.v[0])) = &dummy;

#ifdef ENABLE_BINARY_DEBUG_HANDLE
	h.debug = true;
#endif

	{
		TEST_CHECK(Tell(reader, h) == 0);
#if defined(ENABLE_BINARY_DEBUG_HANDLE)
		TEST_CHECK(Write<uint16_t>(dummy.buffer, sizeof(uint16_t)) == true);
#endif
		TEST_CHECK(Write(dummy.buffer, hg::test::LoremIpsum) == true);
		dummy.buffer.Rewind();

		std::string str;
		TEST_CHECK(Read(reader, h, str) == true);
		TEST_CHECK(str == hg::test::LoremIpsum);
		TEST_CHECK(Tell(reader, h) == dummy.buffer.GetSize());
		TEST_CHECK(Seek(reader, h, 0, SM_Start) == true);
		TEST_CHECK(Tell(reader, h) == 0);
	}
#if defined(ENABLE_BINARY_DEBUG_HANDLE)
	{ 
		dummy.buffer.Reset();
		TEST_CHECK(Write<uint32_t>(dummy.buffer, 0xc0ffee) == true);

		uint32_t v;
		TEST_CHECK(Read<uint32_t>(reader, h, v) == false);
	}
#endif
	{ 
		std::string str;

		dummy.buffer.Reset();

		TEST_CHECK(Read(reader, h, str) == false);

		dummy.buffer.Reset();
#if defined(ENABLE_BINARY_DEBUG_HANDLE)
		TEST_CHECK(Write<uint16_t>(dummy.buffer, sizeof(uint16_t)) == true);
#endif
		TEST_CHECK(Write<uint16_t>(dummy.buffer, hg::test::LoremIpsum.size()) == true);
		TEST_CHECK(dummy.buffer.Write(&hg::test::LoremIpsum[0], 10) == 10);
		dummy.buffer.Rewind();

		TEST_CHECK(Read(reader, h, str) == false);

		dummy.buffer.Reset();

#if defined(ENABLE_BINARY_DEBUG_HANDLE)
		TEST_CHECK(Write<uint16_t>(dummy.buffer, sizeof(uint16_t)) == true);
#endif
		TEST_CHECK(Write<uint16_t>(dummy.buffer, 0) == true);
		dummy.buffer.Rewind();

		str = "this should be cleared";
		TEST_CHECK(Read(reader, h, str) == true);
		TEST_CHECK(str.empty());
	}

	{
		dummy.buffer.Reset();

		uint32_t v0 = 0xcafe;
#if defined(ENABLE_BINARY_DEBUG_HANDLE) 
		TEST_CHECK(Write<uint16_t>(dummy.buffer, sizeof(uint32_t)) == true);
#endif
		Write<uint32_t>(dummy.buffer, v0);
		dummy.buffer.Rewind();

		uint32_t v1;
		TEST_CHECK(Read<uint32_t>(reader, h, v1) == true);
		TEST_CHECK(v0 == v1);

		TEST_CHECK(Seek(reader, h, sizeof(uint32_t), SM_End) == true);
#if defined(ENABLE_BINARY_DEBUG_HANDLE)
		TEST_CHECK(Seek(reader, h, -sizeof(uint16_t), SM_Current) == true);
#endif
		TEST_CHECK(Tell(reader, h) == 0);
	}
	{
		dummy.buffer.Reset();

#if defined(ENABLE_BINARY_DEBUG_HANDLE)
		TEST_CHECK(Write<uint16_t>(dummy.buffer, sizeof(uint16_t)) == true);
#endif
		TEST_CHECK(Write(dummy.buffer, hg::test::LoremIpsum) == true);
			
		size_t after_string = Tell(reader, h);

		uint64_t v0 = 0xfacade;
#if defined(ENABLE_BINARY_DEBUG_HANDLE)
		TEST_CHECK(Write<uint16_t>(dummy.buffer, sizeof(uint64_t)) == true);
#endif
		TEST_CHECK(Write<uint64_t>(dummy.buffer, v0) == true);

		size_t after_facade = Tell(reader, h);

		uint64_t v1 = 0xf00dc0ffee;
#if defined(ENABLE_BINARY_DEBUG_HANDLE)
		TEST_CHECK(Write<uint16_t>(dummy.buffer, sizeof(uint64_t)) == true);
#endif
		TEST_CHECK(Write<uint64_t>(dummy.buffer, v1) == true);
			
		dummy.buffer.Rewind();

		TEST_CHECK(Tell(reader, h) == 0);

		TEST_CHECK(SkipString(reader, h) == true);

		TEST_CHECK(Tell(reader, h) == after_string);

		TEST_CHECK(Skip<uint64_t>(reader, h) == true);

		TEST_CHECK(Tell(reader, h) == after_facade);

		uint64_t v2;
		TEST_CHECK(Read<uint64_t>(reader, h, v2) == true);
		TEST_CHECK(v2 == v1);

		TEST_CHECK(SkipString(reader, h) == false);
		TEST_CHECK(Skip<uint32_t>(reader, h) == false);
	}
	{ 
		dummy.buffer.Reset();
#if defined(ENABLE_BINARY_DEBUG_HANDLE)
		TEST_CHECK(Write<uint16_t>(dummy.buffer, sizeof(uint16_t)) == true);
#endif
		TEST_CHECK(Write(dummy.buffer, hg::test::LoremIpsum) == true);
		dummy.buffer.Rewind();

		Data d0 = LoadData(reader, h);
		TEST_CHECK(d0.Empty() == false);
		TEST_CHECK(d0.GetCursor() == d0.GetSize());

		d0.Rewind();

		std::string str;
#if defined(ENABLE_BINARY_DEBUG_HANDLE)
		TEST_CHECK(d0.Skip(sizeof(uint16_t)) == true);
#endif
		TEST_CHECK(Read(d0, str) == true);
		TEST_CHECK(str == hg::test::LoremIpsum);
	}
}

/*
ReadProvider provider;
provider.open = DummyReaderOpen;
provider.close = DummyReaderClose;
provider.is_file = DummyReaderIsFile;
*/

/*
struct Writer {
	size_t (*write)(Handle h, const void *data, size_t size);
	bool (*seek)(Handle h, ptrdiff_t offset, SeekMode mode);
	size_t (*tell)(Handle h);
	bool (*is_valid)(Handle hnd);
};

template <typename T> bool Write(const Writer &i, const Handle &h, const T &v) {
	if (h.debug) {
		uint16_t _check = sizeof(T);
		if (i.write(h, &_check, sizeof(uint16_t)) != sizeof(uint16_t))
			return false;
	}
	return i.write(h, &v, sizeof(T)) == sizeof(T);
}
#else
template <typename T> bool Write(const Writer &i, const Handle &h, const T &v) { return i.write(h, &v, sizeof(T)) == sizeof(T); }
#endif

//
bool Write(const Writer &i, const Handle &h, const std::string &v);

//
template <typename T> T Read(const Reader &i, const Handle &h) {
	T v;
	bool r = Read(i, h, v);
	__ASSERT__(r == true);
	return v;
}

//
struct ReadProvider {
	Handle (*open)(const std::string &path, bool silent);
	void (*close)(Handle hnd);
	bool (*is_file)(const std::string &path);
};

bool Exists(const Reader &ir, const ReadProvider &i, const std::string &path);

struct WriteProvider {
	Handle (*open)(const std::string &path);
	void (*close)(Handle hnd);
};

//
struct ScopedReadHandle {
#ifdef ENABLE_BINARY_DEBUG_HANDLE
	ScopedReadHandle(ReadProvider i, const std::string &path, bool silent = false, bool debug = false) : i_(i), h_(i.open(path, silent)) { h_.debug = debug; }
#else
	ScopedReadHandle(ReadProvider i, const std::string &path, bool silent = false) : i_(i), h_(i.open(path, silent)) {}
#endif
	~ScopedReadHandle() { i_.close(h_); }

	operator const Handle &() const { return h_; }

private:
	Handle h_;
	ReadProvider i_;
};

struct ScopedWriteHandle {
#ifdef ENABLE_BINARY_DEBUG_HANDLE
	ScopedWriteHandle(WriteProvider i, const std::string &path, bool debug = false) : i_(i), h_(i.open(path)) { h_.debug = debug; }
#else
	ScopedWriteHandle(WriteProvider i, const std::string &path) : i_(i), h_(i.open(path)) {}
#endif
	~ScopedWriteHandle() { i_.close(h_); }

	operator const Handle &() const { return h_; }

private:
	Handle h_;
	WriteProvider i_;
};

//
template <typename T> struct DeferredWrite {
	DeferredWrite(const Writer &iw_, const Handle &h_) : iw(iw_), h(h_) {
		cursor = Tell(iw, h);

#ifdef ENABLE_BINARY_DEBUG_HANDLE
		if (h.debug)
			Seek(iw, h, sizeof(uint16_t), SM_Current); // leave space for debug size marker
#endif
		Seek(iw, h, sizeof(T), SM_Current); // leave space for deferred write
	}

	bool Commit(const T &v) {
		const size_t seek_ = Tell(iw, h);

		if (!Seek(iw, h, cursor, SM_Start) || !Write(iw, h, v) || !Seek(iw, h, seek_, SM_Start))
			return false;

		return true;
	}

	size_t cursor;

	const Writer &iw;
	const Handle &h;
};
*/

void test_rw_interface() { 
	test_reader_interface();	
}