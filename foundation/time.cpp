// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "foundation/time.h"

#include <limits>
#include <climits>

#if _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

namespace hg {

time_ns time_undefined = LONG_MIN;

#if _WIN32
time_ns time_now() { 
	LARGE_INTEGER counter, frequency;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&counter);
	int64_t s  = counter.QuadPart / frequency.QuadPart;
	int64_t ns = 1000000000 * (counter.QuadPart - s * frequency.QuadPart) / frequency.QuadPart;
	return s * 1000000000 + ns;
}
time_ns wall_clock() { return time_now(); }
#else
time_ns time_now() {
	struct timespec tv;
	clock_gettime(CLOCK_MONOTONIC, &tv);
	return tv.tv_sec * 1000000000 + tv.tv_nsec;
}
time_ns wall_clock() { 
	struct timespec tv; 
#	if defined(_gnu_linux_) || defined(__linux__)
	clock_gettime(CLOCK_TAI, &tv);
#	else
	clock_gettime(CLOCK_REALTIME, &tv);
#	endif
	return tv.tv_sec * 1000000000 + tv.tv_nsec;
}
#endif

} // namespace hg
