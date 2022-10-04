// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "foundation/vector2.h"
#include "foundation/math.h"
#include "foundation/matrix3.h"
#include "foundation/vector3.h"
#include "foundation/vector4.h"

namespace hg {

template <> tVec2<float>::tVec2(const Vec3 &v) {
	x = v.x;
	y = v.y;
}

template <> tVec2<float>::tVec2(const Vec4 &v) {
	x = v.x;
	y = v.y;
}

template <> tVec2<int>::tVec2(const Vec3 &v) {
	x = static_cast<int>(v.x);
	y = static_cast<int>(v.y);
}

template <> tVec2<int>::tVec2(const Vec4 &v) {
	x = static_cast<int>(v.x);
	y = static_cast<int>(v.y);
}

template <> bool operator==(const tVec2<float> &a, const tVec2<float> &b) {
	const float epsilon = std::numeric_limits<float>::epsilon();
	return Abs(b.x - a.x) < epsilon && Abs(b.y - a.y) < epsilon;
}

template <> float Len(const tVec2<float> &v) {
	return Sqrt(Len2(v));
}

template <> int Len(const tVec2<int> &v) {
	return static_cast<int>(Sqrt(static_cast<float>(Len2(v))));
}

template <> float Dist(const tVec2<float> &a, const tVec2<float> &b) {
	return Sqrt(Dist2(a, b));
}

template <> int Dist(const tVec2<int> &a, const tVec2<int> &b) {
	return static_cast<int>(Sqrt(static_cast<float>(Dist2(a, b))));
}

} // namespace hg
