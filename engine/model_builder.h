// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#pragma once

#include "engine/render_pipeline.h"
#include "engine/vertex.h"

#include "foundation/cext.h"
#include "foundation/vector2.h"

#include <map>
#include <vector>

namespace hg {

typedef uint32_t VtxIdxType;

struct ModelBuilder {
	ModelBuilder();

	VtxIdxType AddVertex(const Vertex &v);

	void AddTriangle(VtxIdxType a, VtxIdxType b, VtxIdxType c);
	void AddQuad(VtxIdxType a, VtxIdxType b, VtxIdxType c, VtxIdxType d);
	void AddPolygon(const VtxIdxType *idxs, size_t n);
	void AddPolygon(const std::vector<VtxIdxType> &idxs);
	void AddBoneIdx(uint16_t AddBoneIdx);

	size_t GetCurrentListIndexCount() const;

	/// End current list. Return false if list is empty, meaning no new list has been created.
	bool EndList(uint16_t material);

	void Clear();

	typedef void (*end_list_cb)(const VertexLayout &decl, const MinMax &minmax, const std::vector<VtxIdxType> &idx_data, const std::vector<int8_t> &vtx_data,
		const std::vector<uint16_t> &bones_table, uint16_t mat, void *userdata);

	void Make(
		const VertexLayout &decl, end_list_cb on_end_list, void *userdata, bool verbose = false) const;

	Model MakeModel(const VertexLayout &decl, bool verbose = false) const;

private:
	size_t hash_collision;

	struct List {
		std::vector<VtxIdxType> idx;
		std::vector<Vertex> vtx;
		std::vector<uint16_t> bones_table;

		std::map<uint64_t, VtxIdxType> vtx_lookup;
		uint16_t mat;

		MinMax minmax;
	};

	std::vector<List> lists;

	void NewList();
};

} // namespace hg
