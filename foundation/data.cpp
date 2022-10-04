// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "foundation/data.h"
#include "foundation/file.h"

namespace hg {

Data::Data() : data_(nullptr), size_(0), capacity_(0), has_ownership(false), cursor(0) {}

Data::Data(size_t size) : data_(nullptr), size_(0), capacity_(0), has_ownership(false), cursor(0) {
	Resize(size);
}

Data::Data(const Data &data) : data_(nullptr), size_(0), capacity_(0), has_ownership(false), cursor(0) {
	*this = data;
}

Data::Data(const uint8_t *data, size_t size) : data_(nullptr), size_(0), capacity_(0), has_ownership(false), cursor(0) {
	Write(data, size);
}

Data::Data(uint8_t *data, size_t size) : data_(reinterpret_cast<uint8_t *>(data)), size_(size), capacity_(0), has_ownership(false), cursor(0) {}

Data::~Data() {
	Free();
}

Data &Data::operator=(const Data &data) {
	Free();

	if (data.has_ownership) {
		if (Reserve(data.size_)) {
			Write(data.data_, data.size_);
		}
	} else {
		data_ = data.data_;
		size_ = data.size_;
	}

	has_ownership = data.has_ownership;
	cursor = data.cursor;
	return *this;
}

#if __cplusplus >= 201103L
Data &Data::operator=(Data &&data) {
	Free();

	data_ = data.data_;
	size_ = data.size_;
	has_ownership = data.has_ownership;
	cursor = data.cursor;

	data.data_ = nullptr;
	data.size_ = 0;
	data.has_ownership = false;
	data.cursor = 0;

	return *this;
}
#endif

bool Data::Reserve(size_t size) {
	bool res = true;

	const size_t new_capacity = (size / 8192 + 1) * 8192; // grow in 8KB increments

	if (new_capacity > capacity_) {
		uint8_t *_data_ = new uint8_t[new_capacity];

		if (_data_ == nullptr) {
			res = false;
		} else {
			if (data_) {
				std::copy(data_, data_ + size_, _data_);
			}

			if (has_ownership) {
				delete[] data_;
			}
			has_ownership = true;

			data_ = _data_;
			capacity_ = new_capacity;
		}
	}

	return res;
}

bool Data::Resize(size_t size) {
	bool res = false;

	if (Reserve(size)) {
		size_ = size;

		if (size_ < cursor) {
			cursor = size_;
		}

		res = true;
	}

	return res;
}

bool Data::Skip(size_t count) {
	bool res;

	if (Reserve(cursor + count)) {
		cursor += count;

		if (cursor > size_) {
			size_ = cursor;
		}

		res = true;
	} else {
		res = false;
	}

	return res;
}

size_t Data::Write(const void *data, size_t size) {
	size_t res;

	if (Reserve(cursor + size)) {
		std::copy(reinterpret_cast<const uint8_t *>(data), reinterpret_cast<const uint8_t *>(data) + size, data_ + cursor);

		cursor += size;
		if (cursor > size_) {
			size_ = cursor;
		}

		res = size;
	} else {
		res = 0;
	}

	return size;
}

size_t Data::Read(void *data, size_t size) const {
	if (cursor + size > size_) {
		size = size_ - cursor;
	}

	if (size) {
		std::copy(data_ + cursor, data_ + cursor + size, reinterpret_cast<uint8_t *>(data));
		cursor += size;
	}

	return size;
}

void Data::Free() {
	if (has_ownership) {
		delete[] data_;
	}

	data_ = nullptr;
	size_ = 0;
	capacity_ = 0;

	has_ownership = false;
	cursor = 0;
}

//
bool Read(Data &data, std::string &str) {
	bool res;

	uint16_t size;
	if (Read<uint16_t>(data, size)) {
		std::vector<char> s_(static_cast<size_t>(size) + 1);

		if (data.Read(s_.data(), size) == size) {
			if (size) {
				str = s_.data();
			} else {
				str.clear();
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

bool Write(Data &data, const std::string &s) {
	const uint16_t size = static_cast<uint16_t>(s.size());
	return Write(data, size) && data.Write(s.data(), size) == size;
}

//
bool LoadDataFromFile(const std::string &path, Data &data) {
	bool res;

	const File file = Open(path);

	if (IsValid(file)) {
		const size_t size = GetSize(file);

		if (data.Reserve(size)) {
			Read(file, data.GetCursorPtr(), size);
			data.Skip(size);
			res = true;
		}

		Close(file);
	} else {
		res = false;
	}

	return res;
}

bool SaveDataToFile(const std::string &path, const Data &data) {
	bool res;

	const File file = OpenWrite(path);

	if (IsValid(file)) {
		const size_t wsize = Write(file, data.GetData(), data.GetSize());
		res = wsize == data.GetSize();

		Close(file);
	} else {
		res = false;
	}

	return res;
}

} // namespace hg
