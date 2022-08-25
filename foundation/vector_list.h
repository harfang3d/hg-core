// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#pragma once

#include <cstring>
#include <iterator>
#include <vector>

namespace hg {

/*
	vector_list (for lack of a better name)

	- A container of T with linear lookup/add/remove by key.
	- Store less than 16000000 entries.
	- Automatically recycle indexes.
	- Can defragment storage.
	- Fast iteration.

	Memory usage: size() * sizeof(T) + highest index * sizeof(uint32_t) + k

	compact() deals with storage fragmentation issues and should be called when
	storage() is vastly superior to size() (2-3x larger).

	shrink() shrink the index table to the minimum size possible.
*/
template <typename T> class vector_list {
public:
	static const uint32_t invalid_idx = 0xffffffffU;

	vector_list() : storage_capacity_(0), storage_(nullptr), idx_(), size_(0), free_(0) {}
	explicit vector_list(size_t count) : storage_capacity_(0), storage_(nullptr), idx_(), size_(0), free_(0) {
		reserve(count);
	}

	//
	void reserve(size_t count) {
		HG_ASSERT(count < 0x00ffffffU);
		const uint32_t capacity_ = static_cast<uint32_t>(idx_.size());

		if (count > capacity_) {
			reserve_storage_(count);

			idx_.resize(count);
			for (uint32_t i = capacity_; i < count; ++i) {
				idx_[i] = make_free_idx(i + 1, 1);
			}
		}
	}

	size_t size() const {
		return size_;
	}

	size_t capacity() const {
		return idx_.size();
	}

	void clear() {
		for (uint32_t i = first(); i != invalid_idx; i = next(i)) {
			reinterpret_cast<T *>(storage_)[i].~T();
		}

		free(storage_);
		storage_ = nullptr;
		storage_capacity_ = 0;

		idx_.clear();

		size_ = 0;
		free_ = 0;
	}

	//
	T &operator[](size_t i) {
		return reinterpret_cast<T *>(storage_)[idx_[i]];
	}

	const T &operator[](size_t i) const {
		return reinterpret_cast<const T *>(storage_)[idx_[i]];
	}

	T &value(size_t i) {
		return reinterpret_cast<T *>(storage_)[idx_[i]];
	}

	const T &value(size_t i) const {
		return reinterpret_cast<const T *>(storage_)[idx_[i]];
	}

	//
	uint32_t first() const {
		const size_t idx_count = idx_.size();

		uint32_t i = 0;

		while (i < idx_count) {
			const uint32_t idx = idx_[i];

			if (!is_free_idx(idx)) {
				break;
			}

			i += get_free_skip(idx);
		}

		return i == idx_count ? invalid_idx : i;
	}

	uint32_t next(uint32_t from) const {
		const size_t idx_count = idx_.size();

		++from;
		while (from < idx_count) {
			const uint32_t idx = idx_[from];

			if (!is_free_idx(idx)) {
				break;
			}

			from += get_free_skip(idx);
		}

		return from == idx_count ? invalid_idx : from;
	}

	//
	bool is_used(uint32_t i) const {
		return i < idx_.size() && !is_free_idx(idx_[i]);
	}

	//
	class iterator {
	public:
		inline iterator(vector_list<T> *c_, uint32_t i_) : c(c_), i(i_) {}

		inline T &operator*() const {
			return (*c)[i];
		}

		inline T *operator->() const {
			return &(*c)[i];
		}

		inline iterator &operator++() {
			i = c->next(i);
			return *this;
		}
		inline iterator operator++(int) {
			iterator tmp = *this;
			i = c->next(i);
			return tmp;
		}

		inline bool operator==(const iterator &i_) const {
			return i == i_.i;
		}
		inline bool operator!=(const iterator &i_) const {
			return i != i_.i;
		}

		inline uint32_t idx() const {
			return i;
		}

	private:
		vector_list<T> *c;
		uint32_t i;
	};

	iterator begin() {
		return iterator(this, first());
	}

	iterator end() {
		return iterator(this, invalid_idx);
	}

	struct const_iterator {
	public:
		inline const_iterator(const vector_list<T> *c_, uint32_t i_) : c(c_), i(i_) {}

		inline const T &operator*() const {
			return (*c)[i];
		}

		inline const T *operator->() const {
			return &(*c)[i];
		}

		inline bool operator==(const const_iterator &i_) const {
			return i == i_.i;
		}

		inline bool operator!=(const const_iterator &i_) const {
			return i != i_.i;
		}

		inline void operator++() {
			i = c->next(i);
		}

		inline uint32_t idx() const {
			return i;
		}

	private:
		const vector_list<T> *c;
		uint32_t i;
	};

	const_iterator begin() const {
		return const_iterator(this, first());
	}

	const_iterator end() const {
		return const_iterator(this, invalid_idx);
	}

	//
	uint32_t add(const T &v) {
		HG_ASSERT(size_ < 0x00ffffff);
		const size_t capacity_ = capacity();

		if (size_ == capacity_) {
			reserve(capacity_ * 2 + 16);
		}

		const uint32_t i = free_;
		free_ = get_free_idx(idx_[free_]);

		idx_[i] = i;
		new (reinterpret_cast<T *>(storage_) + i) T(v); // emplace_back

		backward_fix_free_skip(i, 0);

		++size_;
		return i;
	}

#if __cpluplus >= 201103L
	uint32_t add(T &&v) {
		HG_ASSERT(size_ < 0x00ffffff);
		const size_t capacity_ = capacity();

		if (size_ == capacity_)
			reserve(capacity_ * 2 + 16);

		const uint32_t i = free_;
		free_ = get_free_idx(idx_[free_]);

		idx_[i] = i;
		new (reinterpret_cast<T *>(storage_) + i) T(std::forward<T>(v)); // emplace_back

		backward_fix_free_skip(i, 0);

		++size_;
		return i;
	}
#endif

	uint32_t remove(uint32_t i) {
		const uint32_t n = next(i); // next entry in use

		{
			const uint32_t idx = idx_[i];
			HG_ASSERT(!is_free_idx(idx)); // assert entry is in use
			reinterpret_cast<T *>(storage_)[idx].~T(); // destroy object
		}

		uint32_t free_skip = 1;
		if ((i + 1) < idx_.size()) {
			const uint32_t idx = idx_[i + 1];

			if (is_free_idx(idx)) {
				const uint32_t next_free_skip = get_free_skip(idx);

				if (next_free_skip < 0x7f) {
					free_skip += next_free_skip;
				}
			}
		}

		idx_[i] = make_free_idx(free_, free_skip); // store the free list node and the number of free indexes to skip while iterating
		free_ = i; // make this index the free list root

		backward_fix_free_skip(i, free_skip); // updating all free skip values leading to this new free entry

		--size_;

		return n;
	}

	//
	void compact() {
		const size_t adjusted_storage_size = get_storage_adjusted_reserve(size_);
		void *new_storage = malloc(sizeof(T) * adjusted_storage_size);

		const size_t idx_count = idx_.size();

		size_t c = 0;
		for (size_t i = 0; i < idx_count;) {
			const uint32_t idx = idx_[i];
			if (!is_free_idx(idx)) {
				idx_[i] = c;
#if __cplusplus >= 201103L
				reinterpret_cast<T *>(new_storage)[c] = std::move(reinterpret_cast<T *>(storage_)[idx]);
#else
				reinterpret_cast<T *>(new_storage)[c] = reinterpret_cast<T *>(storage_)[idx];
#endif
				++c;
				++i;
			} else {
				i += get_free_skip(idx);
			}
		}

		free(storage_);
		storage_capacity_ = adjusted_storage_size;
		storage_ = new_storage;
	}

	// [todo]
#if 0
	void shrink() {
		size_t i;
		for (i = capacity(); i > 0; --i)
			if (!is_free_idx(idx_[i]))
				break;
		// [todo]
	}
#endif

private:
	size_t storage_capacity_; //, storage_size_;
	void *storage_;

#if __cplusplus >= 201103L
	template <typename U = T>
	inline typename std::enable_if<std::is_trivially_copyable<U>::value, void>::type transfer_storage(U *new_storage, const size_t new_storage_size) {
#ifdef __STDC_LIB_EXT1__
		memcpy_s(new_storage, new_storage_size, storage_, sizeof(U) * storage_capacity_);
#else
		memcpy(new_storage, storage_, sizeof(U) * storage_capacity_); // flawfinder: ignore
#endif
	}

	template <typename U = T>
	inline typename std::enable_if<(!std::is_trivially_copyable<U>::value) && std::is_move_constructible<U>::value, void>::type transfer_storage(
		U *new_storage, const size_t new_storage_size) {
		for (uint32_t i = first(); i != invalid_idx; i = next(i)) {
			U *dst = reinterpret_cast<U *>(new_storage) + i;
			U *src = reinterpret_cast<U *>(storage_) + i;
			new (dst) T(std::move(*src));
		}
	}

	template <typename U = T>
	inline typename std::enable_if<(!std::is_trivially_copyable<U>::value) && (!std::is_move_constructible<U>::value), void>::type transfer_storage(
		U *new_storage, const size_t new_storage_size) {
		for (uint32_t i = first(); i != invalid_idx; i = next(i)) {
			U *dst = reinterpret_cast<U *>(new_storage) + i;
			U *src = reinterpret_cast<U *>(storage_) + i;
			new (dst) U(*src);
		}
	}
#else
	template <typename U> inline void transfer_storage(U *new_storage, const size_t new_storage_size) {
		for (uint32_t i = first(); i != invalid_idx; i = next(i)) {
			U *dst = reinterpret_cast<U *>(new_storage) + i;
			U *src = reinterpret_cast<U *>(storage_) + i;
			new (dst) U(*src);
		}
	}
#endif // __cplusplus >= 201103L

	void reserve_storage_(size_t capacity) {
		if (capacity > storage_capacity_) {
			void *new_storage_ = malloc(sizeof(T) * capacity);
			transfer_storage(reinterpret_cast<T *>(new_storage_), sizeof(T) * capacity);
			for (uint32_t i = first(); i != invalid_idx; i = next(i)) {
				reinterpret_cast<T *>(storage_)[i].~T();
			}
			free(storage_);
			storage_ = new_storage_;
			storage_capacity_ = capacity;
		}
	}

	std::vector<uint32_t> idx_;

	size_t size_;
	uint32_t free_;

	static size_t get_storage_adjusted_reserve(size_t size) {
		return size + size / 8;
	}

	static bool is_free_idx(uint32_t idx) {
		return idx & 0x80000000U;
	}

	static uint32_t make_free_idx(uint32_t free_idx, uint32_t free_skip) {
		HG_ASSERT(free_idx < 0x00ffffffU);
		HG_ASSERT(free_skip < 128U);
		return 0x80000000U | ((free_skip & 0x7f) << 24) | (free_idx & 0x00ffffffU);
	}

	static uint32_t get_free_skip(uint32_t idx) {
		return (idx >> 24) & 0x7f;
	}

	static uint32_t get_free_idx(uint32_t idx) {
		return idx & 0x00ffffff;
	}

	void backward_fix_free_skip(uint32_t i, uint32_t free_skip) { // fix free_skip chain leading to this entry (costly backward memory access...)
		// TODO EJ move backward one cache line then process forward
		while (i > 0) {
			--i;
			const uint32_t idx = idx_[i];
			if (!is_free_idx(idx)) {
				break;
			}

			++free_skip;
			if (free_skip > 0x7f) {
				free_skip = 1;
			}

			idx_[i] = make_free_idx(get_free_idx(idx), free_skip);
		}
	}
};

} // namespace hg
