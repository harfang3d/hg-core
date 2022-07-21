// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "foundation/time.h"

#include <climits>
#include <limits>

#if _WIN32
#include <windows.h>
#else
#include <errno.h>
#include <time.h>
#endif

namespace hg {

time_ns time_undefined = LONG_MIN;

#if _WIN32
time_ns time_now() {
	LARGE_INTEGER counter, frequency;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&counter);
	int64_t s = counter.QuadPart / frequency.QuadPart;
	int64_t ns = 1000000000LL * (counter.QuadPart - s * frequency.QuadPart) / frequency.QuadPart;
	return s * 1000000000LL + ns;
}

time_ns wall_clock() { return time_now(); }

void sleep_for(const time_ns &t) {
	int64_t ms = time_to_ms(t);
	int64_t remainder = t - time_from_ms(ms);

	if (ms) {
		Sleep(ms);
	}
	if (remainder) {
		LARGE_INTEGER start, frequency;
		if (QueryPerformanceFrequency(&frequency) && QueryPerformanceCounter(&start)) {
			LONGLONG deadline = start.QuadPart + (remainder * frequency.QuadPart) / 1000000000LL;
			LARGE_INTEGER current;
			do {
				if (!QueryPerformanceCounter(&current)) {
					break;
				}
			} while (current.QuadPart < deadline);
		}
	}
}

#else
time_ns time_now() {
	struct timespec tv;
	clock_gettime(CLOCK_MONOTONIC, &tv);
	int64_t seconds = tv.tv_sec * 1000000000LL;
	return seconds + tv.tv_nsec;
}

time_ns wall_clock() {
	struct timespec tv;
#if defined(_gnu_linux_) || defined(__linux__)
	clock_gettime(CLOCK_TAI, &tv);
#else
	clock_gettime(CLOCK_REALTIME, &tv);
#endif
	int64_t seconds = tv.tv_sec * 1000000000LL;
	return seconds + tv.tv_nsec;
}

void sleep_for(const time_ns &t) {
	if (t <= 0) {
		return;
	}
	int64_t seconds = time_to_sec(t);

	struct timespec ts;
	ts.tv_sec = seconds;
	ts.tv_nsec = t - time_from_sec(seconds);

	while ((nanosleep(&ts, &ts) == -1) && (errno == EINTR)) {
		// sleep was interrupted by a signal.
	}
}
#endif

} // namespace hg
