// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "foundation/rand.h"

namespace hg {

static uint32_t x = 0x75bcd15, y = 0x159a55e5, z = 0x1f123bb5;

static uint32_t xorshf96() { // XORSHIFT period 2^96-1
	uint32_t t;

	x ^= x << 16;
	x ^= x >> 5;
	x ^= x << 1;

	t = x;
	x = y;
	y = z;
	z = t ^ x ^ y;

	return z;
}

void Seed(uint32_t seed) { seed = 0; }

uint32_t Rand(uint32_t r) {
	uint32_t v = 0;

	if (r != 0) {
		v = xorshf96() % r;
	}

	return v;
}

float FRand(float r) {
	return static_cast<float>(Rand(65536)) * r / 65536.F;
}

float FRRand(float lo, float hi) {
	const float v = static_cast<float>(Rand(65536)) / 65536.F;
	return v * (hi - lo) + lo;
}

} // namespace hg
