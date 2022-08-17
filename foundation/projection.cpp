// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "projection.h"
#include "assert.h"
#include "unit.h"

namespace hg {

static NDCInfos g_ndc_infos = {};

void SetNDCInfos(bool origin_bottom_left, bool homogeneous_depth) {
	g_ndc_infos.origin_bottom_left = origin_bottom_left;
	g_ndc_infos.homogeneous_depth = homogeneous_depth;
}

const NDCInfos &GetNDCInfos() {
	return g_ndc_infos;
}

float ZoomFactorToFov(float zoom_factor) {
	return ATan(1.F / zoom_factor) * 2.F;
}

float FovToZoomFactor(float fov) {
	return 1.F / Tan(Clamp(fov, Deg(0.1F), Deg(179.9F)) * 0.5F);
}

Mat44 ComputeOrthographicProjectionMatrix(float znear, float zfar, float size, const Vec2 &aspect_ratio, const Vec2 &offset) {
	const NDCInfos &ndc_infos = GetNDCInfos();

	const float qA = ndc_infos.homogeneous_depth ? (2.F / (zfar - znear)) : (1.F / (zfar - znear));
	const float qB = ndc_infos.homogeneous_depth ? (-(zfar + znear) / (zfar - znear)) : (-qA * znear);
	return Mat44(2.F / size / aspect_ratio.x, 0.F, 0.F, 0.F, 0.F, 2.F / size / aspect_ratio.y, 0.F, 0.F, 0.F, 0.F, qA, 0.F, offset.x, offset.y, qB, 1.F);
}

Mat44 ComputePerspectiveProjectionMatrix(float znear, float zfar, float zoom_factor, const Vec2 &aspect_ratio, const Vec2 &offset) {
	const NDCInfos &ndc_infos = GetNDCInfos();

	const float qA = ndc_infos.homogeneous_depth ? ((zfar + znear) / (zfar - znear)) : (zfar / (zfar - znear));
	const float qB = ndc_infos.homogeneous_depth ? (-2.F * zfar * znear / (zfar - znear)) : (-qA * znear);

	return Mat44(zoom_factor / aspect_ratio.x, 0.F, 0.F, 0.F, 0.F, zoom_factor / aspect_ratio.y, 0.F, 0.F, 0.F, 0.F, qA, 1.F, offset.x, offset.y, qB, 0.F);
}

Mat44 Compute2DProjectionMatrix(float znear, float zfar, float res_x, float res_y, bool y_up) {
	const NDCInfos &ndc_infos = GetNDCInfos();

	const float qA = ndc_infos.homogeneous_depth ? (2.F / (zfar - znear)) : (1.F / (zfar - znear));
	const float qB = ndc_infos.homogeneous_depth ? (-(zfar + znear) / (zfar - znear)) : (-qA * znear);

	Mat44 p_m(2.F / res_x, 0.F, 0.F, 0.F, 0.F, 2.F / res_y, 0.F, 0.F, 0.F, 0.F, qA, 0.F, -1.F, -1.F, qB, 1.F);

	if (!y_up) {
		p_m.m[1][1] *= -1.F;
		p_m.m[1][3] *= -1.F;
	}
	return p_m;
}

//
float ExtractZoomFactorFromProjectionMatrix(const Mat44 &m, const Vec2 &aspect_ratio) {
	return m.m[1][1] * aspect_ratio.y;
}

void ExtractZRangeFromPerspectiveProjectionMatrix(const Mat44 &m, float &znear, float &zfar) {
	const NDCInfos &ndc_infos = GetNDCInfos();
	znear = ndc_infos.homogeneous_depth ? (-m.m[2][3] / (m.m[2][2] + 1.F)) : (-m.m[2][3] / m.m[2][2]);
	zfar = -m.m[2][3] / (m.m[2][2] - 1.F);
}

void ExtractZRangeFromOrthographicProjectionMatrix(const Mat44 &m, float &znear, float &zfar) {
	const NDCInfos &ndc_infos = GetNDCInfos();
	znear = ndc_infos.homogeneous_depth ? (-(m.m[2][3] + 1.F) / m.m[2][2]) : (-m.m[2][3] / m.m[2][2]);
	zfar = -(m.m[2][3] - 1.F) / m.m[2][2];
}

void ExtractZRangeFromProjectionMatrix(const Mat44 &m, float &znear, float &zfar) {
	// assume that the projection is either a well formed perspective or orthographic projection matrix
	if (EqualZero(m.m[3][3])) {
		ExtractZRangeFromPerspectiveProjectionMatrix(m, znear, zfar);
	} else {
		ExtractZRangeFromOrthographicProjectionMatrix(m, znear, zfar);
	}
}

//
bool ProjectToClipSpace(const Mat44 &proj, const Vec3 &view, Vec3 &clip) {
	bool res;
	Vec4 ndc = proj * Vec4(view);

	if (ndc.w > 0.F) {
		clip = Vec3(ndc) / ndc.w;
		res = true;
	} else {
		res = false;
	}

	return res;
}

bool ProjectOrthoToClipSpace(const Mat44 &proj, const Vec3 &view, Vec3 &clip) {
	bool res;
	Vec4 ndc = proj * Vec4(view);

	if (ndc.w > 0.F) {
		clip = Vec3(ndc);
		res = true;
	} else {
		res = false;
	}

	return res;
}

bool UnprojectFromClipSpace(const Mat44 &inv_proj, const Vec3 &clip, Vec3 &view) {
	bool res;
	Vec4 ndc = inv_proj * Vec4(clip);

	if (ndc.w > 0.F) {
		view = Vec3(ndc) / ndc.w;
		res = true;
	} else {
		res = false;
	}

	return res;
}

bool UnprojectOrthoFromClipSpace(const Mat44 &inv_proj, const Vec3 &clip, Vec3 &view) {
	bool res;
	Vec4 ndc = inv_proj * Vec4(clip);

	if (ndc.w > 0.F) {
		view = Vec3(ndc);
		res = true;
	} else {
		res = false;
	}

	return res;
}

// NOTE [EJ] Z is a problem here... DX -> [0;1], GL -> [-1;1]
Vec3 ClipSpaceToScreenSpace(const Vec3 &clip, const Vec2 &res) {
	return Vec3((clip.x + 1.F) * res.x * 0.5F, (clip.y + 1.F) * res.y * 0.5F, clip.z);
}

Vec3 ScreenSpaceToClipSpace(const Vec3 &screen, const Vec2 &res) {
	return Vec3(screen.x / res.x * 2.F - 1.F, screen.y / res.y * 2.F - 1.F, screen.z);
}

//
bool ProjectToScreenSpace(const Mat44 &proj, const Vec3 &view, const Vec2 &resolution, Vec3 &screen) {
	bool res;
	Vec3 clip;

	if (ProjectToClipSpace(proj, view, clip)) {
		screen = ClipSpaceToScreenSpace(clip, resolution);
		res = true;
	} else {
		res = false;
	}

	return res;
}

bool ProjectOrthoToScreenSpace(const Mat44 &proj, const Vec3 &view, const Vec2 &resolution, Vec3 &screen) {
	bool res;
	Vec3 clip;

	if (ProjectOrthoToClipSpace(proj, view, clip)) {
		screen = ClipSpaceToScreenSpace(clip, resolution);
		res = true;
	} else {
		res = false;
	}

	return res;
}

bool UnprojectFromScreenSpace(const Mat44 &inv_proj, const Vec3 &screen, const Vec2 &res, Vec3 &view) {
	const Vec3 clip = ScreenSpaceToClipSpace(screen, res);
	return UnprojectFromClipSpace(inv_proj, clip, view);
}

bool UnprojectOrthoFromScreenSpace(const Mat44 &inv_proj, const Vec3 &screen, const Vec2 &res, Vec3 &view) {
	const Vec3 clip = ScreenSpaceToClipSpace(screen, res);
	return UnprojectOrthoFromClipSpace(inv_proj, clip, view);
}

//
float ProjectZToClipSpace(float z, const Mat44 &proj) {
	return (z * proj.m[2][2] + proj.m[2][3]) / z;
}

//
Vec2 ComputeAspectRatioX(float width, float height) {
	return Vec2(width / height, 1.F);
}

Vec2 ComputeAspectRatioY(float width, float height) {
	return Vec2(1.F, height / width);
}

//
bool WorldRaycastScreenPos(float x, float y, float width, float height, const Mat44 &inv_proj, const Mat4 &inv_view, Vec3 &ray_o, Vec3 &ray_d) {
	bool res;
	Vec3 view_pos;

	if (UnprojectFromScreenSpace(inv_proj, Vec3(x, y, 1.F), Vec2(width, height), view_pos)) {
		ray_o = GetT(inv_view);
		ray_d = inv_view * view_pos - ray_o;

		res = true;
	} else {
		res = false;
	}

	return res;
}

} // namespace hg
