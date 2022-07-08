// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#pragma once

#include "foundation/cext.h"
#include "foundation/matrix4.h"
#include "foundation/plane.h"
#include <stdint.h>

namespace hg {

struct Mat44;

enum FrustumPlane { FP_Top, FP_Bottom, FP_Left, FP_Right, FP_Near, FP_Far, FP_Count };

typedef std::array<Plane, FP_Count> Frustum; // 96B

Frustum MakeFrustum(const Mat44 &projection);
Frustum MakeFrustum(const Mat44 &projection, const Mat4 &mtx);

Frustum TransformFrustum(const Frustum &frustum, const Mat4 &mtx);

//
struct MinMax;

enum Visibility { V_Outside, V_Inside, V_Clipped };

/// Return the visibility of a set of vector.
Visibility TestVisibility(const Frustum &frustum, uint32_t count, const Vec3 *points, float distance = 0.f);
/// Return the visibility of a sphere.
Visibility TestVisibility(const Frustum &frustum, const Vec3 &origin, float radius);
/// Return the visibility of a minmax.
Visibility TestVisibility(const Frustum &frustum, const MinMax &minmax);

} // namespace hg
