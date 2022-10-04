// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#pragma once

#include <stdint.h>

#include "foundation/cext.h"

namespace hg {

typedef int64_t time_ns;

extern time_ns time_undefined;

//
inline constexpr float time_to_sec_f(time_ns t) {
	return static_cast<float>(static_cast<double>(t) / 1000000000.0);
}

inline constexpr float time_to_ms_f(time_ns t) {
	return static_cast<float>(static_cast<double>(t) / 1000000.0);
}

inline constexpr float time_to_us_f(time_ns t) {
	return static_cast<float>(static_cast<double>(t) / 1000.0);
}

//
inline constexpr int64_t time_to_day(time_ns t) {
	return t / (1000000000LL * 60LL * 60LL * 24LL);
}

inline constexpr int64_t time_to_hour(time_ns t) {
	return t / (1000000000LL * 60LL * 60LL);
}

inline constexpr int64_t time_to_min(time_ns t) {
	return t / (1000000000LL * 60LL);
}

inline constexpr int64_t time_to_sec(time_ns t) {
	return t / 1000000000LL;
}

inline constexpr int64_t time_to_ms(time_ns t) {
	return t / 1000000LL;
}

inline constexpr int64_t time_to_us(time_ns t) {
	return t / 1000LL;
}

inline constexpr int64_t time_to_ns(time_ns t) {
	return t;
}

//
inline constexpr time_ns time_from_sec_d(double sec) {
	return static_cast<time_ns>(static_cast<double>(sec) * 1000000000.0);
}

inline constexpr time_ns time_from_ms_d(double ms) {
	return static_cast<time_ns>(static_cast<double>(ms) * 1000000.0);
}

inline constexpr time_ns time_from_us_d(double us) {
	return static_cast<time_ns>(static_cast<double>(us) * 1000.0);
}

//
inline constexpr time_ns time_from_sec_f(float sec) {
	return static_cast<time_ns>(static_cast<double>(sec) * 1000000000.0);
}

inline constexpr time_ns time_from_ms_f(float ms) {
	return static_cast<time_ns>(static_cast<double>(ms) * 1000000.0);
}

inline constexpr time_ns time_from_us_f(float us) {
	return static_cast<time_ns>(static_cast<double>(us) * 1000.0);
}

//
inline constexpr time_ns time_from_day(int64_t day) {
	return day * (1000000000LL * 60LL * 60LL * 24LL);
}

inline constexpr time_ns time_from_hour(int64_t hour) {
	return hour * (1000000000LL * 60LL * 60LL);
}

inline constexpr time_ns time_from_min(int64_t min) {
	return min * (1000000000LL * 60LL);
}

inline constexpr time_ns time_from_sec(int64_t sec) {
	return sec * 1000000000LL;
}

inline constexpr time_ns time_from_ms(int64_t ms) {
	return ms * 1000000LL;
}

inline constexpr time_ns time_from_us(int64_t us) {
	return us * 1000LL;
}

inline constexpr time_ns time_from_ns(int64_t ns) {
	return ns;
}

//
time_ns time_now();
time_ns wall_clock();

void sleep_for(const time_ns &t);

} // namespace hg
