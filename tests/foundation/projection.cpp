// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.
#include <math.h>

#define TEST_NO_MAIN
#include "acutest.h"

#include "foundation/projection.h"

#include "foundation/math.h"
#include "foundation/unit.h"

using namespace hg;

void test_projection() {
	{
		SetNDCInfos(true, false);
		TEST_CHECK(GetNDCInfos().origin_bottom_left == true);
		TEST_CHECK(GetNDCInfos().homogeneous_depth == false);

		SetNDCInfos(false, true);
		TEST_CHECK(GetNDCInfos().origin_bottom_left == false);
		TEST_CHECK(GetNDCInfos().homogeneous_depth == true);
	}
	{
		const float fov = Deg(60.f);
		const float zoom = sqrt(3.f);

		float z = FovToZoomFactor(fov);
		TEST_CHECK(TestEqual(z, zoom));
		
		float f = ZoomFactorToFov(zoom);
		TEST_CHECK(TestEqual(f, fov));
	}
	{
		SetNDCInfos(true, true);
		Mat44 m0 = ComputeOrthographicProjectionMatrix(1.0f, 21.f, 40.f, Vec2(16.f / 9.f, 1.f), Vec2(-1.5f, 1.5f));
		TEST_CHECK(AlmostEqual(GetRow(m0, 0), Vec4(0.028125f, 0.0f,  0.0f,-1.5f), 0.000001f));
		TEST_CHECK(AlmostEqual(GetRow(m0, 1), Vec4(0.0f,      0.05f, 0.0f, 1.5f), 0.000001f));
		TEST_CHECK(AlmostEqual(GetRow(m0, 2), Vec4(0.0f,      0.0f,  0.1f,-1.1f), 0.000001f));
		TEST_CHECK(AlmostEqual(GetRow(m0, 3), Vec4(0.0f,      0.0f,  0.0f, 1.0f), 0.000001f));

		SetNDCInfos(true, false);
		Mat44 m1 = ComputeOrthographicProjectionMatrix(1.0f, 21.f, 40.f, Vec2(16.f / 9.f, 1.f), Vec2(-1.5f, 1.5f));
		TEST_CHECK(AlmostEqual(GetRow(m1, 0), GetRow(m0, 0), 0.000001f));
		TEST_CHECK(AlmostEqual(GetRow(m1, 1), GetRow(m0, 1), 0.000001f));
		TEST_CHECK(AlmostEqual(GetRow(m1, 2), Vec4(0.0f, 0.0f, 0.05f,-0.05f), 0.000001f));
		TEST_CHECK(AlmostEqual(GetRow(m1, 3), GetRow(m0, 3), 0.000001f));
	}
	{
		SetNDCInfos(true, true);
		const float zoom = FovToZoomFactor(Deg(60.f));
		const Vec2 ar = ComputeAspectRatioX(16.f, 9.f);
		const Vec2 off = Vec2(-1.5f, 1.5f);
		const float znear = 1.f;
		const float zfar = 21.f;

		Mat44 m0 = ComputePerspectiveProjectionMatrix(znear, zfar, zoom, ar, off);
		TEST_CHECK(AlmostEqual(GetRow(m0, 0), Vec4(zoom/ar.x, 0.0f, 0.0f, off.x), 0.000001f));
		TEST_CHECK(AlmostEqual(GetRow(m0, 1), Vec4(0.0f, zoom/ar.y, 0.0f, off.y), 0.000001f));
		TEST_CHECK(AlmostEqual(GetRow(m0, 2), Vec4(0.0f, 0.0f, 1.1f, -2.1f), 0.000001f));
		TEST_CHECK(AlmostEqual(GetRow(m0, 3), Vec4(0.0f, 0.0f, 1.0f, 0.0f), 0.000001f));

		SetNDCInfos(true, false);
		Mat44 m1 = ComputePerspectiveProjectionMatrix(znear, zfar, zoom, ar, off);
		TEST_CHECK(AlmostEqual(GetRow(m1, 0), GetRow(m0, 0), 0.000001f));
		TEST_CHECK(AlmostEqual(GetRow(m1, 1), GetRow(m0, 1), 0.000001f));
		TEST_CHECK(AlmostEqual(GetRow(m1, 2), Vec4(0.0f, 0.0f, 1.05f, -1.05f), 0.000001f));
		TEST_CHECK(AlmostEqual(GetRow(m1, 3), GetRow(m0, 3), 0.000001f));
	}
	{ 
		const float zoom = FovToZoomFactor(Deg(60.f));
		{
			SetNDCInfos(true, true);
			const Vec2 ar = ComputeAspectRatioX(4.f, 3.f);
			Mat44 m = ComputePerspectiveProjectionMatrix(0.1f, 100.f, zoom, ar, Vec2::Zero);
			TEST_CHECK(TestEqual(ExtractZoomFactorFromProjectionMatrix(m, ar), zoom));
		}
		{
			SetNDCInfos(true, false);
			const Vec2 ar = ComputeAspectRatioY(4.f, 3.f);
			Mat44 m = ComputePerspectiveProjectionMatrix(0.1f, 100.f, zoom, ar, Vec2::Zero);
			TEST_CHECK(TestEqual(ExtractZoomFactorFromProjectionMatrix(m, ar), zoom));
		}
	}
	{
		const Vec2 ar = ComputeAspectRatioX(4.f, 3.f);
		const float zoom = FovToZoomFactor(Deg(60.f));
		const float z_near = 0.4f; 
		const float z_far = 20.f;
		float z0, z1;
		{
			SetNDCInfos(true, true);
			Mat44 m = ComputePerspectiveProjectionMatrix(z_near, z_far, zoom, ar, Vec2::One);
			ExtractZRangeFromPerspectiveProjectionMatrix(m, z0, z1);
			TEST_CHECK(TestEqual(z0, z_near));
			TEST_CHECK(TestEqual(z1, z_far, 0.0001f));
		}
		{
			SetNDCInfos(true, false);
			Mat44 m = ComputePerspectiveProjectionMatrix(z_near, z_far, zoom, ar, Vec2::Zero);
			ExtractZRangeFromPerspectiveProjectionMatrix(m, z0, z1);
			TEST_CHECK(TestEqual(z0, z_near));
			TEST_CHECK(TestEqual(z1, z_far, 0.0001f));
		}
	}
	{
		const Vec2 ar = ComputeAspectRatioX(4.f, 3.f);
		const float size = 32.f;
		const float z_near = 0.4f;
		const float z_far = 20.f;
		float z0, z1;

		{
			SetNDCInfos(true, true);
			Mat44 m = ComputeOrthographicProjectionMatrix(z_near, z_far, size, Vec2::One);
			ExtractZRangeFromOrthographicProjectionMatrix(m, z0, z1);
			TEST_CHECK(TestEqual(z0, z_near));
			TEST_CHECK(TestEqual(z1, z_far, 0.0001f));
		}
		{
			SetNDCInfos(true, false);
			Mat44 m = ComputeOrthographicProjectionMatrix(z_near, z_far, size, ar, Vec2::Zero);
			ExtractZRangeFromOrthographicProjectionMatrix(m, z0, z1);
			TEST_CHECK(TestEqual(z0, z_near));
			TEST_CHECK(TestEqual(z1, z_far, 0.0001f));
		}
	}
	{
		const Vec2 ar = ComputeAspectRatioX(4.f, 3.f);
		const float size = 32.f;
		const float zoom = FovToZoomFactor(Deg(60.f));
		const float z_near = 0.5f;
		const float z_far = 40.f;
		float z0, z1;
		{
			SetNDCInfos(true, true);
			Mat44 m = ComputePerspectiveProjectionMatrix(z_near, z_far, zoom, ar, Vec2::One);
			ExtractZRangeFromPerspectiveProjectionMatrix(m, z0, z1);
			TEST_CHECK(TestEqual(z0, z_near));
			TEST_CHECK(TestEqual(z1, z_far, 0.0001f));
		}
		{
			SetNDCInfos(true, false);
			Mat44 m = ComputePerspectiveProjectionMatrix(z_near, z_far, zoom, ar, Vec2::Zero);
			ExtractZRangeFromPerspectiveProjectionMatrix(m, z0, z1);
			TEST_CHECK(TestEqual(z0, z_near));
			TEST_CHECK(TestEqual(z1, z_far, 0.0001f));
		}
		{
			SetNDCInfos(true, true);
			Mat44 m = ComputeOrthographicProjectionMatrix(z_near, z_far, size, Vec2::One);
			ExtractZRangeFromProjectionMatrix(m, z0, z1);
			TEST_CHECK(TestEqual(z0, z_near));
			TEST_CHECK(TestEqual(z1, z_far, 0.0001f));
		}
		{
			SetNDCInfos(true, false);
			Mat44 m = ComputeOrthographicProjectionMatrix(z_near, z_far, size, ar, Vec2::Zero);
			ExtractZRangeFromProjectionMatrix(m, z0, z1);
			TEST_CHECK(TestEqual(z0, z_near));
			TEST_CHECK(TestEqual(z1, z_far, 0.0001f));
		}
	}
	//bool ProjectToClipSpace(const Mat44 &proj, const Vec3 &view, Vec3 &clip);
	//bool ProjectOrthoToClipSpace(const Mat44 &proj, const Vec3 &view, Vec3 &clip);
	//bool UnprojectFromClipSpace(const Mat44 &inv_proj, const Vec3 &clip, Vec3 &view);
	//bool UnprojectOrthoFromClipSpace(const Mat44 &inv_proj, const Vec3 &clip, Vec3 &view);
	//Vec3 ClipSpaceToScreenSpace(const Vec3 &clip, const Vec2 &res);
	//Vec3 ScreenSpaceToClipSpace(const Vec3 &screen, const Vec2 &res);
	//bool ProjectToScreenSpace(const Mat44 &proj, const Vec3 &view, const Vec2 &res, Vec3 &screen);
	//bool ProjectOrthoToScreenSpace(const Mat44 &proj, const Vec3 &view, const Vec2 &res, Vec3 &screen);
	//bool UnprojectFromScreenSpace(const Mat44 &inv_proj, const Vec3 &screen, const Vec2 &res, Vec3 &view);
	//bool UnprojectOrthoFromScreenSpace(const Mat44 &inv_proj, const Vec3 &screen, const Vec2 &res, Vec3 &view);
	{
		const Vec2 ar = ComputeAspectRatioX(4.f, 3.f);
		const float size = 32.f;
		const float zoom = FovToZoomFactor(Deg(60.f));
		const float z_near = 0.5f;
		const float z_far = 40.f;
		float z = Lerp(z_near, z_far, 0.4f);
		{
			SetNDCInfos(true, true);
			Mat44 m = ComputePerspectiveProjectionMatrix(z_near, z_far, zoom, ar, Vec2::One);
			TEST_CHECK(TestEqual(ProjectZToClipSpace(z_near, m), -1.f));
			TEST_CHECK(ProjectZToClipSpace(z_near-0.2f, m) < -1.f);
			TEST_CHECK(ProjectZToClipSpace(z_far+10.f, m) > 1.f);
			TEST_CHECK(TestEqual(ProjectZToClipSpace(z, m), (m.m[2][2]*z + m.m[2][3]) / z));
		}
		{
			SetNDCInfos(true, false);
			Mat44 m = ComputePerspectiveProjectionMatrix(z_near, z_far, zoom, ar, Vec2::One);
			TEST_CHECK(TestEqual(ProjectZToClipSpace(z_near, m), 0.f));
			TEST_CHECK(ProjectZToClipSpace(z_near - 0.2f, m) < 0.f);
			TEST_CHECK(ProjectZToClipSpace(z_far + 10.f, m) > 1.f);
			TEST_CHECK(TestEqual(ProjectZToClipSpace(z, m), (m.m[2][2] * z + m.m[2][3]) / z));
		}
	}
	{
		const float width = 1900.f;
		const float height = 1080.f;
		TEST_CHECK(TestEqual(ComputeAspectRatioX(width, height), Vec2(width / height, 1.f), 0.000001f));
		TEST_CHECK(TestEqual(ComputeAspectRatioY(width, height), Vec2(1.f, height / width), 0.000001f));
	}
	//bool WorldRaycastScreenPos(float x, float y, float width, float height, const Mat44 &inv_proj, const Mat4 &inv_view, Vec3 &ray_o, Vec3 &ray_d);
}