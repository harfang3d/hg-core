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
	Dummy d1(2);
	Dummy d2(3);
	
	DummyRef ref0 = cache.Add("item #00", d0);
	TEST_CHECK(ref0.ref != invalid_gen_ref);
	TEST_CHECK(cache.IsValidRef(ref0) == true);
	TEST_CHECK(cache.Has("item #00").ref == ref0.ref);
	TEST_CHECK(cache.Get(ref0) == d0);
	TEST_CHECK(cache.GetName(ref0) == "item #00");
	TEST_CHECK(cache.Get("item #00") == d0);
	TEST_CHECK(cache.GetCount() == 1);

	DummyRef ref1 = cache.Add("item #01", d1);
	TEST_CHECK(cache.IsValidRef(ref1) == true);
	TEST_CHECK(cache.Has("item #01").ref == ref1.ref);
	TEST_CHECK(cache.GetName(ref1) == "item #01");
	TEST_CHECK(cache.Get(ref1) == d1);
	TEST_CHECK(cache.Get("item #01") == d1);
	TEST_CHECK(cache.GetCount() == 2);

	DummyRef tmp_ref = cache.Add("item #00", d1);
	TEST_CHECK(tmp_ref == ref0);
	TEST_CHECK(cache.Get(tmp_ref) == d0);

	DummyRef ref2 = cache.Add("item #02", d2);
	TEST_CHECK(cache.IsValidRef(ref2) == true);
	TEST_CHECK(cache.Get("item #02") == d2);
	TEST_CHECK(cache.GetCount() == 3);

	cache.SetName(ref0, "Item #0000");
	TEST_CHECK(cache.GetName(ref0) == "Item #0000");

	uint16_t index = cache.GetValidatedRefIndex(ref0);
	TEST_CHECK(cache.GetName_unsafe_(index) == "Item #0000");
	TEST_CHECK(cache.GetName_unsafe_(0xffff).empty());
	TEST_CHECK(cache.Get_unsafe_(index) == d0);

	cache.SetName_unsafe_(index, "item #0");
	TEST_CHECK(cache.GetName_unsafe_(index) == "item #0");

	cache.SetName_unsafe_(0xffff, "invalid item");
	TEST_CHECK(cache.GetName_unsafe_(0xffff).empty() == true);
	TEST_CHECK(cache.Get_unsafe_(0xffff).IsValid() == false);

	cache.Destroy(ref1);
	TEST_CHECK(cache.GetCount() == 2);
	TEST_CHECK(cache.IsValidRef(ref1) == false);
	TEST_CHECK(cache.IsValidRef(cache.Has("item #01")) == false);

	Dummy d3(4);
	DummyRef ref3 = cache.Add("item #03", d3);
	TEST_CHECK(cache.GetCount() == 3);

	cache.Update(ref0, d1);
	TEST_CHECK(cache.Get("item #00") != d0);
	TEST_CHECK(cache.Get(ref0) == d1);

	int count[5];
	std::fill_n(count, 5, 0);
	for (gen_ref ref = cache.first_ref(); ref != invalid_gen_ref; ref = cache.next_ref(ref)) {
		const Dummy &e = cache.Get_unsafe_(ref.idx);
		if(TEST_CHECK(e.IsValid())) {
			count[e.v]++;
		}
	}
	TEST_CHECK(count[0] == 0);
	TEST_CHECK(count[1] == 0);
	TEST_CHECK(count[2] == 1);
	TEST_CHECK(count[3] == 1);
	TEST_CHECK(count[4] == 1);
	
	cache.DestroyAll();
	TEST_CHECK(cache.GetCount() == 0);
	TEST_CHECK(cache.Get(ref2).IsValid() == false);
}