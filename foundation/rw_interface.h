// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#pragma once

#include <stdint.h>

#include "foundation/assert.h"
#include "foundation/seek_mode.h"

#include <cstddef>
#include <string>

namespace hg {

#ifndef NDEBUG
#define ENABLE_BINARY_DEBUG_HANDLE
#endif

struct Handle { // 16 bytes
	uint32_t v[4];
#ifdef ENABLE_BINARY_DEBUG_HANDLE
	bool debug;
#endif
};

struct Reader {
	size_t (*read)(Handle h, void *data, size_t size);
	size_t (*size)(Handle h);
	bool (*seek)(Handle h, ptrdiff_t offset, SeekMode mode);
	size_t (*tell)(Handle h);
	bool (*is_valid)(Handle h);
	bool (*is_eof)(Handle h);
};

struct Writer {
	size_t (*write)(Handle h, const void *data, size_t size);
	bool (*seek)(Handle h, ptrdiff_t offset, SeekMode mode);
	size_t (*tell)(Handle h);
	bool (*is_valid)(Handle hnd);
};

//
bool Seek(const Reader &i, const Handle &h, ptrdiff_t offset, SeekMode mode);
bool Seek(const Writer &i, const Handle &h, ptrdiff_t offset, SeekMode mode);

//
#ifdef ENABLE_BINARY_DEBUG_HANDLE

template <typename T> bool Read(const Reader &i, const Handle &h, T &v) {
	bool res = true;

	if (h.debug) {
		uint16_t _check;
		if (i.read(h, &_check, sizeof(uint16_t)) != sizeof(uint16_t)) {
			res = false;
		} else {
			HG_ASSERT(_check == sizeof(T));
		}
	}

	if (res && (i.read(h, &v, sizeof(T)) != sizeof(T))) {
		res = false;
	}
	return res;
}

template <typename T> bool Write(const Writer &i, const Handle &h, const T &v) {
	bool res = true;

	if (h.debug) {
		const uint16_t _check = sizeof(T);
		if (i.write(h, &_check, sizeof(uint16_t)) != sizeof(uint16_t)) {
			res = false;
		}
	}

	if (res && (i.write(h, &v, sizeof(T)) != sizeof(T))) {
		res = false;
	}
	return res;
}

template <typename T> bool Skip(const Reader &i, const Handle &h) {
	bool res = true;

	if (h.debug) {
		uint16_t _check;
		if (i.read(h, &_check, sizeof(uint16_t)) != sizeof(uint16_t)) {
			res = false;
		} else {
			HG_ASSERT(_check == sizeof(T));
		}
	}

	if (res && (i.seek(h, sizeof(T), SM_Current) == false)) {
		res = false;
	}
	return res;
}

#else

template <typename T> bool Read(const Reader &i, const Handle &h, T &v) {
	return i.read(h, &v, sizeof(T)) == sizeof(T);
}

template <typename T> bool Write(const Writer &i, const Handle &h, const T &v) {
	return i.write(h, &v, sizeof(T)) == sizeof(T);
}

template <typename T> bool Skip(const Reader &i, const Handle &h) {
	return Seek(i, h, sizeof(T), SM_Current);
}

#endif

//
bool Read(const Reader &i, const Handle &h, std::string &v);
bool Write(const Writer &i, const Handle &h, const std::string &v);

//
size_t Tell(const Reader &i, const Handle &h);
size_t Tell(const Writer &i, const Handle &h);

//
template <typename T> T Read(const Reader &i, const Handle &h) {
	T v;
	bool r = Read(i, h, v);
	HG_ASSERT(r == true);
	return v;
}

bool SkipString(const Reader &i, const Handle &h);

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
	ScopedReadHandle(ReadProvider i, const std::string &path, bool silent = false, bool debug = false) : i_(i), h_(i.open(path, silent)) {
		h_.debug = debug;
	}
#else
	ScopedReadHandle(ReadProvider i, const std::string &path, bool silent = false) : i_(i), h_(i.open(path, silent)) {}
#endif
	~ScopedReadHandle() {
		i_.close(h_);
	}

	operator const Handle &() const {
		return h_;
	}

private:
	Handle h_;
	ReadProvider i_;
};

struct ScopedWriteHandle {
#ifdef ENABLE_BINARY_DEBUG_HANDLE
	ScopedWriteHandle(WriteProvider i, const std::string &path, bool debug = false) : i_(i), h_(i.open(path)) {
		h_.debug = debug;
	}
#else
	ScopedWriteHandle(WriteProvider i, const std::string &path) : i_(i), h_(i.open(path)) {}
#endif
	~ScopedWriteHandle() {
		i_.close(h_);
	}

	operator const Handle &() const {
		return h_;
	}

private:
	Handle h_;
	WriteProvider i_;
};

//
class Data;

Data LoadData(const Reader &i, const Handle &h);

//
template <typename T> struct DeferredWrite {
	DeferredWrite(const Writer &iw_, const Handle &h_) : iw(iw_), h(h_) {
		cursor = Tell(iw, h);

#ifdef ENABLE_BINARY_DEBUG_HANDLE
		if (h.debug) {
			Seek(iw, h, sizeof(uint16_t), SM_Current); // leave space for debug size marker
		}
#endif
		Seek(iw, h, sizeof(T), SM_Current); // leave space for deferred write
	}

	bool Commit(const T &v) {
		const size_t seek_ = Tell(iw, h);

		bool res = true;
		if (!Seek(iw, h, cursor, SM_Start) || !Write(iw, h, v) || !Seek(iw, h, seek_, SM_Start)) {
			res = false;
		}

		return res;
	}

	size_t cursor;

	const Writer &iw;
	const Handle &h;
};

} // namespace hg
