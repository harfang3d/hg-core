// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "foundation/clock.h"

namespace hg {

static time_ns last = 0;
static time_ns dt = 1;
static time_ns clock = 0;

static int clock_scale = 1024;

void reset_clock() {
	last = time_now();
}

time_ns tick_clock() {
	const time_ns now = time_now();
	dt = last ? ((now - last) * clock_scale) >> 10 : 1;
	last = now;
	clock += dt;
	return dt;
}

time_ns get_clock() {
	return clock;
}

time_ns get_clock_dt() {
	return dt;
}

void skip_clock() {
	reset_clock();
}

void set_clock_scale(float scale) {
	clock_scale = static_cast<int>(scale * 1024.F);
}

float get_clock_scale() {
	return static_cast<float>(clock_scale) / 1024.F;
}

} // namespace hg
