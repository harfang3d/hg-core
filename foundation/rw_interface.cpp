// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include <foundation/cext.h>
#include <foundation/data.h>
#include <foundation/rw_interface.h>
#include <vector>

namespace hg {

bool Exists(const Reader &ir, const ReadProvider &ip, const std::string &path) {
	bool res;

	Handle h = ip.open(path, true);

	if (ir.is_valid(h)) {
		res = true;
	} else {
		ip.close(h);
		res = false;
	}

	return res;
}

//
bool Read(const Reader &i, const Handle &h, std::string &v) {
	bool res;

	uint16_t size;
	if (Read<uint16_t>(i, h, size)) {
		std::vector<char> s_(static_cast<size_t>(size) + 1);

		if (i.read(h, s_.data(), size) == size) {
			if (size) {
				v = s_.data();
			} else {
				v.clear();
			}

			res = true;
		} else {
			res = false;
		}
	} else {
		res = false;
	}

	return res;
}

bool Write(const Writer &i, const Handle &h, const std::string &v) {
	const uint16_t size = numeric_cast<uint16_t>(v.size());
	return Write(i, h, size) && i.write(h, v.data(), size) == size;
}

//
bool SkipString(const Reader &i, const Handle &h) {
	bool res;

	uint16_t size;
	if (Read<uint16_t>(i, h, size)) {
		if (Seek(i, h, size, SM_Current)) {
			res = true;
		} else {
			res = false;
		}
	} else {
		res = false;
	}

	return res;
}

//
size_t Tell(const Reader &i, const Handle &h) {
	return i.tell(h);
}

size_t Tell(const Writer &i, const Handle &h) {
	return i.tell(h);
}

//
bool Seek(const Reader &i, const Handle &h, ptrdiff_t offset, SeekMode mode) {
	return i.seek(h, offset, mode);
}

bool Seek(const Writer &i, const Handle &h, ptrdiff_t offset, SeekMode mode) {
	return i.seek(h, offset, mode);
}

//
Data LoadData(const Reader &i, const Handle &h) {
	Data data;

	const size_t size = i.size(h);

	if (data.Skip(size)) {
		i.read(h, data.GetData(), data.GetSize());
	} else {
		i.seek(h, size, SM_Current);
	}

	return data;
}

} // namespace hg
