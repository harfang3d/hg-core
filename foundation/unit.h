// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#pragma once

#include <iomanip>
#include <sstream>
#include <string>

#include "foundation/time.h"

namespace hg {

/// Convert an angle in degrees to the engine unit system.
template <typename T> constexpr T Deg(T v) {
	return v / static_cast<T>(180) * static_cast<T>(3.1415926535);
}

/// Convert an angle in radians to the engine unit system.
template <typename T> constexpr T Rad(T v) {
	return v;
}

/// Convert an angle in degrees to radians.
template <typename T> constexpr T DegreeToRadian(T v) {
	return v * (3.1415926535F / 180.F);
}

/// Convert an angle in radians to degrees.
template <typename T> constexpr T RadianToDegree(T v) {
	return v * (180.F / 3.1415926535F);
}

template <typename T> constexpr T Sec(T v) {
	return v;
}

template <typename T> constexpr T Csec(T v) {
	return v * static_cast<T>(0.01);
}

template <typename T> constexpr T Ms(T v) {
	return v * static_cast<T>(0.001);
}

template <typename T> constexpr T Ton(T v) {
	return v * static_cast<T>(1000);
}

template <typename T> constexpr T Kg(T v) {
	return v;
}

template <typename T> constexpr T G(T v) {
	return v * static_cast<T>(0.001);
}

template <typename T> constexpr T Km(T v) {
	return v * static_cast<T>(1000);
}

template <typename T> constexpr T Mtr(T v) {
	return v;
}

template <typename T> constexpr T Cm(T v) {
	return v * static_cast<T>(0.01);
}

template <typename T> constexpr T Mm(T v) {
	return v * static_cast<T>(0.001);
}

template <typename T> constexpr T Inch(T v) {
	return v * static_cast<T>(0.0254);
}

inline constexpr size_t KB(const size_t size) {
	return size * 1024;
}

inline constexpr size_t MB(const size_t size) {
	return size * 1024 * 1024;
}

template <typename T> std::string FormatMemorySize(T v_) {
	std::ostringstream str;

	int64_t v = static_cast<int64_t>(v_);

	if (v < 0) {
		str << "-";
		v = -v;
	}

	if (v < 1024LL) {
		str << std::fixed << std::setprecision(0) << v << "B";
	} else if (v < 1024LL * 1024LL) {
		str << std::fixed << std::setfill('0') << std::setprecision(1) << static_cast<float>((v * 10LL) / (1024LL)) / 10.F << "KB";
	} else if (v < 1024LL * 1024LL * 1024LL) {
		str << std::fixed << std::setfill('0') << std::setprecision(1) << static_cast<float>((v * 10LL) / (1024LL * 1024LL)) / 10.F << "MB";
	} else if (v < 1024LL * 1024LL * 1024LL * 1024LL) {
		str << std::fixed << std::setfill('0') << std::setprecision(1) << static_cast<float>((v * 10LL) / (1024LL * 1024LL * 1024LL)) / 10.F << "GB";
	} else {
		str << std::fixed << std::setfill('0') << std::setprecision(1) << static_cast<float>((v * 10LL) / (1024LL * 1024LL * 1024LL * 1024LL)) / 10.F << "TB";
	}

	return str.str();
}

template <typename T> std::string FormatCount(T v_) {
	std::ostringstream str;

	int64_t v = static_cast<int64_t>(v_);

	if (v < 0) {
		str << "-";
		v = -v;
	}

	if (v < 1000LL) {
		str << v;
	} else if (v < 1000LL * 1000LL) {
		str << std::fixed << std::setfill('0') << std::setprecision(1) << static_cast<float>((v * 10LL) / (1000LL)) / 10.F << "K";
	} else if (v < 1000LL * 1000LL * 1000LL) {
		str << std::fixed << std::setfill('0') << std::setprecision(1) << static_cast<float>((v * 10LL) / (1000LL * 1000LL)) / 10.F << "M";
	} else {
		str << std::fixed << std::setfill('0') << std::setprecision(1) << static_cast<float>((v * 10LL) / (1000LL * 1000LL * 1000LL)) / 10.F << "G";
	}

	return str.str();
}

template <typename T> std::string FormatDistance(T v_) {
	std::ostringstream str;

	float v = static_cast<float>(v_);

	if (v < 0.F) {
		str << "-";
		v = -v;
	}

	if (v < 0.1F) {
		str << std::fixed << std::setfill('0') << std::setprecision(1) << static_cast<float>(v) * 1000.F << "mm";
	} else if (v < 1.F) {
		str << std::fixed << std::setfill('0') << std::setprecision(1) << static_cast<float>(v) * 100.F << "cm";
	} else if (v >= 1000.F) {
		str << std::fixed << std::setfill('0') << std::setprecision(1) << static_cast<float>(v) / 1000.F << "km";
	} else {
		str << std::fixed << std::setfill('0') << std::setprecision(1) << static_cast<float>(v) << "m";
	}

	return str.str();
}

std::string FormatTime(time_ns t);

} // namespace hg
