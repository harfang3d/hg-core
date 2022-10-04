// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "foundation/plane.h"
#include "foundation/matrix4.h"
#include "foundation/vector3.h"

namespace hg {

Plane MakePlane(const Vec3 &p, const Vec3 &n) {
	return Plane(n.x, n.y, n.z, -Dot(p, n));
}

Plane MakePlane(const Vec3 &p, const Vec3 &n, const Mat4 &m) {
	Vec3 tp = m * p;
	Vec3 tn = Vec3(m * Vec4(n, 0.F));
	return Plane(tn.x, tn.y, tn.z, -Dot(tp, tn));
}

float DistanceToPlane(const Plane &plane, const Vec3 &p) {
	return Dot(p, Vec3(plane.x, plane.y, plane.z)) + plane.w;
}

} // namespace hg
