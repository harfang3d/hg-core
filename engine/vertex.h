// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#pragma once

#include "foundation/color.h"
#include "foundation/vector2.h"
#include "foundation/vector3.h"

namespace hg {

struct Vertex {
	Vec3 pos;

	Vec3 normal;
	Vec3 tangent;
	Vec3 binormal;

	Vec2 uv0, uv1, uv2, uv3, uv4, uv5, uv6, uv7;
	Color color0, color1, color2, color3;

	uint8_t index[4];
	float weight[4];
};

Vertex MakeVertex(const Vec3 &pos, const Vec3 &nrm = Vec3::Up, const Vec2 &uv = Vec2::Zero, const Color &color = Color::One);

} // namespace hg
