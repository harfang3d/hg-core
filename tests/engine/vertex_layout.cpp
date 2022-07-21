// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#define TEST_NO_MAIN
#include "acutest.h"

#include "engine/render_pipeline.h"

#include "foundation/math.h"

using namespace hg;

void test_vertex_layout() {
	VertexLayout layout;

	TEST_CHECK(layout.GetStride() == 0);

	TEST_CHECK(layout.Has(VA_Position) == false);
	TEST_CHECK(layout.Has(VA_Normal) == false);
	TEST_CHECK(layout.Has(VA_Tangent) == false);
	TEST_CHECK(layout.Has(VA_Bitangent) == false);
	TEST_CHECK(layout.Has(VA_Color) == false);
	TEST_CHECK(layout.Has(VA_BoneIndices) == false);
	TEST_CHECK(layout.Has(VA_BoneWeights) == false);
	TEST_CHECK(layout.Has(VA_UV0) == false);
	TEST_CHECK(layout.Has(VA_UV1) == false);
	
	TEST_CHECK(layout.GetFormat(VA_Position) == SG_VERTEXFORMAT_INVALID);
	TEST_CHECK(layout.GetFormat(VA_Normal) == SG_VERTEXFORMAT_INVALID);
	TEST_CHECK(layout.GetFormat(VA_Tangent) == SG_VERTEXFORMAT_INVALID);
	TEST_CHECK(layout.GetFormat(VA_Bitangent) == SG_VERTEXFORMAT_INVALID);
	TEST_CHECK(layout.GetFormat(VA_Color) == SG_VERTEXFORMAT_INVALID);
	TEST_CHECK(layout.GetFormat(VA_BoneIndices) == SG_VERTEXFORMAT_INVALID);
	TEST_CHECK(layout.GetFormat(VA_BoneWeights) == SG_VERTEXFORMAT_INVALID);
	TEST_CHECK(layout.GetFormat(VA_UV0) == SG_VERTEXFORMAT_INVALID);
	TEST_CHECK(layout.GetFormat(VA_UV1) == SG_VERTEXFORMAT_INVALID);
	
	layout.Set(VA_Position, SG_VERTEXFORMAT_FLOAT3);
	layout.Set(VA_Normal, SG_VERTEXFORMAT_FLOAT3);
	layout.Set(VA_Color, SG_VERTEXFORMAT_FLOAT4);
	layout.Set(VA_UV0, SG_VERTEXFORMAT_FLOAT2);
	layout.Set(VA_UV1, SG_VERTEXFORMAT_FLOAT);

	TEST_CHECK(layout.GetStride() == 52);

	TEST_CHECK(layout.Has(VA_Position) == true);
	TEST_CHECK(layout.Has(VA_Normal) == true);
	TEST_CHECK(layout.Has(VA_Tangent) == false);
	TEST_CHECK(layout.Has(VA_Bitangent) == false);
	TEST_CHECK(layout.Has(VA_Color) == true);
	TEST_CHECK(layout.Has(VA_BoneIndices) == false);
	TEST_CHECK(layout.Has(VA_BoneWeights) == false);
	TEST_CHECK(layout.Has(VA_UV0) == true);
	TEST_CHECK(layout.Has(VA_UV1) == true);

	TEST_CHECK(layout.GetFormat(VA_Position) == SG_VERTEXFORMAT_FLOAT3);
	TEST_CHECK(layout.GetFormat(VA_Normal) == SG_VERTEXFORMAT_FLOAT3);
	TEST_CHECK(layout.GetFormat(VA_Tangent) == SG_VERTEXFORMAT_INVALID);
	TEST_CHECK(layout.GetFormat(VA_Bitangent) == SG_VERTEXFORMAT_INVALID);
	TEST_CHECK(layout.GetFormat(VA_Color) == SG_VERTEXFORMAT_FLOAT4);
	TEST_CHECK(layout.GetFormat(VA_BoneIndices) == SG_VERTEXFORMAT_INVALID);
	TEST_CHECK(layout.GetFormat(VA_BoneWeights) == SG_VERTEXFORMAT_INVALID);
	TEST_CHECK(layout.GetFormat(VA_UV0) == SG_VERTEXFORMAT_FLOAT2);
	TEST_CHECK(layout.GetFormat(VA_UV1) == SG_VERTEXFORMAT_FLOAT);

	int offset[VA_Count];

	size_t stride = layout.GetOffsets(offset);
	TEST_CHECK(stride == 52);
	TEST_CHECK(stride == layout.GetStride());

	TEST_CHECK(offset[VA_Position] == 0);
	TEST_CHECK(offset[VA_Normal] == 12);
	TEST_CHECK(offset[VA_Tangent] == -1);
	TEST_CHECK(offset[VA_Bitangent] == -1);
	TEST_CHECK(offset[VA_Color] == 24);
	TEST_CHECK(offset[VA_BoneIndices] == -1);
	TEST_CHECK(offset[VA_BoneWeights] == -1);
	TEST_CHECK(offset[VA_UV0] == 40);
	TEST_CHECK(offset[VA_UV1] == 48);

	const size_t vertex_count = 4;

	struct {
		Vec3 pos;
		Vec3 normal;
		Vec3 col;
		Vec3 uv;
	} quad[4] = {{Vec3(-1.f, -1.f, 1.f), Vec3::Back, Vec3(1.f, 0.f, 0.f), Vec3(0.f, 0.f, 0.f)},
		{Vec3(1.f, -1.f, 1.f), Vec3::Back, Vec3(0.f, 1.f, 0.f), Vec3(1.f, 0.f, 1.f)}, {Vec3(1.f, 1.f, 1.f), Vec3::Back, Vec3(0.f, 0.f, 1.f), Vec3(1.f, 1.f, 0.f)},
		{Vec3(-1.f, 1.f, 1.f), Vec3::Back, Vec3(1.f, 0.f, 1.f), Vec3(0.f, 1.f, 1.f)}};

	std::vector<int8_t> buffer(vertex_count * stride);
	int8_t *ptr;

	ptr = buffer.data();
	for (size_t i = 0; i < vertex_count; i++, ptr += stride) {
		layout.PackVertex(VA_Position, offset, &quad[i].pos.x, 3, ptr);
		layout.PackVertex(VA_Normal, offset, &quad[i].normal.x, 3, ptr);
		layout.PackVertex(VA_UV0, offset, &quad[i].uv.x, 3, ptr);
		layout.PackVertex(VA_Color, offset, &quad[i].col.x, 3, ptr);
		layout.PackVertex(VA_UV1, offset, &hg::Color::White.r, 4, ptr);
	}

	ptr = buffer.data();
	for (size_t i = 0; i < vertex_count; i++, ptr += stride) {
		float *pos = reinterpret_cast<float *>(ptr + offset[VA_Position]);
		float *normal = reinterpret_cast<float *>(ptr + offset[VA_Normal]);
		float *color = reinterpret_cast<float *>(ptr + offset[VA_Color]);
		float *uv0 = reinterpret_cast<float *>(ptr + offset[VA_UV0]);
		float *uv1 = reinterpret_cast<float *>(ptr + offset[VA_UV1]);

		TEST_CHECK(TestEqual(pos[0], quad[i].pos.x));
		TEST_CHECK(TestEqual(pos[1], quad[i].pos.y));
		TEST_CHECK(TestEqual(pos[2], quad[i].pos.z));

		TEST_CHECK(TestEqual(normal[0], quad[i].normal.x));
		TEST_CHECK(TestEqual(normal[1], quad[i].normal.y));
		TEST_CHECK(TestEqual(normal[2], quad[i].normal.z));

		TEST_CHECK(TestEqual(color[0], quad[i].col.x));
		TEST_CHECK(TestEqual(color[1], quad[i].col.y));
		TEST_CHECK(TestEqual(color[2], quad[i].col.z));
		TEST_CHECK(TestEqual(color[3], 0.f));

		TEST_CHECK(TestEqual(uv0[0], quad[i].uv.x));
		TEST_CHECK(TestEqual(uv0[1], quad[i].uv.y));

		TEST_CHECK(TestEqual(uv1[0], hg::Color::White.r));
	}
}