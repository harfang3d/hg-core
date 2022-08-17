// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "foundation/frustum.h"
#include "foundation/matrix4.h"
#include "foundation/matrix44.h"
#include "foundation/minmax.h"
#include "foundation/vector4.h"

#include <math.h>

namespace hg {

static inline Plane NormalizePlane(const Plane &p) {
	const float l = sqrtf(p.x * p.x + p.y * p.y + p.z * p.z);
	return p / l;
}

Frustum MakeFrustum(const Mat44 &projection) {
	const Vec4 X = GetRow(projection, 0), Y = GetRow(projection, 1), Z = GetRow(projection, 2), W = GetRow(projection, 3);

	Frustum out;
	out[FP_Top] = Reverse(NormalizePlane(W + Y));
	out[FP_Bottom] = Reverse(NormalizePlane(W - Y));
	out[FP_Left] = Reverse(NormalizePlane(W + X));
	out[FP_Right] = Reverse(NormalizePlane(W - X));
	out[FP_Near] = Reverse(NormalizePlane(W + Z));
	out[FP_Far] = Reverse(NormalizePlane(W - Z));
	return out;
}

Frustum MakeFrustum(const Mat44 &projection, const Mat4 &m) {
	return TransformFrustum(MakeFrustum(projection), m);
}

Frustum TransformFrustum(const Frustum &frustum, const Mat4 &m) {
	const Mat44 iMt = Transpose(Mat44(InverseFast(m)));

	Frustum out;
	for (size_t i = 0; i < FP_Count; i++) {
		out[i] = iMt * frustum[i];
	}
	return out;
}

//
Visibility TestVisibility(const Frustum &planes, uint32_t count, const Vec3 point[], float distance) {
	Visibility vis = V_Inside;

	for (uint32_t n = 0; n < FP_Count; ++n) {
		uint32_t out = 0;
		for (uint32_t i = 0; i < count; ++i) {
			if (DistanceToPlane(planes[n], point[i]) > distance) {
				++out;
			}
		}

		if (out == count) {
			vis = V_Outside;
			break;
		}

		if (out > 0) {
			vis = V_Clipped;
		}
	}

	return vis;
}

Visibility TestVisibility(const Frustum &planes, const Vec3 &point, float radius) {
	Visibility vis = V_Inside;

	for (uint32_t n = 0; n < FP_Count; ++n) {
		const float d = DistanceToPlane(planes[n], point);

		if (d > radius) {
			vis = V_Outside;
			break;
		}

		if (d > -radius) {
			vis = V_Clipped;
		}
	}

	return vis;
}

Visibility TestVisibility(const Frustum &planes, const MinMax &minmax) {
	const Vec3 center_x2 = minmax.mn + minmax.mx;
	const Vec3 extend_x2 = minmax.mx - minmax.mn;

	Visibility vis = V_Inside;

	for (uint32_t n = 0; n < FP_Count; ++n) {
		// complete demonstration at: https://fgiesen.wordpress.com/2010/10/17/view-frustum-culling/
		const Plane &plane = planes[n];

		const float d = Dot(Vec3(plane.x, plane.y, plane.z), center_x2);
		const float r = Dot(Abs(Vec3(plane.x, plane.y, plane.z)), extend_x2); // "where there's one, there's many" would take care of that Abs()

		const float plane_d_x2 = -plane.w * 2.F;

		if (d - r > plane_d_x2) {
			vis = V_Outside;
			break;
		}

		if (d + r > plane_d_x2) {
			vis = V_Clipped;
		}
	}

	return vis;
}

} // namespace hg
