// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#pragma once

#include "foundation/cext.h"
#include "foundation/generational_vector_list.h"

#include <map>
#include <string>

namespace hg {

template <typename T> struct ResourceRef {
	gen_ref ref;
	bool operator==(const ResourceRef &o) const { return ref == o.ref; }
	bool operator!=(const ResourceRef &o) const { return ref != o.ref; }
};

template <typename T> class ResourceCache {
public:
	typedef ResourceRef<T> RefType;

private:
	struct name_T {
		std::string name;
		T T_;
	};

	typedef typename std::map<const std::string, RefType>::iterator dict_iterator;
	typedef typename std::map<const std::string, RefType>::const_iterator const_dict_iterator;

	typedef typename generational_vector_list<name_T>::iterator res_iterator;
	typedef typename generational_vector_list<name_T>::const_iterator const_res_iterator;

	inline RefType add_ref(const std::string &name, const T &res) { 
		name_T in = {name, res};
		RefType out;
		out.ref = resources.add_ref(in);
		return out;
	}

public:
	ResourceCache(void (*destroy)(T &)) : _destroy(destroy) {}

	RefType Add(const std::string &name, const T &res) {
		dict_iterator i = name_to_ref.find(name);
		if (i != name_to_ref.end())
			return i->second;

		name_T nt;
		nt.name = name;
		nt.T_ = res;

		RefType ref;
		ref.ref = resources.add_ref(nt);
		name_to_ref[name] = ref;
		return ref;
	}

	void Update(RefType ref, const T &res) {
		if (resources.is_valid(ref.ref)) {
			_destroy(resources[ref.ref.idx].T_);
			resources[ref.ref.idx].T_ = res;
		}
	}

	void Destroy(RefType ref) {
		if (resources.is_valid(ref.ref)) {
			_destroy(resources[ref.ref.idx].T_);
			name_to_ref.erase(resources[ref.ref.idx].name); // drop from cache
			resources.remove_ref(ref.ref);
		}
	}

	void DestroyAll() {
		for (res_iterator i = resources.begin(); i < resources.end(); i++) {
			_destroy(i.T_);
		}
		resources.clear();
		name_to_ref.clear();
	}

	bool IsValidRef(RefType ref) const { return resources.is_valid(ref.ref); }

	// get a resource index for code that does not carry a full reference to the resource.
	uint16_t GetValidatedRefIndex(RefType ref) const { return resources.is_valid(ref.ref) ? numeric_cast<uint16_t>(ref.ref.idx) : 0xffff; }

	RefType Has(const std::string &name) const {
		const_dict_iterator i = name_to_ref.find(name);
		return i != name_to_ref.end() ? i->second : RefType();
	}

	size_t GetCount() const { return resources.size(); }

	void SetName(RefType ref, const std::string &name) {
		if (resources.is_valid(ref.ref))
			resources[ref.ref.idx].name = name;
	}

	void SetName_unsafe_(uint16_t idx, const std::string &name) {
		if (idx != 0xffff) {
			__ASSERT__(resources.is_used(idx));
			resources[idx].name = name;
		}
	}

	std::string GetName(RefType ref) const { return resources.is_valid(ref.ref) ? resources[ref.ref.idx].name : std::string(); }

	std::string GetName_unsafe_(uint16_t idx) const {
		if (idx != 0xffff) {
			__ASSERT__(resources.is_used(idx));
			return resources[idx].name;
		}
		return std::string();
	}

	const T &Get(RefType ref) const { return resources.is_valid(ref.ref) ? resources[ref.ref.idx].T_ : dflt; }

	const T &Get_unsafe_(uint16_t idx) const {
		if (idx != 0xffff) {
			__ASSERT__(resources.is_used(idx));
			return resources[idx].T_;
		}
		return dflt;
	}

	const T &Get(const std::string &name) const {
		const_dict_iterator i = name_to_ref.find(name);
		return i != name_to_ref.end() ? resources[i->second.ref.idx].T_ : dflt;
	}

	T &Get(RefType ref) {
		__ASSERT__(IsValidRef(ref));
		return resources[ref.ref.idx].T_;
	}

	gen_ref first_ref() const { return resources.first_ref(); }
	gen_ref next_ref(gen_ref ref) const { return resources.next_ref(ref); }

#if 0

#if __cplusplus >= 201103L
	R Add(const std::string &name, T &&res) {
		auto i = name_to_ref.find(name);
		if (i != name_to_ref.end())
			return i->second;

		R ref = {resources.add_ref({name, std::move(res)})};
		name_to_ref[name] = ref;
		return ref;
	}

	void Update(R ref, T &&res) {
		if (resources.is_valid(ref.ref)) {
			_destroy(resources[ref.ref.idx].T_);
			resources[ref.ref.idx].T_ = std::move(res);
		}
	}
#endif

#endif

private:
	T dflt;

	generational_vector_list<name_T> resources;
	std::map<const std::string, RefType> name_to_ref;

	void (*_destroy)(T &);
};

} // namespace hg
