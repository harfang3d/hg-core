// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#pragma once

#include <limits>

#include "foundation/cext.h"
#include "foundation/math.h"

namespace hg {

template <typename T> inline T pack_float(float v, float scale = 1.F) {
	return static_cast<T>(Clamp(v / scale, (std::is_signed<T>::value) ? -1.F : 0.F, 1.F) * std::numeric_limits<T>::max());
}

template <typename T> inline float unpack_float(T v, float scale = 1.F) {
	return (static_cast<float>(v) / std::numeric_limits<T>::max()) * scale;
}

//
template <> inline float pack_float<float>(float v, float) {
	return v;
}

template <> inline float unpack_float<float>(float v, float) {
	return v;
}

} // namespace hg
