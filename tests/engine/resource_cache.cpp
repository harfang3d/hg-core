// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#define TEST_NO_MAIN
#include "acutest.h"

#include "engine/resource_cache.h"

using namespace hg;

struct Dummy {
	int v;

	Dummy() : v(0xcafe) {}
	explicit Dummy(int w) : v(w) {}
	Dummy(const Dummy &d) : v(d.v) {}
	Dummy &operator=(const Dummy &d) {
		v = d.v;
		return *this;
	}
	
	friend bool operator==(const Dummy &d0, const Dummy &d1) { return d0.v == d1.v; }
	friend bool operator!=(const Dummy &d0, const Dummy &d1) { return d0.v != d1.v; }

	bool IsValid() const { return v != 0xcafe; }
};

static void Destroy(Dummy& d) {
	// [todo]
}

typedef ResourceRef<Dummy> DummyRef;

void test_resource_cache() {
	{
		ResourceRef<int> r0;
		ResourceRef<int> r1;
		ResourceRef<int> r2;

		r0.ref.idx = 1, r0.ref.gen = 0xc07;
		r1.ref.idx = 1, r1.ref.gen = 0xa07;
		r2.ref.idx = 0, r2.ref.gen = 0xc07;

		TEST_CHECK((r0 == r1) == false);
		TEST_CHECK((r0 == r2) == false);
		TEST_CHECK((r2 == r1) == false);
		TEST_CHECK((r0 != r1) == true);
		TEST_CHECK((r0 != r2) == true);
		TEST_CHECK((r2 != r1) == true);
	}

	ResourceCache<Dummy> cache(Destroy);
	TEST_CHECK(cache.Has("item #00").ref == invalid_gen_ref);
	TEST_CHECK(cache.Get("item #00").IsValid() == false);
	TEST_CHECK(cache.GetCount() == 0);

	Dummy d0(1);
	
	DummyRef ref0 = cache.Add("item #00", d0);
	TEST_CHECK(ref0.ref != invalid_gen_ref);
	TEST_CHECK(cache.Has("item #00").ref == ref0.ref);
	TEST_CHECK(cache.Get(ref0) == d0);
	TEST_CHECK(cache.GetName(ref0) == "item #00");
	TEST_CHECK(cache.Get("item #00") == d0);
	TEST_CHECK(cache.GetCount() == 1);
	/*
	RefType Add(const std::string &name, const T &res);
	void Update(RefType ref, const T &res);
	void Destroy(RefType ref);
	void DestroyAll();
	bool IsValidRef(RefType ref);
	uint16_t GetValidatedRefIndex(RefType ref) const;
	RefType Has(const std::string &name) const;
	size_t GetCount() const;
	void SetName(RefType ref, const std::string &name);
	void SetName_unsafe_(uint16_t idx, const std::string &name);
	std::string GetName(RefType ref) const;
	std::string GetName_unsafe_(uint16_t idx) const;
	const T &Get(RefType ref) const;
	const T &Get_unsafe_(uint16_t idx) const;
	const T &Get(const std::string &name) const;
	T &Get(RefType ref);
	gen_ref first_ref() const;
	gen_ref next_ref(gen_ref ref) const;
*/
}