// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "foundation/data.h"
#include "foundation/file.h"

namespace hg {

Data::Data()
	: data_(nullptr)
	, size_(0)
	, capacity_(0)
	, has_ownership(false)
	, cursor(0)
{}

Data::~Data() { Free(); }

Data &Data::operator=(const Data &data) {
	Free();

	if (data.has_ownership) {
		Reserve(data.size_);
		Write(data.data_, data.size_);
	} else {
		data_ = data.data_;
		size_ = data.size_;
	}

	has_ownership = data.has_ownership;
	cursor = data.cursor;
	return *this;
}

/*
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
*/

void Data::Reserve(size_t size) {
	const size_t new_capacity = (size / 8192 + 1) * 8192; // grow in 8KB increments

	if (new_capacity > capacity_) {
		uint8_t* _data_ = new uint8_t[new_capacity];

		if (data_)
			std::copy(data_, data_ + size_, _data_);

		if (has_ownership)
			delete[] data_;
		has_ownership = true;

		data_ = _data_;
		capacity_ = new_capacity;
	}
}

void Data::Resize(size_t size) {
	Reserve(size);

	size_ = size;

	if (size_ < cursor)
		cursor = size_;
}

void Data::Skip(size_t count) {
	Reserve(cursor + count);

	cursor += count;
	if (cursor > size_)
		size_ = cursor;
}

size_t Data::Write(const void *data, size_t size) {
	Reserve(cursor + size);

	std::copy(reinterpret_cast<const uint8_t *>(data), reinterpret_cast<const uint8_t *>(data) + size, data_ + cursor);

	cursor += size;
	if (cursor > size_)
		size_ = cursor;

	return size;
}

size_t Data::Read(void *data, size_t size) const {
	if (cursor + size > size_)
		size = size_ - cursor;

	std::copy(data_ + cursor, data_ + cursor + size, reinterpret_cast<uint8_t *>(data));
	cursor += size;

	return size;
}

void Data::Free() {
	if (has_ownership)
		delete[] data_;

	data_ = nullptr;
	size_ = 0;
	capacity_ = 0;

	has_ownership = false;
	cursor = 0;
}

//
bool Read(Data &data, std::string &str) {
	uint16_t size;
	if (!Read<uint16_t>(data, size))
		return false;

	std::vector<char> s_(size_t(size) + 1);
	if (!data.Read(s_.data(), size))
		return false;

	if (size)
		str = s_.data();
	else
		str.clear();
	return true;
}

bool Write(Data &data, const std::string &s) {
	const uint16_t size = uint16_t(s.size());
	return Write(data, size) && data.Write(s.data(), size) == size;
}

//
bool LoadDataFromFile(const std::string &path, Data &data) {
	const File file = Open(path);
	if (!IsValid(file))
		return false;

	const size_t size = GetSize(file);
	data.Reserve(size);
	Read(file, data.GetCursorPtr(), size);
	data.Skip(size);

	Close(file);
	return true;
}

bool SaveDataToFile(const std::string &path, const Data &data) {
	const File file = OpenWrite(path);
	if (!IsValid(file))
		return false;

	const size_t wsize = Write(file, data.GetData(), data.GetSize());

	Close(file);
	return wsize == data.GetSize();
}

} // namespace hg
