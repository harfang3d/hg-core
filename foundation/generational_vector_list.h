// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#pragma once

#include "foundation/cext.h"
#include "foundation/assert.h"
#include "foundation/vector_list.h"

namespace hg {

struct gen_ref {
	uint32_t idx, gen;
	gen_ref() : idx(0xffffffff), gen(0xffffffff) {}
};

static const gen_ref invalid_gen_ref;

inline bool operator==(gen_ref a, gen_ref b) { return a.idx == b.idx && a.gen == b.gen; }
inline bool operator!=(gen_ref a, gen_ref b) { return a.idx != b.idx || a.gen != b.gen; }

inline bool operator<(gen_ref a, gen_ref b) { return a.gen == b.gen ? a.idx < b.idx : a.gen < b.gen; }

template <typename T> class generational_vector_list : public vector_list<T> {
public:
	gen_ref add_ref(T &v) {
		gen_ref ref;
		ref.idx = vector_list<T>::add(v);
		if (ref.idx >= generations.size())
			generations.resize(size_t(ref.idx) + 64);
		ref.gen = generations[ref.idx];
		return ref;
	}
#if __cpluscplus >= 201103L
	gen_ref add_ref(T &&v) {
		auto idx = vector_list<T>::add(std::forward<T>(v));
		if (idx >= generations.size())
			generations.resize(size_t(idx) + 64);
		return {idx, generations[idx]};
	}
#endif

	void remove_ref(gen_ref ref) {
		if (is_valid(ref)) {
			++generations[ref.idx]; // increase generation for this index
			this->remove(ref.idx);
		}
	}

	gen_ref first_ref() const {
		const uint32_t idx = this->first();
		gen_ref ref = {idx, idx == 0xffffffff ? 0xffffffff : generations[idx]};
		return ref;
	}

	gen_ref next_ref(gen_ref ref) const {
		const uint32_t idx = this->next(ref.idx);
		gen_ref next_ref = {idx, idx == 0xffffffff ? 0xffffffff : generations[idx]};
		return next_ref;
	}

	T &get_safe(gen_ref ref, T &dflt) { return is_valid(ref) ? (*this)[ref.idx] : dflt; }
	const T &get_safe(gen_ref ref, const T &dflt) const { return is_valid(ref) ? (*this)[ref.idx] : dflt; }

	gen_ref get_ref(uint32_t idx) const { 
		if (this->is_used(idx) && idx < generations.size()) {
			gen_ref ref = {idx, generations[idx]};
			return ref;
		}
		return invalid_gen_ref; 
	}

	bool is_valid(gen_ref ref) const { return this->is_used(ref.idx) && ref.idx < generations.size() && ref.gen == generations[ref.idx]; }

	void clear() {
		vector_list<T>::clear();
		generations.clear();
	}

private:
	std::vector<uint32_t> generations;
};

} // namespace hg

namespace std {

#if __cplusplus < 200103L
template <typename T> struct hash {
	size_t operator()(const T &) { return 0; } // [todo] ?
};
#endif

template <> struct hash<hg::gen_ref> {
	size_t operator()(const hg::gen_ref &ref) const {
		// FNV
		uint32_t h = ref.idx, x = ref.gen;
		for (int i = 0; i < 4; ++i) {
			h ^= x & 255;
			x >>= 8;
			h = (h << 24) + h * 0x193;
		}
		return h;
	}
};
} // namespace std
