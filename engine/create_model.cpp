// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "foundation/math.h"

#include "engine/create_model.h"
#include "engine/model_builder.h"

namespace hg {

static inline VtxIdxType AddVertex(ModelBuilder &builder, const Vec3 &p, const Vec3 &n, const Vec2 &uv = Vec2::Zero) {
	Vertex v;
	v.pos = p;
	v.normal = n;
	v.uv0 = uv;
	return builder.AddVertex(v);
}

Model CreateCubeModel(const VertexLayout &layout, float x, float y, float z) {
	ModelBuilder builder;

	x /= 2.f;
	y /= 2.f;
	z /= 2.f;

	VtxIdxType a, b, c, d;
	Vertex v;

	a = AddVertex(builder, Vec3(-x, -y, -z), Vec3(0.f, 0.f, -1.f), Vec2(0, 0)); // -Z
	b = AddVertex(builder, Vec3(-x, y, -z), Vec3(0.f, 0.f, -1.f), Vec2(0, 1));
	c = AddVertex(builder, Vec3(x, y, -z), Vec3(0.f, 0.f, -1.f), Vec2(1, 1));
	d = AddVertex(builder, Vec3(x, -y, -z), Vec3(0.f, 0.f, -1.f), Vec2(1, 0));
	builder.AddQuad(d, c, b, a);

	a = AddVertex(builder, Vec3(-x, -y, z), Vec3(0.f, 0.f, 1.f), Vec2(0, 0)); // +Z
	b = AddVertex(builder, Vec3(-x, y, z), Vec3(0.f, 0.f, 1.f), Vec2(0, 1));
	c = AddVertex(builder, Vec3(x, y, z), Vec3(0.f, 0.f, 1.f), Vec2(1, 1));
	d = AddVertex(builder, Vec3(x, -y, z), Vec3(0.f, 0.f, 1.f), Vec2(1, 0));
	builder.AddQuad(a, b, c, d);

	a = AddVertex(builder, Vec3(-x, -y, -z), Vec3(0.f, -1.f, 0.f), Vec2(0, 0)); // -Y
	b = AddVertex(builder, Vec3(-x, -y, z), Vec3(0.f, -1.f, 0.f), Vec2(0, 1));
	c = AddVertex(builder, Vec3(x, -y, z), Vec3(0.f, -1.f, 0.f), Vec2(1, 1));
	d = AddVertex(builder, Vec3(x, -y, -z), Vec3(0.f, -1.f, 0.f), Vec2(1, 0));
	builder.AddQuad(a, b, c, d);

	a = AddVertex(builder, Vec3(-x, y, -z), Vec3(0.f, 1.f, 0.f), Vec2(0, 0)); // +Y
	b = AddVertex(builder, Vec3(-x, y, z), Vec3(0.f, 1.f, 0.f), Vec2(0, 1));
	c = AddVertex(builder, Vec3(x, y, z), Vec3(0.f, 1.f, 0.f), Vec2(1, 1));
	d = AddVertex(builder, Vec3(x, y, -z), Vec3(0.f, 1.f, 0.f), Vec2(1, 0));
	builder.AddQuad(d, c, b, a);

	a = AddVertex(builder, Vec3(-x, -y, -z), Vec3(-1.f, 0.f, 0.f), Vec2(0, 0)); // -X
	b = AddVertex(builder, Vec3(-x, -y, z), Vec3(-1.f, 0.f, 0.f), Vec2(0, 1));
	c = AddVertex(builder, Vec3(-x, y, z), Vec3(-1.f, 0.f, 0.f), Vec2(1, 1));
	d = AddVertex(builder, Vec3(-x, y, -z), Vec3(-1.f, 0.f, 0.f), Vec2(1, 0));
	builder.AddQuad(d, c, b, a);

	a = AddVertex(builder, Vec3(x, -y, -z), Vec3(1.f, 0.f, 0.f), Vec2(0, 0)); // +X
	b = AddVertex(builder, Vec3(x, -y, z), Vec3(1.f, 0.f, 0.f), Vec2(0, 1));
	c = AddVertex(builder, Vec3(x, y, z), Vec3(1.f, 0.f, 0.f), Vec2(1, 1));
	d = AddVertex(builder, Vec3(x, y, -z), Vec3(1.f, 0.f, 0.f), Vec2(1, 0));
	builder.AddQuad(a, b, c, d);

	return builder.MakeModel(layout);
}

Model CreateSphereModel(const VertexLayout &layout, float radius, int subdiv_x, int subdiv_y) {
	ModelBuilder builder;

	const VtxIdxType i_top = AddVertex(builder, Vec3(0.f, radius, 0.f), Vec3(0.f, 1.f, 0.f));
	const VtxIdxType i_bottom = AddVertex(builder, Vec3(0.f, -radius, 0.f), Vec3(0.f, -1.f, 0.f));

	std::vector<uint16_t> ref(subdiv_y + 1), old_ref;

	for (int s = 0; s < subdiv_x + 1; ++s) {
		const float t = (s + 1.f) / (subdiv_x + 2.f);
		const float a = t * Pi;

		const float section_y = Cos(a) * radius;
		const float section_radius = Sin(a) * radius;

		uint16_t i, j;

		const Vec3 v(section_radius, section_y, 0.f);
		ref[0] = i = AddVertex(builder, v, Normalize(v));

		for (int c = 1; c <= subdiv_y; ++c) {
			const float c_a = float(c) * TwoPi / subdiv_y;

			const Vec3 v(Cos(c_a) * section_radius, section_y, Sin(c_a) * section_radius);
			j = AddVertex(builder, v, Normalize(v));

			if (s == 0) // top section
				builder.AddTriangle(i_top, i, j);
			else // middle sections
				builder.AddQuad(old_ref[c], old_ref[c - 1], i, j);

			if (s == subdiv_x) // bottom section
				builder.AddTriangle(i, i_bottom, j);

			ref[c] = i = j;
		}

		old_ref = ref;
	}

	return builder.MakeModel(layout);
}

Model CreatePlaneModel(const VertexLayout &layout, float width, float length, int subdiv_x, int subdiv_z) {
	ModelBuilder builder;

	float dx = width / static_cast<float>(subdiv_x);
	float dz = length / static_cast<float>(subdiv_z);

	float z0 = -length / 2.f;

	for (int j = 0; j < subdiv_z; j++) {
		float z1 = z0 + dz;
		float x0 = -width / 2.f;

		for (int i = 0; i < subdiv_x; i++) {
			float x1 = x0 + dx;

			uint16_t a = AddVertex(builder, Vec3(x0, 0.f, z0), Vec3(0.f, 1.f, 0.f), Vec2(float(i) / subdiv_x, float(j) / subdiv_z));
			uint16_t b = AddVertex(builder, Vec3(x0, 0.f, z1), Vec3(0.f, 1.f, 0.f), Vec2(float(i) / subdiv_x, float(j + 1) / subdiv_z));
			uint16_t c = AddVertex(builder, Vec3(x1, 0.f, z1), Vec3(0.f, 1.f, 0.f), Vec2(float(i + 1) / subdiv_x, float(j + 1) / subdiv_z));
			uint16_t d = AddVertex(builder, Vec3(x1, 0.f, z0), Vec3(0.f, 1.f, 0.f), Vec2(float(i + 1) / subdiv_x, float(j) / subdiv_z));
			builder.AddQuad(d, c, b, a);

			x0 = x1;
		}
		z0 = z1;
	}

	return builder.MakeModel(layout);
}

Model CreateCylinderModel(const VertexLayout &layout, float radius, float height, int subdiv_x) {
	ModelBuilder builder;

	if (subdiv_x < 3) {
		subdiv_x = 3;
	}

	std::vector<VtxIdxType> ref(subdiv_x * 2);
	const float z = height / 2.f;
	for (int i = 0; i < subdiv_x; i++) {
		float t = 2.f * Pi * i / static_cast<float>(subdiv_x);
		float cs = Cos(t);
		float sn = Sin(t);
		float x = radius * cs, y = radius * sn;
		ref[2 * i] = AddVertex(builder, Vec3(x, z, y), Vec3(cs, 0.f, sn));
		ref[2 * i + 1] = AddVertex(builder, Vec3(x, -z, y), Vec3(cs, 0.f, sn));
	}

	VtxIdxType a = ref[2 * subdiv_x - 2];
	VtxIdxType b = ref[2 * subdiv_x - 1];
	for (int i = 0; i < subdiv_x; i++) {
		const VtxIdxType c = ref[2 * i + 1];
		const VtxIdxType d = ref[2 * i];
		builder.AddQuad(a, b, c, d);
		a = d;
		b = c;
	}

	std::vector<VtxIdxType> cap(subdiv_x + 2);
	cap[0] = AddVertex(builder, Vec3(0, z, 0), Vec3(0, 1, 0));
	for (int i = 0; i < subdiv_x; i++) {
		cap[i + 1] = ref[2 * i];
	}
	cap[subdiv_x + 1] = ref[0];
	builder.AddPolygon(cap);

	cap[0] = AddVertex(builder, Vec3(0, -z, 0), Vec3(0, -1, 0));
	for (int i = 0; i < subdiv_x; i++) {
		cap[i + 1] = ref[2 * (subdiv_x - i) - 1];
	}
	cap[subdiv_x + 1] = ref[2 * subdiv_x - 1];
	builder.AddPolygon(cap);

	return builder.MakeModel(layout);
}

Model CreateConeModel(const VertexLayout &layout, float radius, float height, int subdiv_x) {
	ModelBuilder builder;

	if (subdiv_x < 3)
		subdiv_x = 3;

	std::vector<VtxIdxType> ref(subdiv_x + 2);
	for (int i = 0; i < subdiv_x; i++) {
		float t = 2.f * Pi * i / static_cast<float>(subdiv_x);
		float cs = Cos(t);
		float sn = Sin(t);
		float x = radius * cs, y = radius * sn;
		ref[i + 1] = AddVertex(builder, Vec3(x, 0, y), Vec3(cs, 0.f, sn));
	}
	ref[subdiv_x + 1] = ref[0];

	ref[0] = AddVertex(builder, Vec3(0, height, 0), Vec3(0, 1, 0));
	builder.AddPolygon(ref);

	std::reverse(ref.begin() + 1, ref.end());

	ref[0] = AddVertex(builder, Vec3(0, 0, 0), Vec3(0, -1, 0));
	builder.AddPolygon(ref);

	return builder.MakeModel(layout);
}

Model CreateCapsuleModel(const VertexLayout &layout, float radius, float height, int subdiv_x, int subdiv_y) {
	if (subdiv_x < 3)
		subdiv_x = 3;
	if (subdiv_y < 1)
		subdiv_y = 1;

	if (height < (2.f * radius))
		return CreateSphereModel(layout, radius, subdiv_x, subdiv_y);

	ModelBuilder builder;

	// cylinder
	std::vector<VtxIdxType> ref(subdiv_x * 2);
	const float z = height / 2.f - radius;
	for (int i = 0; i < subdiv_x; i++) {
		float t = 2.f * Pi * i / static_cast<float>(subdiv_x);
		float cs = Cos(t);
		float sn = Sin(t);
		float x = radius * cs, y = radius * sn;
		ref[2 * i] = AddVertex(builder, Vec3(x, z, y), Vec3(cs, 0.f, sn));
		ref[2 * i + 1] = AddVertex(builder, Vec3(x, -z, y), Vec3(cs, 0.f, sn));
	}

	VtxIdxType a0, b0, a1, b1;
	a0 = ref[2 * subdiv_x - 2];
	b0 = ref[2 * subdiv_x - 1];
	for (int i = 0; i < subdiv_x; i++) {
		VtxIdxType c0 = ref[2 * i + 1];
		VtxIdxType d0 = ref[2 * i];
		builder.AddQuad(a0, b0, c0, d0);
		a0 = d0;
		b0 = c0;
	}

	VtxIdxType top = AddVertex(builder, Vec3(0.f, height / 2.f, 0.f), Vec3(0.f, 1.f, 0.f));
	VtxIdxType bottom = AddVertex(builder, Vec3(0.f, -height / 2.f, 0.f), Vec3(0.f, -1.f, 0.f));

	std::vector<VtxIdxType> fan(2 + subdiv_x);

	if (subdiv_y == 1) {
		// top cap
		fan[0] = top;
		for (int i = 0; i < subdiv_x; i++) {
			fan[i + 1] = ref[2 * i];
		}
		fan[subdiv_x + 1] = fan[1];
		builder.AddPolygon(fan);

		// bottom cap
		fan[0] = bottom;
		for (int i = 0; i < subdiv_x; i++) {
			fan[i + 1] = ref[2 * (subdiv_x - 1 - i) + 1];
		}
		fan[subdiv_x + 1] = fan[1];
		builder.AddPolygon(fan);

		return builder.MakeModel(layout);
	}

	// hemispherical caps
	std::vector<VtxIdxType> cap(subdiv_x * 2 * (subdiv_y - 1));
	for (int i = 0, k = 0; i < subdiv_x; i++) {
		float t0 = 2.f * Pi * i / static_cast<float>(subdiv_x);
		float c0 = radius * Cos(t0);
		float s0 = radius * Sin(t0);
		for (int j = 1; j < subdiv_y; j++) {
			float t1 = Pi * (j / static_cast<float>(subdiv_y)) / 2.f;
			Vec3 v(c0 * Sin(t1), radius * Cos(t1), s0 * Sin(t1));
			cap[k++] = AddVertex(builder, Vec3(v.x, v.y + z, v.z), Normalize(v));
			v.y = -v.y;
			cap[k++] = AddVertex(builder, Vec3(v.x, v.y - z, v.z), Normalize(v));
		}
	}

	// top cap
	fan[0] = top;
	for (int i = 0; i < subdiv_x; i++)
		fan[i + 1] = cap[2 * (subdiv_y - 1) * i];

	fan[subdiv_x + 1] = fan[1];
	builder.AddPolygon(fan);

	// bottom cap
	fan[0] = bottom;
	for (int i = 0; i < subdiv_x; i++)
		fan[i + 1] = cap[2 * (subdiv_y - 1) * (subdiv_x - 1 - i) + 1];

	fan[subdiv_x + 1] = fan[1];
	builder.AddPolygon(fan);

	// inner
	for (int j = 0; j < (subdiv_y - 2); j++) {
		int k = 2 * ((subdiv_y - 1) * (subdiv_x - 1) + j);
		// top
		a0 = cap[k];
		b0 = cap[k + 2];
		// bottom
		a1 = cap[k + 3];
		b1 = cap[k + 1];
		for (int i = 0; i < subdiv_x; i++) {
			k = 2 * ((subdiv_y - 1) * i + j);
			VtxIdxType c0 = cap[k + 2];
			VtxIdxType d0 = cap[k];
			builder.AddQuad(a0, b0, c0, d0);
			a0 = d0;
			b0 = c0;

			VtxIdxType c1 = cap[k + 1];
			VtxIdxType d1 = cap[k + 3];
			builder.AddQuad(a1, b1, c1, d1);
			a1 = d1;
			b1 = c1;
		}
	}

	// junction
	int offset = 2 * ((subdiv_y - 1) * subdiv_x - 1);
	a0 = cap[offset];
	b1 = cap[offset + 1];
	offset = 2 * (subdiv_x - 1);
	b0 = ref[offset];
	a1 = ref[offset + 1];
	for (int i = 0; i < subdiv_x; i++) {
		offset = 2 * i;
		VtxIdxType c0 = ref[offset];
		VtxIdxType d1 = ref[offset + 1];
		offset = 2 * ((subdiv_y - 1) * (i + 1) - 1);
		VtxIdxType d0 = cap[offset];
		VtxIdxType c1 = cap[offset + 1];
		builder.AddQuad(a0, b0, c0, d0);
		builder.AddQuad(a1, b1, c1, d1);
		a0 = d0;
		b0 = c0;
		a1 = d1;
		b1 = c1;
	}

	return builder.MakeModel(layout);
}

} // namespace hg
