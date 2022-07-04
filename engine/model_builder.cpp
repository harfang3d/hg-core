// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "engine/model_builder.h"

#include "foundation/log.h"

#include <fmt/format.h>
#include <meshoptimizer.h>

namespace hg {

ModelBuilder::ModelBuilder() {
	lists.reserve(4);
	lists.push_back({});
}

size_t ModelBuilder::GetCurrentListIndexCount() const { return lists.back().idx.size(); }

bool ModelBuilder::EndList(uint16_t material) {
	auto &list = lists.back();

	if (list.idx.empty()) {
		list.vtx.clear();
		list.bones_table.clear();
		list.vtx_lookup.clear();
		return false;
	}

	lists.back().mat = material; // close current list, store material
	NewList();

	return true;
}

static uint64_t fnv1a64(const void *buf, size_t len) {
	static const uint64_t fnv64_prime = UINT64_C(1099511628211);
	static const uint64_t fnv64_offset = UINT64_C(14695981039346656037);

	auto pointer = reinterpret_cast<const uint8_t *>(buf);
	const uint8_t *buf_end = pointer + len;
	uint64_t hash = fnv64_offset;

	while (pointer < buf_end) {
		hash ^= uint64_t(*pointer++);
		hash *= fnv64_prime;
	}

	return hash;
}

static bool operator==(const Vertex &a, const Vertex &b) {
	const bool uv =
		a.uv0 == b.uv0 && a.uv1 == b.uv1 && a.uv2 == b.uv2 && a.uv3 == b.uv3 && a.uv4 == b.uv4 && a.uv5 == b.uv5 && a.uv6 == b.uv6 && a.uv7 == b.uv7;
	const bool c = a.color0 == b.color0 && a.color1 == b.color1 && a.color2 == b.color2 && a.color3 == b.color3;
	const bool i = a.index[0] == b.index[0] && a.index[1] == b.index[1] && a.index[2] == b.index[2] && a.index[3] == b.index[3];
	const bool w = a.weight[0] == b.weight[0] && a.weight[1] == b.weight[1] && a.weight[2] == b.weight[2] && a.weight[3] == b.weight[3];
	return a.pos == b.pos && a.normal == b.normal && a.tangent == b.tangent && a.binormal == b.binormal && uv && c && i && w;
}

VtxIdxType ModelBuilder::AddVertex(const Vertex &vtx) {
	auto &list = lists.back();

	size_t idx = list.vtx.size();

	const auto hash = fnv1a64(&vtx, sizeof(Vertex));
	const auto i_vtx = list.vtx_lookup.find(hash);

	if (i_vtx == std::end(list.vtx_lookup)) {
		list.vtx_lookup[hash] = VtxIdxType(idx); // store hash
		list.vtx.push_back(vtx); // commit candidate
	} else {
		const auto hashed_idx = i_vtx->second;

		if (list.vtx[hashed_idx] == vtx) {
			idx = hashed_idx;
		} else {
			++hash_collision;

			auto i = std::find(std::begin(list.vtx), std::end(list.vtx), vtx);
			if (i != std::end(list.vtx))
				idx = std::distance(std::begin(list.vtx), i);
			else
				list.vtx.push_back(vtx); // commit candidate
		}
	}
	return VtxIdxType(idx);
}

//
void ModelBuilder::AddTriangle(VtxIdxType a, VtxIdxType b, VtxIdxType c) {
	auto &list = lists.back();

	list.idx.push_back(a);
	list.idx.push_back(b);
	list.idx.push_back(c);
}

void ModelBuilder::AddQuad(VtxIdxType a, VtxIdxType b, VtxIdxType c, VtxIdxType d) {
	AddTriangle(a, b, c);
	AddTriangle(a, c, d);
}

void ModelBuilder::AddPolygon(const VtxIdxType *idxs, size_t n) {
	for (size_t i = 1; i < n - 1; ++i)
		AddTriangle(idxs[0], idxs[i], idxs[i + 1]);
}

void ModelBuilder::AddPolygon(const std::vector<VtxIdxType> &idxs) {
	for (int i = 1; i < idxs.size() - 1; ++i)
		AddTriangle(idxs[0], idxs[i], idxs[i + 1]);
}

void ModelBuilder::AddBoneIdx(uint16_t idx) {
	auto &list = lists.back();
	list.bones_table.push_back(idx);
	__ASSERT__(list.bones_table.size() <= max_skinned_model_matrix_count);
}

//
void ModelBuilder::Make(const VertexLayout &decl, end_list_cb on_end_list, void *userdata, ModelOptimisationLevel optimisation_level, bool verbose) const {
	const auto stride = decl.GetStride();

	Model model;
	model.lists.reserve(lists.size());

	size_t vtx_count = 0;
	for (auto &list : lists) {
		if (list.idx.empty() || list.vtx.empty())
			continue;

		MinMax minmax = {Vec3::Max, Vec3::Min};

		std::vector<int8_t> vtx_data(list.vtx.size() * stride);
		int8_t *p_vtx = vtx_data.data();

		for (const auto &vtx : list.vtx) {
			decl.PackVertex(VAS_Position, &vtx.pos.x, 3, p_vtx);

			if (decl.Has(VAS_Normal))
				decl.PackVertex(VAS_Normal, &vtx.normal.x, 3, p_vtx);
			if (decl.Has(VAS_Tangent))
				decl.PackVertex(VAS_Tangent, &vtx.tangent.x, 3, p_vtx);
			if (decl.Has(VAS_Bitangent))
				decl.PackVertex(VAS_Bitangent, &vtx.binormal.x, 3, p_vtx);

			if (decl.Has(VAS_Color))
				decl.PackVertex(VAS_Color, &vtx.color0.r, 4, p_vtx);

			if (decl.Has(VAS_UV0))
				decl.PackVertex(VAS_UV0, &vtx.uv0.x, 2, p_vtx);
			if (decl.Has(VAS_UV1))
				decl.PackVertex(VAS_UV1, &vtx.uv1.x, 2, p_vtx);

			if (decl.Has(VAS_BoneIndices))
				decl.PackVertex(VAS_BoneIndices, vtx.index, 4, p_vtx);
			if (decl.Has(VAS_BoneWeights))
				decl.PackVertex(VAS_BoneWeights, vtx.weight, 4, p_vtx);

			minmax.mn = Min(minmax.mn, vtx.pos); // update list minmax
			minmax.mx = Max(minmax.mx, vtx.pos);

			vtx_count += list.vtx.size();
			p_vtx += stride;
		}

		if (verbose)
			debug(fmt::format("End list {} indexes, {} vertices, material index {}", list.idx.size(), list.vtx.size(), list.mat));

		//
		if (optimisation_level == MOL_None) {
			on_end_list(decl, minmax, list.idx, vtx_data, list.bones_table, list.mat, userdata);
		} else if (optimisation_level == MOL_Minimal) {
			std::vector<uint32_t> idx(list.idx.size());
			meshopt_optimizeVertexCache(idx.data(), list.idx.data(), list.idx.size(), list.vtx.size());

			on_end_list(decl, minmax, idx, vtx_data, list.bones_table, list.mat, userdata);
		} else if (optimisation_level == MOL_Full) {
			std::vector<uint32_t> idx_a(list.idx.size()), idx_b(list.idx.size());
			meshopt_optimizeVertexCache(idx_a.data(), list.idx.data(), list.idx.size(), list.vtx.size());
			meshopt_optimizeOverdraw(idx_b.data(), idx_a.data(), list.idx.size(), reinterpret_cast<float *>(vtx_data.data()), list.vtx.size(), stride, 1.05f);

			std::vector<int8_t> vtx(list.vtx.size() * stride);
			meshopt_optimizeVertexFetch(vtx.data(), idx_b.data(), idx_b.size(), vtx_data.data(), list.vtx.size(), stride);

			on_end_list(decl, minmax, idx_b, vtx, list.bones_table, list.mat, userdata);
		}
	}

	if (verbose) {
		if (vtx_count == 0)
			debug("No vertex in geometry!");
		else
			debug(fmt::format("Vertex hash collision: {} ({}%)", hash_collision, hash_collision * 100 / vtx_count));
	}
}

Model ModelBuilder::MakeModel(const VertexLayout &layout, ModelOptimisationLevel optimisation_level, bool verbose) const {
	Model model;
	model.lists.reserve(16);

	Make(
		layout,
		[](const VertexLayout &layout, const MinMax &minmax, const std::vector<VtxIdxType> &idx_data, const std::vector<int8_t> &vtx_data,
			const std::vector<uint16_t> &bones_table, uint16_t mat, void *userdata) {
			Model &model = *reinterpret_cast<Model *>(userdata);

			DisplayList list;
			// list.index_buffer;
			list.vertex_buffer = MakeVertexBuffer(vtx_data.data(), vtx_data.size());
			// const auto idx_hnd = bgfx::createIndexBuffer(bgfx::copy(idx_data.data(), uint32_t(idx_data.size() * sizeof(uint32_t))), BGFX_BUFFER_INDEX32);
			// const auto vtx_hnd = bgfx::createVertexBuffer(bgfx::copy(vtx_data.data(), uint32_t(vtx_data.size())), decl);

			model.bounds.push_back(minmax);
			model.lists.push_back(list);
			model.mats.push_back(mat);
		},
		&model, optimisation_level, verbose);

	return model;
}

//
void ModelBuilder::NewList() {
	lists.push_back({});
	lists.back().idx.reserve(256);
	lists.back().vtx.reserve(256);
}

void ModelBuilder::Clear() {
	hash_collision = 0;
	lists.clear();
	NewList();
}

} // namespace hg
