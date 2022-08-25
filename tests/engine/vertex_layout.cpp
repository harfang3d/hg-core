// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#define TEST_NO_MAIN
#include "acutest.h"

#include "engine/render_pipeline.h"

#include "foundation/math.h"
#include "foundation/pack_float.h"

using namespace hg;

void test_vertex_layout() {
	{
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

		size_t offset = 0;
		layout.Set(VA_Position, SG_VERTEXFORMAT_FLOAT3, offset);
		offset += 3 * sizeof(float);
		layout.Set(VA_Normal, SG_VERTEXFORMAT_FLOAT3, offset);
		offset += 3 * sizeof(float);
		layout.Set(VA_Color, SG_VERTEXFORMAT_FLOAT4, offset);
		offset += 4 * sizeof(float);
		layout.Set(VA_UV0, SG_VERTEXFORMAT_FLOAT2, offset);
		offset += 2 * sizeof(float);
		layout.Set(VA_UV1, SG_VERTEXFORMAT_FLOAT, offset);
		offset += sizeof(float);

		TEST_CHECK(layout.GetStride() == offset);

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

		TEST_CHECK(layout.GetStride() == 52);

		TEST_CHECK(layout.GetOffset(VA_Position) == 0);
		TEST_CHECK(layout.GetOffset(VA_Normal) == 12);
		TEST_CHECK(layout.GetOffset(VA_Tangent) == -1);
		TEST_CHECK(layout.GetOffset(VA_Bitangent) == -1);
		TEST_CHECK(layout.GetOffset(VA_Color) == 24);
		TEST_CHECK(layout.GetOffset(VA_BoneIndices) == -1);
		TEST_CHECK(layout.GetOffset(VA_BoneWeights) == -1);
		TEST_CHECK(layout.GetOffset(VA_UV0) == 40);
		TEST_CHECK(layout.GetOffset(VA_UV1) == 48);

		const size_t vertex_count = 4;

		struct {
			Vec3 pos;
			Vec3 normal;
			Vec3 col;
			Vec3 uv;
		} quad[4] = {{Vec3(-1.f, -1.f, 1.f), Vec3::Back, Vec3(1.f, 0.f, 0.f), Vec3(0.f, 0.f, 0.f)},
			{Vec3(1.f, -1.f, 1.f), Vec3::Back, Vec3(0.f, 1.f, 0.f), Vec3(1.f, 0.f, 1.f)},
			{Vec3(1.f, 1.f, 1.f), Vec3::Back, Vec3(0.f, 0.f, 1.f), Vec3(1.f, 1.f, 0.f)},
			{Vec3(-1.f, 1.f, 1.f), Vec3::Back, Vec3(1.f, 0.f, 1.f), Vec3(0.f, 1.f, 1.f)}};

		size_t stride = offset; // [todo] layout.GetStride();
		std::vector<int8_t> buffer(vertex_count * stride);
		int8_t *ptr;

		ptr = buffer.data();
		for (size_t i = 0; i < vertex_count; i++, ptr += stride) {
			layout.PackVertex(VA_Position, &quad[i].pos.x, 3, ptr);
			layout.PackVertex(VA_Normal, &quad[i].normal.x, 3, ptr);
			layout.PackVertex(VA_UV0, &quad[i].uv.x, 3, ptr);
			layout.PackVertex(VA_Color, &quad[i].col.x, 3, ptr);
			layout.PackVertex(VA_UV1, &hg::Color::White.r, 4, ptr);
		}

		ptr = buffer.data();
		for (size_t i = 0; i < vertex_count; i++, ptr += stride) {
			float *pos = reinterpret_cast<float *>(ptr + layout.GetOffset(VA_Position));
			float *normal = reinterpret_cast<float *>(ptr + layout.GetOffset(VA_Normal));
			float *color = reinterpret_cast<float *>(ptr + layout.GetOffset(VA_Color));
			float *uv0 = reinterpret_cast<float *>(ptr + layout.GetOffset(VA_UV0));
			float *uv1 = reinterpret_cast<float *>(ptr + layout.GetOffset(VA_UV1));

			TEST_CHECK(Equal(pos[0], quad[i].pos.x));
			TEST_CHECK(Equal(pos[1], quad[i].pos.y));
			TEST_CHECK(Equal(pos[2], quad[i].pos.z));

			TEST_CHECK(Equal(normal[0], quad[i].normal.x));
			TEST_CHECK(Equal(normal[1], quad[i].normal.y));
			TEST_CHECK(Equal(normal[2], quad[i].normal.z));

			TEST_CHECK(Equal(color[0], quad[i].col.x));
			TEST_CHECK(Equal(color[1], quad[i].col.y));
			TEST_CHECK(Equal(color[2], quad[i].col.z));
			TEST_CHECK(Equal(color[3], 0.f));

			TEST_CHECK(Equal(uv0[0], quad[i].uv.x));
			TEST_CHECK(Equal(uv0[1], quad[i].uv.y));

			TEST_CHECK(Equal(uv1[0], hg::Color::White.r));
		}
	}

	{
		VertexLayout layout;

		size_t offset = 0;
		layout.Set(VA_Position, SG_VERTEXFORMAT_BYTE4, offset);
		offset += 4;
		layout.Set(VA_Normal, SG_VERTEXFORMAT_BYTE4N, offset);
		offset += 4;
		layout.Set(VA_Color, SG_VERTEXFORMAT_UBYTE4, offset);
		offset += 4;
		layout.Set(VA_UV0, SG_VERTEXFORMAT_UBYTE4N, offset);
		offset += 4;

		size_t stride = layout.GetStride();
		TEST_CHECK(stride == offset);

		stride = offset; // [todo] remove this as soon as stride computation is fixed

		const size_t vertex_count = 3;

		struct {
			Vec3 pos;
			Vec3 normal;
			Color col;
			Vec2 uv;
		} triangle[4] = {{Vec3(-11.f, 0.f, 7.f), Vec3::Front, Color::Red, Vec2(0.f, 0.f)},
			{Vec3(10.f, 0.f, 5.f), Vec3::Right, Color::Green, Vec2(1.f, 0.f)},
			{Vec3(0.f, 7.f, -1.f), Vec3::Down, Color::Blue, Vec2(0.f, 1.f)}};

		std::vector<int8_t> buffer(vertex_count * stride);
		int8_t *ptr;

		ptr = buffer.data();
		for (size_t i = 0; i < vertex_count; i++, ptr += stride) {
			layout.PackVertex(VA_Position, &triangle[i].pos.x, 3, ptr);
			layout.PackVertex(VA_Normal, &triangle[i].normal.x, 3, ptr);
			layout.PackVertex(VA_Color, &triangle[i].col.r, 4, ptr);
			layout.PackVertex(VA_UV0, &triangle[i].uv.x, 2, ptr);
		}

		ptr = buffer.data();
		for (size_t i = 0; i < vertex_count; i++, ptr += stride) {
			const int8_t *pos = ptr + layout.GetOffset(VA_Position);
			const int8_t *normal = ptr + layout.GetOffset(VA_Normal);
			const uint8_t *color = reinterpret_cast<uint8_t *>(ptr + layout.GetOffset(VA_Color));
			const uint8_t *uv0 = reinterpret_cast<uint8_t *>(ptr + layout.GetOffset(VA_UV0));

			TEST_CHECK(AlmostEqual(Vec3(pos[0], pos[1], pos[2]), triangle[i].pos, 0.000001f));
			TEST_CHECK(pos[3] == 0);

			TEST_CHECK(AlmostEqual(Color(color[0], color[1], color[2], color[3]), triangle[i].col, 0.000001f));

			Vec3 n(unpack_float<int8_t>(normal[0]), unpack_float<int8_t>(normal[1]), unpack_float<int8_t>(normal[2]));
			TEST_CHECK(AlmostEqual(n, triangle[i].normal, 0.000001f));
			TEST_CHECK(normal[3] == 0);

			Vec2 t(unpack_float<uint8_t>(uv0[0]), unpack_float<uint8_t>(uv0[1]));
			TEST_CHECK(AlmostEqual(t, triangle[i].uv, 0.000001f));
			TEST_CHECK(uv0[2] == 0);
			TEST_CHECK(uv0[3] == 0);
		}
	}
}