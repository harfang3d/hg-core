// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "foundation/obb.h"
#include "foundation/matrix3.h"
#include "foundation/matrix4.h"
#include "foundation/minmax.h"

namespace hg {

OBB OBBFromMinMax(const MinMax &minmax) {
	return OBB((minmax.mn + minmax.mx) * 0.5F, minmax.mx - minmax.mn);
}

MinMax MinMaxFromOBB(const OBB &obb) {
	Vec3 xtd = obb.scl * 0.5F;
	const Vec3 smt[4] = {Vec3(xtd.x, xtd.y, xtd.z), Vec3(-xtd.x, xtd.y, xtd.z), Vec3(xtd.x, -xtd.y, xtd.z), Vec3(xtd.x, xtd.y, -xtd.z)};

	MinMax mm;
	mm.mx = Abs(obb.rot * smt[0]);
	for (int n = 1; n < 4; ++n) {
		mm.mx = Max(mm.mx, Abs(obb.rot * smt[n]));
	}
	mm.mn = Reverse(mm.mx);

	mm.mn += obb.pos;
	mm.mx += obb.pos;
	return mm;
}

OBB TransformOBB(const OBB &obb, const Mat4 &m) {
	Vec3 pos, scl;
	Mat3 rot;
	Decompose(m, &pos, &rot, &scl);
	return OBB(rot * obb.pos + pos, scl * obb.scl, rot * obb.rot);
}

} // namespace hg
