// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#pragma once

#include "foundation/cext.h"
#include "foundation/color.h"
#include "foundation/vector2.h"
#include "foundation/vector3.h"

namespace hg {

struct Vertex {
	enum { UVCount = 8, ColorCount = 4, BoneCount = 4 };

	Vec3 pos;

	Vec3 normal;
	Vec3 tangent;
	Vec3 binormal;

	Vec2 uv[UVCount];
	Color color[ColorCount];

	uint8_t index[BoneCount];
	float weight[BoneCount];
};

Vertex MakeVertex(const Vec3 &pos, const Vec3 &nrm = Vec3::Up, const Vec2 &uv = Vec2::Zero, const Color &color = Color::One);

} // namespace hg
