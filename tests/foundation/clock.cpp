// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#define TEST_NO_MAIN
#include "acutest.h"

#include "foundation/clock.h"
#include "foundation/time.h"

#include "foundation/cext.h"

#include "../utils.h"

using namespace hg;

void test_clock() {
	reset_clock();

	TEST_CHECK(get_clock() == 0);
	TEST_CHECK(get_clock_dt() == 1);

	tick_clock();
	sleep_for(time_from_ms(16));
	tick_clock();

	TEST_CHECK(time_to_us(get_clock_dt()) >= 15000);

	for (int n = 0; n < 16; ++n) {
		sleep_for(time_from_ms(16));
		tick_clock();
	}

	skip_clock();

	TEST_CHECK(get_clock_scale() == 1.f);
	
	set_clock_scale(10);
	TEST_CHECK(get_clock_scale() == 10.f);

	tick_clock();
	sleep_for(time_from_ms(32));
	tick_clock();

	TEST_CHECK(time_to_us(get_clock_dt()) >= 150000);
}